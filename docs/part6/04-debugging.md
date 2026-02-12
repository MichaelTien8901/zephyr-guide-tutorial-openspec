---
layout: default
title: "Debugging"
parent: "Part 6: Advanced Topics"
nav_order: 4
---

# Debugging Techniques

This chapter covers GDB debugging, fault analysis, thread diagnostics, and memory debugging. For logging, shell, and tracing, see the dedicated chapters:

- [Logging]({% link part6/01-logging.md %}) — structured log output, levels, backends
- [Shell]({% link part6/02-shell.md %}) — interactive command-line interface
- [Tracing]({% link part6/03-tracing.md %}) — kernel event tracing and visualization

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

### VS Code Debugging

Configure `.vscode/launch.json` for integrated debugging:

```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Zephyr Debug (J-Link)",
            "type": "cortex-debug",
            "request": "launch",
            "servertype": "jlink",
            "device": "STM32F769NI",
            "executable": "${workspaceFolder}/build/zephyr/zephyr.elf",
            "interface": "swd",
            "runToEntryPoint": "main"
        },
        {
            "name": "Zephyr Debug (OpenOCD)",
            "type": "cortex-debug",
            "request": "launch",
            "servertype": "openocd",
            "executable": "${workspaceFolder}/build/zephyr/zephyr.elf",
            "configFiles": ["board/stm32f769i_disco.cfg"]
        }
    ]
}
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

### Fatal Error Handler

```c
#include <zephyr/fatal.h>

void k_sys_fatal_error_handler(unsigned int reason,
                               const z_arch_esf_t *esf)
{
    printk("Fatal error: %u\n", reason);

    /* Switch to synchronous logging to flush buffer */
    LOG_PANIC();

    /* Can reset or halt */
    /* sys_reboot(SYS_REBOOT_COLD); */

    CODE_UNREACHABLE;
}
```

### Core Dump

Zephyr supports core dumps for post-mortem analysis:

```kconfig
CONFIG_DEBUG_COREDUMP=y
CONFIG_DEBUG_COREDUMP_BACKEND_LOGGING=y
# Or store to flash
CONFIG_DEBUG_COREDUMP_BACKEND_FLASH_PARTITION=y
```

When a crash occurs, the core dump is output through the configured backend. You can analyze it with GDB:

```bash
# Extract core dump from log output
python3 scripts/coredump/coredump_serial_log_parser.py log.txt core.bin

# Analyze with GDB
arm-zephyr-eabi-gdb build/zephyr/zephyr.elf -c core.bin
(gdb) bt              # Backtrace at crash
(gdb) info registers  # Register state
(gdb) list            # Source code at crash location
```

## Memory Debugging

### Stack Canaries

```kconfig
CONFIG_STACK_CANARIES=y
CONFIG_STACK_SENTINEL=y
```

### Stack Overflow Detection

```kconfig
# Hardware-based detection (recommended)
CONFIG_HW_STACK_PROTECTION=y

# Software sentinel (fallback)
CONFIG_STACK_SENTINEL=y
```

When a stack overflow is detected:
```
[00:00:01.234] <err> os: ***** USAGE FAULT *****
[00:00:01.234] <err> os: Stack overflow (context area at 0x20001000)
```

**Fix**: Increase the thread's stack size:
```c
/* Before: too small */
K_THREAD_STACK_DEFINE(my_stack, 512);

/* After: larger stack */
K_THREAD_STACK_DEFINE(my_stack, 2048);
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

**Fix**: Increase thread stack size. Use thread analyzer to find actual usage.

### Hard Fault

```
[00:00:01.234] <err> os: ***** HARD FAULT *****
```

**Common causes**:
- NULL pointer dereference
- Unaligned access
- Invalid memory access
- Stack overflow (without HW protection)

**Diagnosis**: Check the Program Counter (PC) and Link Register (LR) values in the fault dump. Map addresses to source using:
```bash
arm-zephyr-eabi-addr2line -e build/zephyr/zephyr.elf 0x00012345
```

### Assertion Failure

```
ASSERTION FAIL @ file.c:42
```

**Fix**: Check the condition and calling code. Enable `CONFIG_ASSERT_VERBOSE=y` for more details.

## Tips for Devices in the Field

1. **Enable core dump** — configure flash-based core dump storage for post-mortem analysis
2. **Use LOG_PANIC()** — switch to synchronous logging in fatal handlers
3. **Log boot count** — track unexpected reboots using NVS
4. **Watchdog timer** — reset if application hangs
5. **Remote logging** — send logs over network if possible
6. **Minimal fatal handler** — save diagnostic data, then reboot cleanly

## Best Practices

1. **Use logging** — structured output, not just printk (see [Logging]({% link part6/01-logging.md %}))
2. **Enable shell** — runtime inspection (see [Shell]({% link part6/02-shell.md %}))
3. **Add assertions** — catch bugs early with `__ASSERT`
4. **Check stack sizes** — use thread analyzer regularly
5. **Build with debug** — `CONFIG_DEBUG=y` for meaningful backtraces
6. **Enable HW stack protection** — catches overflows before they corrupt memory

## Next Steps

Learn about [Testing]({% link part6/05-testing.md %}) for automated testing with Ztest and Twister.
