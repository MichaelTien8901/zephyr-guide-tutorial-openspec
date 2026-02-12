---
layout: default
title: "Logging"
parent: "Part 6: Advanced Topics"
nav_order: 1
---

# Logging Subsystem

Zephyr's logging subsystem provides structured, filterable output that is far superior to `printk` for production applications. This chapter covers the logging APIs in depth.

## Why Logging vs. printk/printf

| Feature | printk | printf | LOG_* |
|:--|:--|:--|:--|
| Structured output | No | No | Yes |
| Log levels | No | No | Yes (ERR, WRN, INF, DBG) |
| Per-module filtering | No | No | Yes |
| Runtime level control | No | No | Yes |
| Multiple backends | No | No | Yes (UART, RTT, filesystem) |
| Deferred mode | No | No | Yes |
| Hex dump | No | No | Yes |
| Timestamps | No | No | Yes |
| ISR-safe (deferred) | Yes* | No | Yes |
| Code size impact | Small | Large | Configurable |

{: .note }
`printk` is useful for early boot messages and quick debugging, but for any real application, use the logging subsystem.

## Configuration

### Basic Setup

```ini
# prj.conf
CONFIG_LOG=y
CONFIG_LOG_DEFAULT_LEVEL=3       # 0=OFF, 1=ERR, 2=WRN, 3=INF, 4=DBG
CONFIG_LOG_MAX_LEVEL=4           # Maximum compiled-in level

# Backend (at least one required)
CONFIG_LOG_BACKEND_UART=y
```

### Processing Mode

Zephyr logging supports two modes:

```ini
# Deferred mode (default) — messages queued, processed later
CONFIG_LOG_MODE_DEFERRED=y
CONFIG_LOG_BUFFER_SIZE=4096      # Buffer for deferred messages

# Immediate mode — messages output synchronously (slower but simpler)
CONFIG_LOG_MODE_IMMEDIATE=y
```

**Deferred mode** is recommended for production:
- Minimal impact on calling thread's timing
- Safe to use in ISRs
- Messages may be lost if buffer overflows

**Immediate mode** is useful for debugging:
- Messages appear instantly
- Blocks the calling thread
- Can cause timing issues in real-time code

### Timestamp Configuration

```ini
CONFIG_LOG_TIMESTAMP_64BIT=y     # Use 64-bit timestamps
```

## Log Module Registration

Every source file that uses logging must register a log module:

```c
#include <zephyr/logging/log.h>

/* Register with default level from Kconfig */
LOG_MODULE_REGISTER(my_module);

/* Or register with an explicit compile-time level */
LOG_MODULE_REGISTER(my_module, LOG_LEVEL_DBG);
```

### Declaring a Module in Headers

If your module spans multiple files, declare it in the header and register in one `.c` file:

```c
/* my_module.h */
#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(my_module);

/* my_module.c */
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(my_module, LOG_LEVEL_INF);

/* my_module_utils.c */
#include "my_module.h"
/* Uses LOG_MODULE_DECLARE from the header */
```

## Log Levels

| Level | Macro | Value | Use For |
|:--|:--|:--|:--|
| OFF | — | 0 | Disable logging for module |
| ERROR | `LOG_ERR` | 1 | Unrecoverable errors |
| WARNING | `LOG_WRN` | 2 | Recoverable issues |
| INFO | `LOG_INF` | 3 | Normal operational messages |
| DEBUG | `LOG_DBG` | 4 | Detailed debug information |

### Level Filtering

Messages are filtered at two stages:

1. **Compile-time** (`CONFIG_LOG_MAX_LEVEL`) — messages above this level are completely removed from the binary
2. **Runtime** (`CONFIG_LOG_DEFAULT_LEVEL`) — messages above this level are suppressed but still compiled in

```ini
# Compile out DBG messages entirely (saves code size)
CONFIG_LOG_MAX_LEVEL=3

# Default runtime level is INF, but DBG is compiled in
CONFIG_LOG_MAX_LEVEL=4
CONFIG_LOG_DEFAULT_LEVEL=3
```

## Log APIs

### Basic Messages

```c
LOG_ERR("Sensor read failed: %d", err);
LOG_WRN("Battery low: %d%%", battery_pct);
LOG_INF("System initialized, version %s", APP_VERSION);
LOG_DBG("Entering state %d, counter=%u", state, count);
```

### Hex Dump

```c
uint8_t packet[] = { 0x01, 0x02, 0x03, 0x04, 0x05 };

LOG_HEXDUMP_INF(packet, sizeof(packet), "Received packet:");
LOG_HEXDUMP_DBG(buffer, len, "Raw data:");
```

Output:
```
[00:00:01.234] <inf> my_module: Received packet:
                                01 02 03 04 05                |.....
```

### Formatted Strings

The logging macros support standard printf format specifiers:

```c
LOG_INF("Integer: %d", value);
LOG_INF("Unsigned: %u", uvalue);
LOG_INF("Hex: 0x%08x", reg_value);
LOG_INF("String: %s", name);
LOG_INF("Float: %.2f", (double)temp);  /* Cast to double for %f */
LOG_INF("Pointer: %p", ptr);
```

{: .warning }
Float formatting requires `CONFIG_CBPRINTF_FP_SUPPORT=y` and significantly increases code size. Consider using integer math instead: `LOG_INF("Temp: %d.%02d C", temp / 100, temp % 100);`

