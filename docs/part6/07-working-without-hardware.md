---
layout: default
title: "Working without Hardware"
parent: "Part 6: Advanced Topics"
nav_order: 7
---

# Working without Hardware

You don't always need physical hardware to develop Zephyr applications. QEMU and native_sim let you build, test, and debug on your host machine. This is invaluable for CI/CD pipelines, early prototyping, and testing logic before hardware is available.

## QEMU

QEMU is a hardware emulator that can run Zephyr binaries for various architectures.

### Supported Targets

| Board Target | Architecture | Typical Use |
|:--|:--|:--|
| `qemu_cortex_m3` | ARM Cortex-M3 | General embedded testing |
| `qemu_cortex_m0` | ARM Cortex-M0 | Constrained device testing |
| `qemu_cortex_a53` | ARM Cortex-A53 | 64-bit ARM testing |
| `qemu_x86` | x86 | x86-specific features |
| `qemu_riscv32` | RISC-V 32-bit | RISC-V development |
| `qemu_riscv64` | RISC-V 64-bit | RISC-V development |
| `qemu_xtensa` | Xtensa | DSP-like workloads |

### Building and Running

```bash
# Build for QEMU Cortex-M3
west build -b qemu_cortex_m3 samples/hello_world

# Run in QEMU (launches automatically)
west build -t run

# Exit QEMU: Ctrl+A, X
```

### QEMU with Networking

```bash
# Build networking sample
west build -b qemu_x86 samples/net/sockets/echo_server

# Run with host networking (TAP interface)
west build -t run

# From host, connect to the echo server
echo "Hello" | nc 192.0.2.1 4242
```

### QEMU with Debugging

```bash
# Start QEMU with GDB server
west build -t debugserver

# In another terminal, connect GDB
arm-zephyr-eabi-gdb build/zephyr/zephyr.elf
(gdb) target remote localhost:1234
(gdb) break main
(gdb) continue
```

### QEMU Limitations

- **No real peripherals** — GPIO, I2C, SPI are limited or absent
- **Timing differences** — QEMU timing doesn't match real hardware
- **Limited peripheral emulation** — some drivers won't work
- **No power management** — PM states aren't meaningfully simulated

## native_sim

`native_sim` (formerly `native_posix`) compiles Zephyr as a **native Linux executable**. This is the fastest way to iterate on application logic.

### How It Works

Instead of cross-compiling for an embedded target, native_sim:
- Compiles your Zephyr app as a Linux process
- Uses POSIX threads to simulate Zephyr threads
- Simulates timers using host time
- Can use host networking, Bluetooth (via HCI), and file systems

### Building and Running

```bash
# Build as native executable
west build -b native_sim samples/hello_world

# Run directly
./build/zephyr/zephyr.exe

# Or via west
west build -t run

# Pass arguments
./build/zephyr/zephyr.exe --stop_at=10  # Stop after 10 simulated seconds
```

### Advantages Over QEMU

| Feature | QEMU | native_sim |
|:--|:--|:--|
| Build speed | Normal cross-compile | Fast (native compiler) |
| Execution speed | Emulated (slower) | Native (fast) |
| Debugging | Remote GDB | Native GDB (direct) |
| Host integration | Limited | Full (files, networking) |
| Code coverage | Complex setup | Easy (gcov/lcov) |
| Address sanitizer | No | Yes (ASan, UBSan) |

### native_sim with Debugging

```bash
# Build with debug info
west build -b native_sim -- -DCONFIG_DEBUG=y

# Debug natively — no remote GDB needed!
gdb ./build/zephyr/zephyr.exe
(gdb) break main
(gdb) run
```

### native_sim with Address Sanitizer

```bash
# Detect memory errors
west build -b native_sim -- -DCONFIG_ASAN=y

# Run — ASan will report memory errors
./build/zephyr/zephyr.exe
```

## Emulated Devices

Both QEMU and native_sim support emulated devices for testing without hardware.

### Emulated I2C/SPI Devices

Zephyr provides an emulation framework for testing drivers:

```ini
# prj.conf
CONFIG_EMUL=y
CONFIG_I2C_EMUL=y
```

```dts
/* Overlay for emulated sensor */
/ {
    emul_i2c: emul-i2c {
        compatible = "zephyr,i2c-emul-controller";
        #address-cells = <1>;
        #size-cells = <0>;

        bme280_emul: bme280@76 {
            compatible = "bosch,bme280";
            reg = <0x76>;
        };
    };
};
```

