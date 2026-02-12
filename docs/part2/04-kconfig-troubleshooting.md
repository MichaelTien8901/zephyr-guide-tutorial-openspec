---
layout: default
title: "Kconfig Troubleshooting"
parent: "Part 2: Build System"
nav_order: 4
---

# Kconfig Troubleshooting

Kconfig issues are among the most common problems developers face when working with Zephyr. This chapter covers the most frequent errors and how to resolve them.

## prj.conf Not Found

### Symptom

Your configuration options are completely ignored, and the build uses default values for everything.

### Common Causes

1. **Wrong file location** — `prj.conf` must be in the application root directory (same level as `CMakeLists.txt`)
2. **Wrong filename** — must be exactly `prj.conf` (case-sensitive on Linux)
3. **Build directory stale** — CMake cached an old path

### How to Diagnose

```bash
# Check that prj.conf is in the right place
ls -la your-app/
# Should show:
#   CMakeLists.txt
#   prj.conf
#   src/

# Verify CMake found it — look for "Using ... as CONF_FILE"
west build -b your_board your-app -- -DCMAKE_VERBOSE_MAKEFILE=ON 2>&1 | grep CONF_FILE
```

### Solutions

```bash
# 1. Make sure prj.conf is at the app root
mv src/prj.conf ./prj.conf

# 2. Clean build to reset CMake cache
west build -p always -b your_board your-app

# 3. If using a non-standard name, specify explicitly
west build -b your_board your-app -- -DCONF_FILE=my-custom.conf
```

{: .tip }
Always do a **pristine build** (`west build -p always`) when debugging Kconfig issues. Stale CMake caches are a frequent source of confusion.

## Configuration Option Not Applied

### Symptom

You set `CONFIG_SOMETHING=y` in `prj.conf`, but the feature isn't enabled in the build.

### Common Causes

1. **Unmet dependencies** — the option depends on another option that isn't enabled
2. **Board/SoC doesn't support it** — the option is gated by hardware capabilities
3. **Overridden by board defconfig** — board-level config takes precedence in some cases
4. **Typo in option name** — no error, just silently ignored

### How to Diagnose

```bash
# Check the final merged config
cat build/zephyr/.config | grep CONFIG_SOMETHING

# Use menuconfig to see dependencies
west build -t menuconfig
# Press '/' to search for the option
# Look at "Depends on:" and "Selected by:" fields

# Check for warnings during build
west build 2>&1 | grep -i "warning.*kconfig\|override"
```

### Understanding the Config Merge Order

Zephyr merges Kconfig from multiple sources in this order (later overrides earlier):

```
1. Kconfig defaults (defconfig values in Kconfig files)
2. SoC defconfig (soc/<arch>/<soc>/Kconfig.defconfig)
3. Board defconfig (boards/<vendor>/<board>/<board>_defconfig)
4. Application prj.conf
5. Overlay configs (-DOVERLAY_CONFIG=...)
6. Command line (-DCONFIG_SOMETHING=y)
```

{: .warning }
If a board defconfig uses `config FOO` with `default y` in a Kconfig.defconfig file (not a `_defconfig` file), it may appear to override your prj.conf. This is a Kconfig subtlety — `default` values have lower priority, but `select` statements are forced.

## Unmet Dependencies

### Symptom

Build output shows warnings like:

```
warning: FOO (defined at subsys/Kconfig:42) was assigned the value 'y'
but got the value 'n'. Check these unsatisfied dependencies: BAR, BAZ
```

### How to Read Dependency Chains

```
# In menuconfig, search for your option (press '/')
# The output shows:

Symbol: MY_FEATURE [=n]
Type  : bool
Prompt: Enable my feature
  Location:
    -> Subsystem XYZ
  Defined at subsys/xyz/Kconfig:15
  Depends on: NETWORKING [=y] && NET_TCP [=n] && NET_SOCKETS [=n]
  Selected by [n]:
    - OTHER_FEATURE [=n]
```

This tells you `MY_FEATURE` needs both `NET_TCP` and `NET_SOCKETS` enabled.

### Resolving Dependencies

