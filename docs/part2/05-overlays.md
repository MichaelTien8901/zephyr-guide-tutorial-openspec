---
layout: default
title: "Overlays"
parent: "Part 2: Build System"
nav_order: 5
---

# Device Tree Overlays and Config Fragments

Overlays let you customize hardware description without modifying board files. Config fragments add board-specific Kconfig settings.

## What are Overlays?

Overlays modify the base device tree:
- Add new nodes
- Modify existing properties
- Enable/disable devices

```mermaid
flowchart LR
    A[Board DTS] --> B[+ App Overlay]
    B --> C[Final Device Tree]
```

## Creating an Overlay

### File Location

```
my-app/
├── CMakeLists.txt
├── prj.conf
├── app.overlay              # For all boards
└── boards/
    └── nrf52840dk_nrf52840.overlay  # Board-specific
```

### Basic Overlay Syntax

```dts
/* app.overlay or boards/board_name.overlay */

/* Enable a disabled peripheral */
&i2c0 {
    status = "okay";
};

/* Add a device to I2C bus */
&i2c0 {
    my_sensor: sensor@48 {
        compatible = "ti,tmp102";
        reg = <0x48>;
    };
};

/* Modify existing property */
&uart0 {
    current-speed = <115200>;
};
```

## Overlay Operations

### Enable Disabled Peripheral

```dts
/* The base DTS has status = "disabled" */
&spi1 {
    status = "okay";
};
```

### Add Child Node

```dts
&i2c0 {
    /* Add sensor to I2C bus */
    temp_sensor: tmp102@48 {
        compatible = "ti,tmp102";
        reg = <0x48>;
        label = "TMP102";
    };
};
```

### Modify Property

```dts
&uart0 {
    current-speed = <921600>;
};

&gpio0 {
    /* Change GPIO configuration */
};
```

### Delete Node

```dts
/delete-node/ &unwanted_node;
```

### Delete Property

```dts
&my_node {
    /delete-property/ unwanted-prop;
};
```

## Practical Examples

### Adding an I2C Sensor

```dts
/* boards/nrf52840dk_nrf52840.overlay */

&i2c0 {
    status = "okay";
    clock-frequency = <I2C_BITRATE_FAST>;

    bme280: bme280@76 {
        compatible = "bosch,bme280";
        reg = <0x76>;
        label = "BME280";
    };
};
```

### Adding SPI Flash

```dts
&spi0 {
    status = "okay";

    flash0: w25q128@0 {
        compatible = "jedec,spi-nor";
        reg = <0>;
        spi-max-frequency = <80000000>;
        jedec-id = [ef 40 18];
        size = <134217728>;  /* 128 Mbit = 16 MB */
        label = "W25Q128";
    };
};
```

### Custom GPIO Configuration

```dts
/ {
    /* Add custom buttons */
    buttons {
        compatible = "gpio-keys";

        button0: button_0 {
            gpios = <&gpio0 11 (GPIO_PULL_UP | GPIO_ACTIVE_LOW)>;
            label = "Push button switch 0";
        };
    };

    /* Add custom LEDs */
    leds {
        compatible = "gpio-leds";

        led0: led_0 {
            gpios = <&gpio0 13 GPIO_ACTIVE_LOW>;
            label = "Green LED 0";
        };
    };

    aliases {
        sw0 = &button0;
        led0 = &led0;
    };
};
```

### PWM Configuration

```dts
&pwm0 {
    status = "okay";
    pinctrl-0 = <&pwm0_default>;
    pinctrl-names = "default";
};

/ {
    pwmleds {
        compatible = "pwm-leds";

        pwm_led0: pwm_led_0 {
            pwms = <&pwm0 0 PWM_MSEC(20) PWM_POLARITY_INVERTED>;
        };
    };
};
```

## Configuration Fragments

### Board-Specific Kconfig

Create `boards/<board_name>.conf`:

```ini
# boards/nrf52840dk_nrf52840.conf

# Enable I2C
CONFIG_I2C=y

# Enable sensor
CONFIG_SENSOR=y
CONFIG_BME280=y

# Board-specific stack size
CONFIG_MAIN_STACK_SIZE=2048
```

### Multiple Overlay/Config Files

```bash
# Specify extra overlays
west build -b board -- \
    -DDTC_OVERLAY_FILE="custom.overlay;debug.overlay"

# Specify extra config
west build -b board -- \
    -DEXTRA_CONF_FILE="debug.conf;test.conf"
```

## Build System Integration

### Automatic Detection

The build system automatically finds:
- `app.overlay` in application root
- `boards/<board_name>.overlay`
- `boards/<board_name>.conf`

### CMakeLists.txt Override

```cmake
# Specify overlay in CMakeLists.txt
set(DTC_OVERLAY_FILE ${CMAKE_CURRENT_SOURCE_DIR}/my_overlay.dts)

# Multiple overlays
set(DTC_OVERLAY_FILE
    "${CMAKE_CURRENT_SOURCE_DIR}/base.overlay"
    "${CMAKE_CURRENT_SOURCE_DIR}/extra.overlay"
)
```

## Shields

Shields are standardized overlays for add-on boards:

```bash
# Build with shield
west build -b nrf52840dk_nrf52840 -- -DSHIELD=x_nucleo_iks01a3

# Multiple shields
west build -b board -- -DSHIELD="shield1;shield2"
```

### Shield Structure

```
shields/my_shield/
├── my_shield.overlay
├── Kconfig.shield
└── Kconfig.defconfig
```

## Debugging Overlays

### View Final Device Tree

```bash
# After build
cat build/zephyr/zephyr.dts
```

### View Preprocessed Output

```bash
cat build/zephyr/zephyr.dts.pre
```

### Check Overlay Applied

```bash
# Search for your additions
grep -A5 "my_sensor" build/zephyr/zephyr.dts
```

## Common Patterns

### Conditional Based on Board

```dts
/* Works if node exists */
&spi0 {
    status = "okay";
};

/* Or use chosen for board-specific */
/ {
    chosen {
        app-spi = &spi0;  /* Can differ per board overlay */
    };
};
```

### Re-pin a Peripheral

```dts
&uart1 {
    status = "okay";
    pinctrl-0 = <&uart1_custom>;
    pinctrl-names = "default";
};

&pinctrl {
    uart1_custom: uart1_custom {
        group1 {
            psels = <NRF_PSEL(UART_TX, 0, 20)>,
                    <NRF_PSEL(UART_RX, 0, 21)>;
        };
    };
};
```

## Troubleshooting

### Overlay Not Applied

1. Check file name matches board name exactly
2. Verify file is in correct location
3. Check build output for overlay processing

### Node Not Found

```
error: node 'i2c0' not found
```

Check the base board DTS for correct node label.

### Property Type Mismatch

```
error: expected array, got string
```

Check binding file for correct property type.

## Best Practices

1. **Use board-specific overlays** for hardware differences
2. **Keep overlays minimal** - only what's different from base
3. **Document pin assignments** in comments
4. **Test on multiple boards** to ensure portability

## Next Steps

Learn about [Application Structure]({% link part2/06-application-structure.md %}) to organize your Zephyr projects.