### Emulated GPIO

```dts
/ {
    gpio_emul: gpio-emul {
        compatible = "zephyr,gpio-emul";
        gpio-controller;
        #gpio-cells = <2>;
        ngpios = <32>;
    };
};
```

### Using Emulated Devices in Tests

```c
#include <zephyr/drivers/emul.h>

void test_sensor_read(void)
{
    const struct emul *emul = EMUL_DT_GET(DT_NODELABEL(bme280_emul));
    const struct device *dev = DEVICE_DT_GET(DT_NODELABEL(bme280_emul));

    /* Set emulated sensor values */
    emul_bme280_set_temperature(emul, 2350);  /* 23.50°C */

    /* Read through normal driver API */
    struct sensor_value val;
    sensor_sample_fetch(dev);
    sensor_channel_get(dev, SENSOR_CHAN_AMBIENT_TEMP, &val);

    zassert_equal(val.val1, 23, "Expected 23, got %d", val.val1);
}
```

## Creating a Device Driver Emulator

For custom drivers, you can create your own emulator:

### Step 1: Define the Emulator API

```c
/* my_sensor_emul.h */
#include <zephyr/drivers/emul.h>

struct my_sensor_emul_data {
    int32_t temperature;
    int32_t pressure;
};

void my_sensor_emul_set_temperature(const struct emul *emul, int32_t temp);
void my_sensor_emul_set_pressure(const struct emul *emul, int32_t pressure);
```

### Step 2: Implement the Emulator

```c
/* my_sensor_emul.c */
#include "my_sensor_emul.h"
#include <zephyr/drivers/i2c_emul.h>

static int my_sensor_emul_transfer(const struct emul *emul,
                                    struct i2c_msg *msgs,
                                    int num_msgs, int addr)
{
    struct my_sensor_emul_data *data = emul->data;

    /* Respond to I2C reads with emulated data */
    if (msgs[0].flags & I2C_MSG_READ) {
        /* Return temperature register */
        msgs[0].buf[0] = (data->temperature >> 8) & 0xFF;
        msgs[0].buf[1] = data->temperature & 0xFF;
    }

    return 0;
}

void my_sensor_emul_set_temperature(const struct emul *emul, int32_t temp)
{
    struct my_sensor_emul_data *data = emul->data;
    data->temperature = temp;
}

static struct i2c_emul_api my_sensor_emul_api = {
    .transfer = my_sensor_emul_transfer,
};

static int my_sensor_emul_init(const struct emul *emul,
                                const struct device *parent)
{
    return 0;
}

#define MY_SENSOR_EMUL(n) \
    static struct my_sensor_emul_data my_sensor_data_##n; \
    EMUL_DT_INST_DEFINE(n, my_sensor_emul_init, \
                        &my_sensor_data_##n, NULL, \
                        &my_sensor_emul_api, NULL)

DT_INST_FOREACH_STATUS_OKAY(MY_SENSOR_EMUL)
```

## CI/CD Integration

### GitHub Actions with native_sim

```yaml
# .github/workflows/test.yml
name: Zephyr Tests
on: [push, pull_request]

jobs:
  test:
    runs-on: ubuntu-latest
    container:
      image: ghcr.io/zephyrproject-rtos/ci:latest
    steps:
      - uses: actions/checkout@v4
      - name: Build and test
        run: |
          west init -l .
          west update
          west build -b native_sim tests/my_test
          ./build/zephyr/zephyr.exe
```

### Running Twister on native_sim

```bash
# Run all tests that support native_sim
west twister -p native_sim

# Run specific test suite
west twister -p native_sim -T tests/my_tests/

# Generate JUnit report for CI
west twister -p native_sim --report-format junit
```

## When to Use What

| Scenario | Recommended | Why |
|:--|:--|:--|
| Unit tests | native_sim | Fast build, native debugging, code coverage |
| Integration tests | native_sim or QEMU | Depends on peripheral needs |
| Network protocol testing | QEMU (x86) | Host networking support |
| Bluetooth testing | native_sim | HCI over UART to host |
| Architecture-specific code | QEMU | Tests actual instruction set |
| CI/CD pipeline | native_sim | Fastest build and execution |
| Hardware timing validation | Real hardware | Emulators don't match timing |

## Next Steps

Continue to [Networking Overview]({% link part6/08-networking-overview.md %}) to learn about Zephyr's networking stack.