```ini
# prj.conf - enable the full dependency chain
CONFIG_NETWORKING=y
CONFIG_NET_TCP=y
CONFIG_NET_SOCKETS=y
CONFIG_MY_FEATURE=y
```

{: .tip }
Some options use `select` to automatically pull in dependencies. Others use `depends on` which requires you to manually enable each dependency. When in doubt, use `menuconfig` to trace the full chain.

### Common Dependency Patterns

| Feature | Typical Dependencies |
|:--|:--|
| Bluetooth | `CONFIG_BT=y` requires `CONFIG_ENTROPY_GENERATOR=y` |
| Networking | `CONFIG_NETWORKING=y` often needs `CONFIG_NET_L2_*` for a specific L2 |
| Shell | `CONFIG_SHELL=y` requires a backend (e.g., `CONFIG_SHELL_BACKEND_SERIAL=y`) |
| Logging | `CONFIG_LOG=y` requires a backend (e.g., `CONFIG_LOG_BACKEND_UART=y`) |
| USB | `CONFIG_USB_DEVICE_STACK=y` requires SoC USB support |

## Build Errors After Config Changes

### Symbol Unknown Warnings

```
warning: attempt to assign the value 'y' to the undefined symbol FOO
```

**Causes:**
- Typo in the `CONFIG_` name
- Option was renamed or removed in a Zephyr version update
- The module providing this option isn't in your west manifest

**Solutions:**
```bash
# Search for the correct name
west build -t menuconfig
# Press '/' and search

# Check if the option exists in current Zephyr version
grep -r "config FOO" $ZEPHYR_BASE/Kconfig* $ZEPHYR_BASE/subsys/ $ZEPHYR_BASE/drivers/
```

### Type Mismatch Errors

```
error: Value "hello" for CONFIG_MY_INT is not a valid integer
```

Check the Kconfig definition — `int` options need integer values, `hex` needs hex values, `string` needs quoted strings:

```ini
# Correct types
CONFIG_MY_INT=42
CONFIG_MY_HEX=0x1000
CONFIG_MY_STRING="hello"
CONFIG_MY_BOOL=y
```

## Practical Debugging Workflow

When you hit a Kconfig issue, follow this systematic approach:

1. **Clean build** — `west build -p always -b board app`
2. **Check merged config** — `cat build/zephyr/.config | grep YOUR_OPTION`
3. **Search in menuconfig** — `west build -t menuconfig`, press `/`
4. **Read dependencies** — note every `Depends on:` item
5. **Enable dependencies** — add missing options to `prj.conf`
6. **Rebuild and verify** — check `.config` again

### Useful Build Targets

```bash
# Interactive config editor
west build -t menuconfig

# GUI config editor (if Qt is installed)
west build -t guiconfig

# Show all config differences from defaults
west build -t hardenconfig
```

## Common Pitfalls

### Extra Spaces or Characters

```ini
# WRONG — trailing space
CONFIG_FOO=y

# WRONG — comment on same line (some parsers handle this, some don't)
CONFIG_FOO=y # enable foo

# CORRECT
CONFIG_FOO=y
```

### Conflicting Overlays

When using multiple overlay files, later files override earlier ones:

```bash
# overlay-debug.conf sets CONFIG_LOG_DEFAULT_LEVEL=4
# overlay-release.conf sets CONFIG_LOG_DEFAULT_LEVEL=0
# Result: LOG_DEFAULT_LEVEL=0 (last one wins)
west build -b board app -- -DOVERLAY_CONFIG="overlay-debug.conf;overlay-release.conf"
```

### Board-Specific Fragments

Some configs only make sense for specific boards. Use board-specific fragments:

```
my-app/
├── prj.conf                    # Common config
├── boards/
│   ├── nrf52840dk_nrf52840.conf  # nRF52840-DK specific
│   └── stm32f769i_disco.conf     # STM32F769I-DISCO specific
└── src/
    └── main.c
```

Zephyr automatically merges `boards/<board>.conf` with `prj.conf`.

## Next Steps

Continue to [Device Tree]({% link part2/05-device-tree.md %}) to learn about hardware description in Zephyr.
