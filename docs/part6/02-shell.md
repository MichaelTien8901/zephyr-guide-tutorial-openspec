---
layout: default
title: "Shell"
parent: "Part 6: Advanced Topics"
nav_order: 2
---

# Shell Subsystem

Zephyr's shell subsystem provides an interactive command-line interface for runtime inspection, configuration, and debugging. It's invaluable during development and can also serve as a management interface in production.

## Overview

The shell provides:

- **Built-in commands** — thread info, device list, kernel stats, log control
- **Custom commands** — add application-specific commands
- **Tab completion** — auto-complete commands and subcommands
- **Command history** — navigate previous commands
- **Multiple backends** — UART, RTT, Telnet, USB CDC ACM

## Configuration

### Basic Setup

```ini
# prj.conf
CONFIG_SHELL=y

# Choose a backend (at least one required)
CONFIG_SHELL_BACKEND_SERIAL=y    # UART (most common)

# Optional features
CONFIG_SHELL_HISTORY=y
CONFIG_SHELL_HISTORY_BUFFER=512
CONFIG_SHELL_TAB=y
CONFIG_SHELL_TAB_AUTOCOMPLETION=y
CONFIG_SHELL_CMDS=y              # Enable built-in commands
```

### Backend Options

```ini
# UART/Serial backend
CONFIG_SHELL_BACKEND_SERIAL=y

# SEGGER RTT backend
CONFIG_SHELL_BACKEND_RTT=y
CONFIG_USE_SEGGER_RTT=y

# Telnet backend (requires networking)
CONFIG_SHELL_BACKEND_TELNET=y
CONFIG_NETWORKING=y

# USB CDC ACM backend
CONFIG_SHELL_BACKEND_SERIAL=y
CONFIG_USB_DEVICE_STACK=y
CONFIG_UART_LINE_CTRL=y
```

## Built-in Shell Commands

Once the shell is enabled, you get many useful commands out of the box:

### Kernel Commands

```
uart:~$ kernel threads
 Thread          Stack    Used   Prio  State
 main            2048     512    0     Ready
 idle            256      64     15    Ready
 logging         768      256    14    Pending

uart:~$ kernel stacks
 Thread          Stack Size   Used   Max Used  Prio
 main            2048         512    784       0

uart:~$ kernel uptime
 Uptime: 12345 ms

uart:~$ kernel version
 Zephyr version 3.6.0

uart:~$ kernel reboot cold
 # Reboots the system
```

### Device Commands

```
uart:~$ device list
 devices:
 - gpio@50000000 (READY)
 - uart@40002000 (READY)
 - i2c@40003000 (READY)
 - spi@40004000 (READY)
```

### Log Commands

```
uart:~$ log status
 Module          Level
 my_app          INF
 net             WRN
 bt              ERR

uart:~$ log enable dbg my_app
uart:~$ log disable net
```

### Help

```
uart:~$ help
 Available commands:
  clear   : Clear screen
  device  : Device commands
  help    : Show help
  history : Command history
  kernel  : Kernel commands
  log     : Log commands
  resize  : Console resize

uart:~$ help kernel
 kernel - Kernel commands
 Subcommands:
  reboot   : Reboot
  stacks   : Thread stack info
  threads  : Thread list
  uptime   : Kernel uptime
  version  : Kernel version
```

## Custom Shell Commands

### Simple Command

```c
#include <zephyr/shell/shell.h>

static int cmd_hello(const struct shell *sh, size_t argc, char **argv)
{
    shell_print(sh, "Hello from Zephyr!");
    return 0;
}

/* Register a top-level command */
SHELL_CMD_REGISTER(hello, NULL, "Say hello", cmd_hello);
```

Usage:
```
uart:~$ hello
Hello from Zephyr!
```

### Command with Arguments

```c
static int cmd_set_value(const struct shell *sh, size_t argc, char **argv)
{
    if (argc < 2) {
        shell_error(sh, "Usage: set_value <number>");
        return -EINVAL;
    }

    int value = atoi(argv[1]);
    shell_print(sh, "Value set to %d", value);
    return 0;
}

/* mandatory_args=2 (command + 1 arg), optional_args=0 */
SHELL_CMD_ARG_REGISTER(set_value, NULL, "Set a value", cmd_set_value, 2, 0);
```

### Subcommand Groups

