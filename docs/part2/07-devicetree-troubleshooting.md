---
layout: default
title: "Devicetree Troubleshooting"
parent: "Part 2: Build System"
nav_order: 7
---

# Devicetree Troubleshooting

Devicetree errors can produce cryptic, overwhelming compilation messages. This chapter helps you decode common errors and develop effective debugging strategies.

## "Gosh, I Got Hundreds of Cryptic Compilation Errors!"

If you see a wall of errors after modifying a devicetree overlay, don't panic. Devicetree errors cascade — one small mistake can produce dozens of compiler errors because the generated macros all depend on each other.

**Golden rule:** Always look at the **first error** and ignore the rest until you fix it.

### Example Cascade

A single typo in a node name can produce errors like:

```
error: 'DT_N_S_soc_S_i2c_40003000_S_sensor_48_P_reg_IDX_0_VAL_ADDRESS' undeclared
error: 'DT_N_S_soc_S_i2c_40003000_S_sensor_48_P_label' undeclared
error: implicit declaration of function 'DT_N_S_soc_S_i2c_40003000_S_sensor_48_P_...'
... 47 more errors ...
```

All of these stem from a single issue: the node `sensor@48` doesn't exist or has the wrong path.

## Common Devicetree Issues

### Node Not Found

**Symptom:**
```
error: '__device_dts_ord_XX' undeclared (first use in this function)
```

**Cause:** Your code references a device that doesn't exist in the devicetree.

**Diagnosis:**
```bash
# Check the generated devicetree
cat build/zephyr/zephyr.dts | grep -A 5 "your-node"

# Check available nodes
cat build/zephyr/zephyr.dts | grep "compatible ="
```

**Common fixes:**
- Verify the node exists in your board's `.dts` file
- Check your overlay applies to the correct node path
- Ensure the node has `status = "okay";`

### Property Not Found or Wrong Type

**Symptom:**
```
error: 'DT_N_S_soc_S_i2c_40003000_P_clock_frequency' undeclared
```

**Cause:** The property doesn't exist in the node, or the property name has a typo.

**Diagnosis:**
```bash
# Check the node's actual properties
cat build/zephyr/zephyr.dts | grep -A 20 "i2c@40003000"

# Check what the binding expects
find $ZEPHYR_BASE/dts/bindings -name "*.yaml" | xargs grep -l "your-compatible"
```

### Status Not "okay"

**Symptom:** Device exists in DTS but driver doesn't initialize.

Many nodes in SoC `.dtsi` files default to `status = "disabled";`. You must enable them:

```dts
/* In your overlay */
&i2c0 {
    status = "okay";
    /* your config here */
};
```

### Wrong Compatible String

**Symptom:** Node exists but the wrong (or no) driver binds to it.

```dts
/* WRONG — compatible doesn't match any binding */
my-sensor {
    compatible = "my-sensor";
};

/* CORRECT — use the vendor,device format */
my-sensor {
    compatible = "bosch,bme280";
};
```

Check available bindings:
```bash
ls $ZEPHYR_BASE/dts/bindings/ | head -20
find $ZEPHYR_BASE/dts/bindings -name "*.yaml" | xargs grep "compatible:" | sort
```

## Overlay Issues

### Overlay Not Applied

**Symptom:** Your overlay changes don't appear in the final devicetree.

**Diagnosis:**
```bash
# Check if your overlay was found
west build 2>&1 | grep -i "overlay\|DTC_OVERLAY"

# Compare the generated DTS with and without overlay
cat build/zephyr/zephyr.dts
```

**Common causes:**
1. **Wrong filename** — must be `<board>.overlay` or explicitly specified
2. **Wrong location** — must be in the app root or `boards/` directory
3. **Wrong node path** — the path in the overlay doesn't match the actual DTS

**Overlay search order:**
```
1. boards/<board>.overlay (in app directory)
2. <app>.overlay (same name as app directory)
3. -DDTC_OVERLAY_FILE=custom.overlay (explicit)
```

### Syntax Errors in Overlays

Devicetree syntax is picky. Watch for:

```dts
/* WRONG — missing semicolons */
&i2c0 {
    status = "okay"
    clock-frequency = <100000>
}

/* WRONG — using = for node references */
&i2c0 {
    sensor = &my_sensor;
};

/* CORRECT */
&i2c0 {
    status = "okay";
    clock-frequency = <100000>;
};
```

### Modifying vs. Replacing Properties

Overlays **merge** with the base DTS. Properties are replaced, not appended:

```dts
/* Base DTS has: reg = <0x40003000 0x1000>; */

/* This REPLACES the entire reg property */
&i2c0 {
    reg = <0x40003000 0x2000>;
};
```

To add child nodes, just define them — they're added to the existing node:

```dts
&i2c0 {
    /* This ADDS a child node, doesn't replace existing children */
    my_sensor: sensor@48 {
        compatible = "vendor,sensor";
        reg = <0x48>;
    };
};
```

## Binding Issues

### Missing Binding File

**Symptom:**
```
devicetree error: /soc/my-device: could not find binding for compatible 'vendor,my-device'
```

**Solution:** Create a binding YAML file or check that the module providing it is in your west manifest.

```yaml
# dts/bindings/vendor,my-device.yaml
description: My custom device
compatible: "vendor,my-device"
properties:
  reg:
    required: true
```

### Required Property Missing

**Symptom:**
```
devicetree error: /soc/i2c@.../sensor@48: missing required property 'interrupt-gpios'
```

**Solution:** Add the required property to your node:

```dts
sensor@48 {
    compatible = "vendor,sensor";
    reg = <0x48>;
    interrupt-gpios = <&gpio0 12 GPIO_ACTIVE_LOW>;  /* Add this */
};
```

Check what properties are required:
```bash
cat $ZEPHYR_BASE/dts/bindings/vendor,sensor.yaml
# Look for "required: true" properties
```

## Debugging Strategies

### Inspect the Generated Devicetree

The most important debugging tool is the generated DTS:

```bash
# Full merged devicetree (after all overlays applied)
cat build/zephyr/zephyr.dts

# Just the header file with all macros
cat build/zephyr/include/generated/devicetree_generated.h
```

### Use the Devicetree Viewer

```bash
# Interactive devicetree visualization (if available)
west build -t dt_viewer

# List all nodes
west build -t dt_report
```

### Check Node Existence in Code

```c
/* Compile-time check if a node exists */
#if DT_NODE_EXISTS(DT_NODELABEL(my_sensor))
    /* Node exists */
#else
    #error "my_sensor node not found in devicetree!"
#endif

/* Check if a node is enabled (status = "okay") */
#if DT_NODE_HAS_STATUS(DT_NODELABEL(my_sensor), okay)
    /* Node is enabled */
#endif
```

### Verbose DTC Output

```bash
# Show the devicetree compiler's preprocessing steps
west build -- -DDTC_FLAGS="-W all"
```

## Practical Tips and Tricks

### Tip 1: Start from Working Examples

Don't write overlays from scratch. Start from a working board DTS and modify:

```bash
# Find your board's DTS
find $ZEPHYR_BASE/boards -name "*.dts" | grep your_board

# Find sample overlays for your board
find $ZEPHYR_BASE/samples -name "*.overlay" | xargs grep -l "your_board"
```

### Tip 2: Use Node Labels

Always define and use node labels — they make overlays much easier:

```dts
/* In board DTS */
my_i2c: &i2c0 {
    status = "okay";
};

/* In overlay — reference by label */
&my_i2c {
    clock-frequency = <400000>;
};
```

### Tip 3: Check Property Types

Each property has a specific type. Common mistakes:

```dts
/* Strings use quotes */
label = "My Device";

/* Integers use angle brackets */
reg = <0x48>;
clock-frequency = <100000>;

/* Booleans are present/absent (no value) */
wakeup-source;

/* Phandles reference other nodes */
interrupt-parent = <&gpio0>;

/* Arrays use angle brackets with spaces */
reg = <0x40003000 0x1000>;
```

### Tip 4: Cross-Reference with Bindings

When in doubt, the binding YAML is the source of truth:

```bash
# Find the binding for a compatible
find $ZEPHYR_BASE/dts/bindings -name "*.yaml" | xargs grep -l "your-compatible"

# Read it to understand required/optional properties and their types
```

## Quick Reference: Error → Solution

| Error Pattern | Likely Cause | Fix |
|:--|:--|:--|
| `'DT_N_S_..._P_...' undeclared` | Property doesn't exist | Check node path and property name |
| `'__device_dts_ord_XX' undeclared` | Device not in DTS | Add node or enable with `status = "okay"` |
| `could not find binding` | Missing or wrong `compatible` | Check binding YAML exists |
| `missing required property` | Binding needs more props | Add required properties |
| `parse error` in DTS | Syntax error | Check semicolons, brackets, quotes |
| Overlay has no effect | Wrong file path/name | Check overlay naming and location |

## Next Steps

Continue to [Application Structure]({% link part2/08-application-structure.md %}) to learn about organizing Zephyr projects.
