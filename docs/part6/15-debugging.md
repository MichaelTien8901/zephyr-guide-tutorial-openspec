---
layout: default
title: "Debugging"
parent: "Part 6: Advanced Topics"
nav_order: 15
---

# Debugging Techniques

Zephyr provides various tools for debugging and diagnosing issues.

## Logging

### Basic Logging

```c
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(my_module, LOG_LEVEL_DBG);

void my_function(void)
{
    LOG_INF("Information message");
    LOG_DBG("Debug message");
    LOG_WRN("Warning message");
    LOG_ERR("Error message");

    /* With format arguments */
    int value = 42;
    LOG_INF("Value is %d", value);

    /* Hex dump */
    uint8_t data[] = { 0x01, 0x02, 0x03 };
    LOG_HEXDUMP_INF(data, sizeof(data), "Data:");
}
```

### Kconfig for Logging

```kconfig
CONFIG_LOG=y
CONFIG_LOG_DEFAULT_LEVEL=3  # INF
CONFIG_LOG_MAX_LEVEL=4      # DBG

# Per-module level
CONFIG_MY_MODULE_LOG_LEVEL_DBG=y

# Backends
CONFIG_LOG_BACKEND_UART=y
CONFIG_LOG_BACKEND_RTT=y  # For SEGGER RTT
```

### Runtime Log Control

```c
/* Change log level at runtime */
log_filter_set(NULL, 0, LOG_SOURCE_ID_GET(my_module), LOG_LEVEL_DBG);

/* Panic mode (synchronous output) */
LOG_PANIC();
```

## Shell

### Enabling Shell

```kconfig
CONFIG_SHELL=y
CONFIG_SHELL_BACKEND_SERIAL=y

# Optional features
CONFIG_SHELL_HISTORY=y
CONFIG_SHELL_TAB=y
CONFIG_SHELL_TAB_AUTOCOMPLETION=y
```

### Custom Shell Commands

```c
#include <zephyr/shell/shell.h>

static int cmd_status(const struct shell *sh, size_t argc, char **argv)
{
    shell_print(sh, "System status: OK");
    shell_print(sh, "Uptime: %lld ms", k_uptime_get());
    return 0;
}

static int cmd_led(const struct shell *sh, size_t argc, char **argv)
{
    if (argc < 2) {
        shell_error(sh, "Usage: myapp led <on|off>");
        return -EINVAL;
    }

    if (strcmp(argv[1], "on") == 0) {
        gpio_pin_set_dt(&led, 1);
        shell_print(sh, "LED on");
    } else if (strcmp(argv[1], "off") == 0) {
        gpio_pin_set_dt(&led, 0);
        shell_print(sh, "LED off");
    } else {
        shell_error(sh, "Invalid argument");
        return -EINVAL;
    }

    return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(myapp_cmds,
    SHELL_CMD(status, NULL, "Show status", cmd_status),
    SHELL_CMD_ARG(led, NULL, "Control LED", cmd_led, 2, 0),
    SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(myapp, &myapp_cmds, "My application commands", NULL);
```

### Built-in Shell Commands

```
uart:~$ help
uart:~$ kernel threads
uart:~$ kernel stacks
uart:~$ kernel uptime
uart:~$ device list
uart:~$ log enable dbg my_module
```

## GDB Debugging

### Build with Debug Info

```bash
west build -b <board> -- -DCONFIG_DEBUG=y -DCONFIG_DEBUG_OPTIMIZATIONS=y
```

### Start GDB Server

```bash
# For J-Link
JLinkGDBServer -device <device> -if SWD

# For OpenOCD
openocd -f board/<board>.cfg

# Using west
west debug --runner jlink
```

### GDB Commands

```gdb
# Connect
target remote localhost:2331

# Load symbols
file build/zephyr/zephyr.elf

# Breakpoints
break main
break my_function
break file.c:42

# Run
continue
step
next
finish

# Inspect
print variable
print *pointer
print array[0]@10
info threads
thread 2
backtrace

# Memory
x/10xb 0x20000000  # 10 hex bytes
x/4xw 0x20000000   # 4 hex words
```

## Thread Analysis

### Stack Usage

```kconfig
CONFIG_THREAD_ANALYZER=y
CONFIG_THREAD_ANALYZER_AUTO=y
CONFIG_THREAD_ANALYZER_AUTO_INTERVAL=10
```

```c
#include <zephyr/debug/thread_analyzer.h>

/* Manual analysis */
thread_analyzer_print();
```

### Thread Info

```c
/* Print thread info */
void print_threads(void)
{
    k_thread_foreach(thread_cb, NULL);
}

void thread_cb(const struct k_thread *thread, void *user_data)
{
    const char *name = k_thread_name_get((k_tid_t)thread);
    printk("Thread: %s, prio: %d\n",
           name ? name : "unnamed",
           k_thread_priority_get((k_tid_t)thread));
}
```

## Assert and Fault Handling

### Assertions

```c
#include <zephyr/sys/__assert.h>

void process_data(void *data)
{
    __ASSERT(data != NULL, "Data pointer is NULL");

    /* Continue processing */
}

/* Conditional assert */
__ASSERT_NO_MSG(condition);
```

### Fault Handlers

```c
#include <zephyr/fatal.h>

void k_sys_fatal_error_handler(unsigned int reason,
                               const z_arch_esf_t *esf)
{
    printk("Fatal error: %u\n", reason);

    /* Log additional info */
    LOG_PANIC();

    /* Can reset or halt */
    /* sys_reboot(SYS_REBOOT_COLD); */

    CODE_UNREACHABLE;
}
```

## Tracing

```kconfig
CONFIG_TRACING=y
CONFIG_TRACING_CTF=y  # Common Trace Format
# Or
CONFIG_SEGGER_SYSTEMVIEW=y
```

```c
#include <zephyr/tracing/tracing.h>

void my_operation(void)
{
    sys_trace_isr_enter();
    /* ISR code */
    sys_trace_isr_exit();
}
```

## Memory Debugging

### Stack Canaries

```kconfig
CONFIG_STACK_CANARIES=y
CONFIG_STACK_SENTINEL=y
```

### Memory Protection

```kconfig
CONFIG_HW_STACK_PROTECTION=y
CONFIG_USERSPACE=y  # For MPU-based protection
```

### Heap Tracking

```kconfig
CONFIG_SYS_HEAP_RUNTIME_STATS=y
```

```c
struct sys_memory_stats stats;
sys_heap_runtime_stats_get(&_system_heap, &stats);
printk("Heap: %zu used, %zu free\n", stats.allocated_bytes, stats.free_bytes);
```

## Common Issues

### Stack Overflow

```
[00:00:01.234] <err> os: ***** USAGE FAULT *****
[00:00:01.234] <err> os: Stack overflow
```

**Fix**: Increase thread stack size in definition.

### Hard Fault

```
[00:00:01.234] <err> os: ***** HARD FAULT *****
```

**Common causes**:
- NULL pointer dereference
- Unaligned access
- Invalid memory access

### Assertion Failure

```
ASSERTION FAIL @ file.c:42
```

**Fix**: Check the condition and calling code.

## Best Practices

1. **Use logging** - Not just printk
2. **Enable shell** - For runtime inspection
3. **Add assertions** - Catch bugs early
4. **Check stack sizes** - Use thread analyzer
5. **Build with debug** - For meaningful backtraces

## Next Steps

Learn about [Testing]({% link part6/16-testing.md %}) for automated testing.
