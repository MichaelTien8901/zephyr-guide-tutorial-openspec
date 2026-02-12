---
layout: default
title: "Tracing"
parent: "Part 6: Advanced Topics"
nav_order: 3
---

# Tracing Subsystem

Zephyr's tracing subsystem allows you to record and visualize kernel events — thread switches, ISR entry/exit, semaphore operations, and more. Tracing is essential for understanding real-time behavior and diagnosing timing issues.

## Why Tracing?

Logging tells you *what* happened. Tracing tells you *when* it happened and how events relate in time:

| Approach | What It Shows | Overhead | Best For |
|:--|:--|:--|:--|
| printk/LOG | Text messages | Medium-High | Functional debugging |
| Tracing | Timestamped kernel events | Low | Timing analysis, scheduling |
| GDB | Code execution, variables | High (halts CPU) | Logic bugs |

## Configuration

### Common Trace Format (CTF)

CTF is an open binary format that can be analyzed with tools like Babeltrace and TraceCompass:

```ini
# prj.conf
CONFIG_TRACING=y
CONFIG_TRACING_CTF=y

# Choose transport
CONFIG_TRACING_BACKEND_UART=y
# Or
CONFIG_TRACING_BACKEND_USB=y
# Or
CONFIG_TRACING_BACKEND_RAM=y    # Store in RAM buffer
```

### SEGGER SystemView

SystemView provides a real-time visual trace viewer with a GUI:

```ini
# prj.conf
CONFIG_TRACING=y
CONFIG_SEGGER_SYSTEMVIEW=y
CONFIG_USE_SEGGER_RTT=y

# Optional: increase RTT buffer for trace data
CONFIG_SEGGER_RTT_BUFFER_SIZE_UP=4096
```

### Percepio Tracealyzer

```ini
# prj.conf
CONFIG_TRACING=y
CONFIG_PERCEPIO_TRACERECORDER=y
```

## What Gets Traced

The tracing subsystem automatically records:

| Event Category | Events |
|:--|:--|
| **Thread** | Create, switch, ready, pend, suspend, resume, abort |
| **ISR** | Enter, exit |
| **Semaphore** | Init, give, take |
| **Mutex** | Init, lock, unlock |
| **Timer** | Start, stop, expire |
| **Work** | Submit, execute |
| **Syscall** | Enter, exit (with userspace) |

## Using CTF Tracing

### Capturing Traces

```bash
# Build with CTF tracing
west build -b your_board your_app -- -DCONFIG_TRACING=y -DCONFIG_TRACING_CTF=y

# For UART backend, capture the trace output
# (use a separate UART from the console if possible)
cat /dev/ttyACM1 > trace.ctf

# For RAM backend, use GDB to dump the buffer
(gdb) dump binary memory trace.ctf &tracing_buffer &tracing_buffer+tracing_buffer_size
```

### Analyzing CTF Traces

```bash
# Install babeltrace2
sudo apt install babeltrace2

# View trace events
babeltrace2 trace.ctf

# Use TraceCompass for visual analysis
# Download: https://www.eclipse.org/tracecompass/
```

## Using SEGGER SystemView

### Setup

1. Install [SEGGER SystemView](https://www.segger.com/products/development-tools/systemview/) on your host
2. Connect J-Link debugger to your board
3. Build with SystemView enabled
4. Start SystemView and connect

### Real-Time Viewing

SystemView shows:
- **Timeline** — visual representation of thread execution
- **CPU load** — per-thread CPU usage
- **ISR timing** — interrupt latency and duration
- **Context switches** — when and why threads switch

### Configuration Tuning

```ini
# Increase buffer for high-event-rate systems
CONFIG_SEGGER_RTT_BUFFER_SIZE_UP=8192

# System description for better labels
CONFIG_SEGGER_SYSTEMVIEW_APP_TITLE="My Application"
```

## Custom Trace Points

Add your own trace events to capture application-specific behavior:

### User Events

```c
#include <zephyr/tracing/tracing.h>

void my_function(void)
{
    sys_trace_isr_enter();
    /* Your ISR code */
    sys_trace_isr_exit();
}
```

### With SEGGER SystemView

```c
#include <SEGGER_SYSVIEW.h>

/* Define custom event IDs */
#define MY_EVENT_SENSOR_READ    (32u)
#define MY_EVENT_DATA_PROCESS   (33u)

void sensor_read(void)
{
    SEGGER_SYSVIEW_RecordU32(MY_EVENT_SENSOR_READ, sensor_id);

    /* Read sensor */
    int value = read_sensor();

    SEGGER_SYSVIEW_RecordU32(MY_EVENT_DATA_PROCESS, value);
}
```

### With CTF

```c
#include <zephyr/tracing/tracing.h>

/* Custom trace macros */
#define TRACE_SENSOR_START()  sys_trace_k_thread_switched_in()
#define TRACE_SENSOR_END()    sys_trace_k_thread_switched_out()
```

## Tracing Overhead

Tracing adds minimal overhead but it's not zero:

| Backend | Typical Overhead | Buffering |
|:--|:--|:--|
| RAM | ~1-2 us per event | Circular buffer |
| RTT (SystemView) | ~2-5 us per event | Streaming via J-Link |
| UART (CTF) | ~5-10 us per event | UART speed limited |

### Minimizing Impact

```ini
# Only trace specific event types
CONFIG_TRACING_THREAD=y
CONFIG_TRACING_ISR=y
CONFIG_TRACING_SEMAPHORE=n     # Disable if not needed
CONFIG_TRACING_MUTEX=n         # Disable if not needed
```

## Common Use Cases

### Diagnosing Priority Inversion

Tracing reveals when a high-priority thread is blocked by a lower-priority thread:

```
Time  Event
0.000 Thread A (prio 5) takes mutex
0.001 Thread B (prio 3) becomes ready
0.002 Thread C (prio 1) tries to take mutex — blocked!
0.003 Thread B runs (prio 3) — C is starved!
```

This pattern is visible as a gap in Thread C's execution timeline.

### Measuring ISR Latency

```
Time   Event
0.0000 Hardware interrupt fires
0.0003 ISR enters (300ns latency)
0.0005 ISR exits (200ns duration)
```

### Finding Scheduling Bottlenecks

Trace visualization shows:
- Threads that run too long without yielding
- Unnecessary context switches
- Work queue backlogs
- Timer callback delays

## Best Practices

1. **Use in development** — enable tracing during development, disable for production
2. **Separate transport** — use a dedicated UART or RTT channel for trace data
3. **Size buffers appropriately** — too small loses events, too large wastes RAM
4. **Filter events** — only trace what you need to reduce overhead
5. **Compare before/after** — capture traces before and after optimization
6. **Use SystemView** for interactive debugging, **CTF** for automated analysis

## Example Code

[View the complete tracing example](https://github.com/MichaelTien8901/zephyr-guide-tutorial/tree/main/examples/part6/tracing) — producer-consumer application with CTF tracing enabled.

```bash
west build -b qemu_cortex_m3 examples/part6/tracing
west build -t run
```

## Next Steps

Continue to [Debugging]({% link part6/04-debugging.md %}) for GDB debugging, fault analysis, and runtime diagnostics.
