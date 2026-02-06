---
layout: default
title: "Timing"
parent: "Part 3: Kernel Essentials"
nav_order: 4
---

# Timing Services

Zephyr provides various timing mechanisms for delays, timeouts, and periodic operations.

## Time Units

### Kernel Ticks

The fundamental time unit in Zephyr:

```c
/* Get current tick count */
int64_t ticks = k_uptime_ticks();

/* Convert to milliseconds */
int64_t ms = k_ticks_to_ms_floor64(ticks);

/* Ticks per second */
uint32_t ticks_per_sec = CONFIG_SYS_CLOCK_TICKS_PER_SEC;
```

### Timeout Values

```c
/* Various timeout specifications */
k_timeout_t timeout;

timeout = K_NO_WAIT;      /* Return immediately */
timeout = K_FOREVER;      /* Wait forever */
timeout = K_MSEC(100);    /* 100 milliseconds */
timeout = K_USEC(500);    /* 500 microseconds */
timeout = K_SECONDS(5);   /* 5 seconds */
timeout = K_MINUTES(1);   /* 1 minute */
timeout = K_HOURS(1);     /* 1 hour */

/* Absolute timeout */
int64_t target = k_uptime_get() + 1000;
timeout = K_TIMEOUT_ABS_MS(target);
```

## Sleeping

### Basic Sleep

```c
#include <zephyr/kernel.h>

void my_thread(void *p1, void *p2, void *p3)
{
    /* Sleep for duration */
    k_sleep(K_MSEC(500));

    /* Convenience functions */
    k_msleep(500);   /* Same as K_MSEC(500) */
    k_usleep(1000);  /* Microseconds */
}
```

### Busy Wait (No Scheduling)

```c
/* Busy wait - blocks CPU, no thread switch */
k_busy_wait(100);  /* 100 microseconds */
```

{: .warning }
`k_busy_wait()` wastes CPU cycles. Use only for very short delays where thread switching overhead is unacceptable.

### Precise Periodic Timing

```c
void periodic_thread(void *p1, void *p2, void *p3)
{
    int64_t next_wake = k_uptime_get();

    while (1) {
        do_periodic_work();

        /* Schedule next wake time */
        next_wake += 100;  /* 100ms period */
        k_sleep(K_TIMEOUT_ABS_MS(next_wake));
    }
}
```

## Timers

Timers execute callbacks or signal threads at specified intervals.

### Basic Timer

```c
#include <zephyr/kernel.h>

void timer_callback(struct k_timer *timer)
{
    /* Called in ISR context! */
    /* Keep it short, use ISR-safe APIs only */
    k_sem_give(&timer_sem);
}

K_TIMER_DEFINE(my_timer, timer_callback, NULL);

void start_timer(void)
{
    /* Start: 1000ms initial, 500ms periodic */
    k_timer_start(&my_timer, K_MSEC(1000), K_MSEC(500));
}

void stop_timer(void)
{
    k_timer_stop(&my_timer);
}
```

### Timer with Expiry Function

```c
void timer_expiry(struct k_timer *timer)
{
    /* Called when timer expires */
    printk("Timer expired!\n");
}

void timer_stop_func(struct k_timer *timer)
{
    /* Called when timer is stopped */
    printk("Timer stopped\n");
}

K_TIMER_DEFINE(my_timer, timer_expiry, timer_stop_func);
```

### One-Shot Timer

```c
/* Fire once after 1 second */
k_timer_start(&my_timer, K_SECONDS(1), K_NO_WAIT);
```

### Timer Status

```c
/* Get remaining time */
uint32_t remaining = k_timer_remaining_get(&my_timer);

/* Get status (times fired since last check) */
uint32_t status = k_timer_status_get(&my_timer);

/* Sync (wait for timer) */
uint32_t synced = k_timer_status_sync(&my_timer);
```

### Using Timer User Data