### Conditional Logging

```c
/* Only log if condition is true */
LOG_INF_IF(verbose_mode, "Detailed status: %d", status);

/* Instance-based logging (for drivers with multiple instances) */
LOG_INST_INF(log, "Instance %d initialized", instance_id);
```

## Runtime Log Control

### Changing Levels at Runtime

```c
#include <zephyr/logging/log_ctrl.h>

/* Change the runtime filter for a module */
log_filter_set(NULL, 0, LOG_SOURCE_ID_GET(my_module), LOG_LEVEL_DBG);

/* Get current level */
uint32_t level = log_filter_get(NULL, 0, LOG_SOURCE_ID_GET(my_module), true);
```

### Via Shell Commands

If the shell is enabled, you can control logging at runtime:

```
uart:~$ log enable dbg my_module
uart:~$ log disable my_module
uart:~$ log status
```

### Panic Mode

In critical error handlers, switch to synchronous output to ensure messages are flushed:

```c
void k_sys_fatal_error_handler(unsigned int reason, const z_arch_esf_t *esf)
{
    LOG_PANIC();  /* Switch to synchronous mode, flush buffer */
    LOG_ERR("Fatal error: reason %u", reason);
    /* ... */
}
```

## Log Backends

Zephyr supports multiple simultaneous log backends:

### UART Backend

```ini
CONFIG_LOG_BACKEND_UART=y
```

### SEGGER RTT Backend

```ini
CONFIG_LOG_BACKEND_RTT=y
CONFIG_USE_SEGGER_RTT=y
```

### Filesystem Backend

```ini
CONFIG_LOG_BACKEND_FS=y
CONFIG_FILE_SYSTEM=y
```

### Network Backend (Syslog)

```ini
CONFIG_LOG_BACKEND_NET=y
CONFIG_NETWORKING=y
```

### Custom Backend

```c
#include <zephyr/logging/log_backend.h>
#include <zephyr/logging/log_output.h>

static uint8_t output_buf[256];

static int custom_output(uint8_t *buf, size_t size, void *ctx)
{
    /* Send to your custom destination */
    my_transport_send(buf, size);
    return size;
}

LOG_OUTPUT_DEFINE(custom_output_inst, custom_output, output_buf, sizeof(output_buf));

static void custom_process(const struct log_backend *const backend,
                           union log_msg_generic *msg)
{
    log_output_msg_process(&custom_output_inst, &msg->log, 0);
}

static const struct log_backend_api custom_api = {
    .process = custom_process,
};

LOG_BACKEND_DEFINE(custom_backend, custom_api, true);
```

## Custom Application Kconfig for Logging

Define module-specific log levels in your application's Kconfig:

```kconfig
# Kconfig file in your application
module = MY_APP
module-str = my_app
source "subsys/logging/Kconfig.template.log_config"
```

This auto-generates:
- `CONFIG_MY_APP_LOG_LEVEL` — integer log level
- `CONFIG_MY_APP_LOG_LEVEL_OFF`, `_ERR`, `_WRN`, `_INF`, `_DBG` — level choices

Users can then set in `prj.conf`:

```ini
CONFIG_MY_APP_LOG_LEVEL_DBG=y
```

## Performance Considerations

### Deferred Mode Buffer Sizing

```ini
# If you see "log dropped X messages", increase the buffer
CONFIG_LOG_BUFFER_SIZE=8192

# Or increase the processing thread priority
CONFIG_LOG_PROCESS_THREAD_PRIORITY=5
```

### Compile-Time Stripping

```ini
# Strip all logging for minimal footprint
CONFIG_LOG_MAX_LEVEL=0

# Strip debug messages only
CONFIG_LOG_MAX_LEVEL=3
```

### ISR Usage

In ISR context, always use deferred mode. Immediate mode will crash or cause timing issues:

```c
/* Safe in ISR with deferred mode */
void my_isr(void *arg)
{
    LOG_INF("ISR triggered");  /* Queued, processed later */
}
```

## Common Patterns

### Module with Sub-Components

```c
/* main_module.c */
LOG_MODULE_REGISTER(main_module, LOG_LEVEL_INF);

/* sub_component.c — shares the parent module name */
LOG_MODULE_DECLARE(main_module);
```

### Conditional Verbose Logging

```c
#ifdef CONFIG_MY_APP_VERBOSE
LOG_MODULE_REGISTER(my_app, LOG_LEVEL_DBG);
#else
LOG_MODULE_REGISTER(my_app, LOG_LEVEL_INF);
#endif
```

### Error Path Logging

```c
int sensor_init(void)
{
    int ret;

    ret = i2c_configure(dev, I2C_SPEED_SET(I2C_SPEED_FAST));
    if (ret < 0) {
        LOG_ERR("I2C configure failed: %d", ret);
        return ret;
    }

    LOG_INF("Sensor initialized successfully");
    return 0;
}
```

## Example Code

[View the complete logging example](https://github.com/MichaelTien8901/zephyr-guide-tutorial/tree/main/examples/part6/logging) — demonstrates log levels, hexdump, and multi-module logging.

```bash
west build -b qemu_cortex_m3 examples/part6/logging
west build -t run
```

## Next Steps

Continue to [Shell]({% link part6/02-shell.md %}) to learn about the interactive shell subsystem.