```c
static int cmd_led_on(const struct shell *sh, size_t argc, char **argv)
{
    shell_print(sh, "LED on");
    return 0;
}

static int cmd_led_off(const struct shell *sh, size_t argc, char **argv)
{
    shell_print(sh, "LED off");
    return 0;
}

static int cmd_led_toggle(const struct shell *sh, size_t argc, char **argv)
{
    shell_print(sh, "LED toggled");
    return 0;
}

/* Create subcommand set */
SHELL_STATIC_SUBCMD_SET_CREATE(led_cmds,
    SHELL_CMD(on, NULL, "Turn LED on", cmd_led_on),
    SHELL_CMD(off, NULL, "Turn LED off", cmd_led_off),
    SHELL_CMD(toggle, NULL, "Toggle LED", cmd_led_toggle),
    SHELL_SUBCMD_SET_END
);

/* Register parent command with subcommands */
SHELL_CMD_REGISTER(led, &led_cmds, "LED control commands", NULL);
```

Usage:
```
uart:~$ led on
LED on
uart:~$ led toggle
LED toggled
uart:~$ led <TAB>
  off     on      toggle
```

### Nested Subcommands

```c
SHELL_STATIC_SUBCMD_SET_CREATE(sensor_temp_cmds,
    SHELL_CMD(read, NULL, "Read temperature", cmd_temp_read),
    SHELL_CMD(calibrate, NULL, "Calibrate sensor", cmd_temp_cal),
    SHELL_SUBCMD_SET_END
);

SHELL_STATIC_SUBCMD_SET_CREATE(sensor_cmds,
    SHELL_CMD(temp, &sensor_temp_cmds, "Temperature sensor", NULL),
    SHELL_CMD(humidity, NULL, "Read humidity", cmd_humidity_read),
    SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(sensor, &sensor_cmds, "Sensor commands", NULL);
```

Usage:
```
uart:~$ sensor temp read
Temperature: 23.5 C
uart:~$ sensor temp calibrate
Calibrating...
```

## Shell Output Functions

```c
/* Normal output */
shell_print(sh, "Status: OK");

/* Error output (highlighted in red if color enabled) */
shell_error(sh, "Failed to read sensor: %d", err);

/* Warning output */
shell_warn(sh, "Battery low: %d%%", pct);

/* Info output */
shell_info(sh, "System ready");

/* Raw data output (no newline appended) */
shell_fprintf(sh, SHELL_NORMAL, "Progress: %d%%\r", progress);
```

## Dynamic Commands

For commands that depend on runtime state (e.g., listing available sensors):

```c
static void get_dynamic_entries(size_t idx, struct shell_static_entry *entry)
{
    if (idx < num_registered_sensors) {
        entry->syntax = sensor_names[idx];
        entry->help = "Read this sensor";
        entry->handler = cmd_read_sensor;
        entry->subcmd = NULL;
    } else {
        entry->syntax = NULL;  /* End of list */
    }
}

SHELL_DYNAMIC_CMD_CREATE(sensor_list, get_dynamic_entries);
SHELL_CMD_REGISTER(read, &sensor_list, "Read sensor", NULL);
```

## Shell in Production

### Security Considerations

```ini
# Require login before shell access
CONFIG_SHELL_START_OBSCURED=y

# Limit shell to specific backends
CONFIG_SHELL_BACKEND_SERIAL=y
# Don't enable Telnet shell in production without authentication
```

### Resource Usage

The shell adds code size and RAM usage:

| Feature | Typical Cost |
|:--|:--|
| Shell core | ~8-12 KB flash, ~2 KB RAM |
| Tab completion | ~1-2 KB flash |
| History | Configurable (CONFIG_SHELL_HISTORY_BUFFER) |
| Each command | ~100-200 bytes flash |

### Conditional Compilation

```c
#ifdef CONFIG_SHELL
SHELL_CMD_REGISTER(debug, &debug_cmds, "Debug commands", NULL);
#endif
```

## Best Practices

1. **Use subcommand groups** — organize related commands hierarchically
2. **Validate arguments** — check argc and provide usage messages
3. **Return error codes** — return 0 for success, negative errno for errors
4. **Add help strings** — every command should have a brief description
5. **Use shell output functions** — `shell_print` instead of `printk` for command output
6. **Keep handlers fast** — don't block the shell thread with long operations

## Next Steps

Continue to [Tracing]({% link part6/03-tracing.md %}) to learn about the tracing subsystem for performance analysis.