```c
struct my_data {
    int id;
    int count;
};

void timer_callback(struct k_timer *timer)
{
    struct my_data *data = k_timer_user_data_get(timer);
    data->count++;
}

struct my_data timer_data = {.id = 1, .count = 0};

void setup(void)
{
    k_timer_user_data_set(&my_timer, &timer_data);
    k_timer_start(&my_timer, K_MSEC(100), K_MSEC(100));
}
```

## Uptime Functions

```c
/* Milliseconds since boot */
int64_t ms = k_uptime_get();

/* 32-bit version (wraps after ~49 days) */
uint32_t ms32 = k_uptime_get_32();

/* Delta time */
int64_t start = k_uptime_get();
do_work();
int64_t elapsed = k_uptime_delta(&start);
printk("Work took %lld ms\n", elapsed);
```

## Cycle Counting

For high-resolution timing:

```c
/* Hardware cycles */
uint32_t start = k_cycle_get_32();
do_work();
uint32_t cycles = k_cycle_get_32() - start;

/* Convert to nanoseconds */
uint64_t ns = k_cyc_to_ns_floor64(cycles);
```

## Timeout Handling in APIs

Most Zephyr APIs accept timeouts:

```c
/* Semaphore with timeout */
int ret = k_sem_take(&my_sem, K_MSEC(500));
if (ret == -EAGAIN) {
    /* Timeout occurred */
}

/* Message queue with timeout */
ret = k_msgq_get(&my_queue, &data, K_SECONDS(1));
if (ret == -EAGAIN) {
    /* Timeout occurred */
}
```

## Common Patterns

### Watchdog-Style Timer

```c
K_TIMER_DEFINE(watchdog, watchdog_expired, NULL);

void watchdog_expired(struct k_timer *timer)
{
    printk("Watchdog timeout! System may be stuck.\n");
    /* Take recovery action */
}

void feed_watchdog(void)
{
    /* Restart timer - prevents expiry */
    k_timer_start(&watchdog, K_SECONDS(5), K_NO_WAIT);
}

void main_loop(void)
{
    k_timer_start(&watchdog, K_SECONDS(5), K_NO_WAIT);

    while (1) {
        do_work();
        feed_watchdog();  /* Reset watchdog */
    }
}
```

### Debounce Timer

```c
K_TIMER_DEFINE(debounce_timer, debounce_expired, NULL);
static volatile bool button_pressed;

void button_isr(const void *arg)
{
    /* Start debounce timer */
    k_timer_start(&debounce_timer, K_MSEC(50), K_NO_WAIT);
}

void debounce_expired(struct k_timer *timer)
{
    /* Read stable button state */
    button_pressed = gpio_pin_get_dt(&button);
}
```

### Rate Limiting

```c
static int64_t last_action_time;
#define MIN_INTERVAL_MS 100

bool can_perform_action(void)
{
    int64_t now = k_uptime_get();
    if (now - last_action_time >= MIN_INTERVAL_MS) {
        last_action_time = now;
        return true;
    }
    return false;
}
```

## Configuration

```ini
# prj.conf

# System clock frequency
CONFIG_SYS_CLOCK_TICKS_PER_SEC=1000

# High resolution timers
CONFIG_HRTIMER=y

# Tickless kernel (power saving)
CONFIG_TICKLESS_KERNEL=y
```

## Best Practices

1. **Use appropriate precision** - Don't use microseconds when milliseconds suffice
2. **Avoid busy waiting** - Use `k_sleep()` or timers instead
3. **Timer callbacks are ISR context** - Keep them short
4. **Use absolute timeouts for periodic tasks** - Prevents drift
5. **Check timeout return values** - Handle -EAGAIN appropriately

## Example Code

See the complete [Timers Example]({% link examples/part3/timers/src/main.c %}) demonstrating timer creation, periodic callbacks, and timing patterns.

## Next Steps

Learn about [Workqueues]({% link part3/05-workqueues.md %}) for deferred work processing.
