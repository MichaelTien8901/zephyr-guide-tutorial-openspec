---
layout: default
title: "Events and Polling"
parent: "Part 4: Synchronization & IPC"
nav_order: 8
---

# Events and Polling

Events provide bitmask-based signaling. Polling allows waiting on multiple kernel objects.

## Kernel Events

```c
#include <zephyr/kernel.h>

#define EVENT_BUTTON_PRESSED  BIT(0)
#define EVENT_DATA_READY      BIT(1)
#define EVENT_TIMER_EXPIRED   BIT(2)

K_EVENT_DEFINE(my_events);

/* Signal from ISR or thread */
void button_isr(const void *arg)
{
    k_event_post(&my_events, EVENT_BUTTON_PRESSED);
}

/* Wait for events */
void event_thread(void *p1, void *p2, void *p3)
{
    while (1) {
        /* Wait for any of these events */
        uint32_t events = k_event_wait(&my_events,
            EVENT_BUTTON_PRESSED | EVENT_DATA_READY,
            false,  /* false=any, true=all */
            K_FOREVER);

        if (events & EVENT_BUTTON_PRESSED) {
            handle_button();
        }
        if (events & EVENT_DATA_READY) {
            process_data();
        }
    }
}
```

### Event API

```c
void k_event_post(struct k_event *event, uint32_t events);
uint32_t k_event_wait(struct k_event *event, uint32_t events,
                      bool wait_all, k_timeout_t timeout);
uint32_t k_event_wait_all(struct k_event *event, uint32_t events,
                          bool reset, k_timeout_t timeout);
void k_event_clear(struct k_event *event, uint32_t events);
```

## Polling Multiple Objects

```c
struct k_poll_event events[3];
K_SEM_DEFINE(my_sem, 0, 1);
K_FIFO_DEFINE(my_fifo);
K_MSGQ_DEFINE(my_msgq, 4, 10, 4);

void init_poll_events(void)
{
    k_poll_event_init(&events[0], K_POLL_TYPE_SEM_AVAILABLE,
                      K_POLL_MODE_NOTIFY_ONLY, &my_sem);
    k_poll_event_init(&events[1], K_POLL_TYPE_FIFO_DATA_AVAILABLE,
                      K_POLL_MODE_NOTIFY_ONLY, &my_fifo);
    k_poll_event_init(&events[2], K_POLL_TYPE_MSGQ_DATA_AVAILABLE,
                      K_POLL_MODE_NOTIFY_ONLY, &my_msgq);
}

void poll_thread(void *p1, void *p2, void *p3)
{
    init_poll_events();

    while (1) {
        int rc = k_poll(events, 3, K_FOREVER);

        for (int i = 0; i < 3; i++) {
            if (events[i].state == K_POLL_STATE_SEM_AVAILABLE) {
                k_sem_take(&my_sem, K_NO_WAIT);
                handle_semaphore();
            }
            if (events[i].state == K_POLL_STATE_FIFO_DATA_AVAILABLE) {
                void *data = k_fifo_get(&my_fifo, K_NO_WAIT);
                handle_fifo(data);
            }
            if (events[i].state == K_POLL_STATE_MSGQ_DATA_AVAILABLE) {
                uint32_t msg;
                k_msgq_get(&my_msgq, &msg, K_NO_WAIT);
                handle_msgq(msg);
            }

            /* Reset for next poll */
            events[i].state = K_POLL_STATE_NOT_READY;
        }
    }
}
```

## Poll Signal

```c
struct k_poll_signal my_signal;
struct k_poll_event events[1];

void init(void)
{
    k_poll_signal_init(&my_signal);
    k_poll_event_init(&events[0], K_POLL_TYPE_SIGNAL,
                      K_POLL_MODE_NOTIFY_ONLY, &my_signal);
}

/* Signal from anywhere */
void signal_event(int result)
{
    k_poll_signal_raise(&my_signal, result);
}

/* Wait for signal */
void wait_for_signal(void)
{
    k_poll(events, 1, K_FOREVER);

    int signaled, result;
    k_poll_signal_check(&my_signal, &signaled, &result);
    k_poll_signal_reset(&my_signal);
}
```

## Next Steps

See the [IPC Selection Guide]({% link part4/09-ipc-selection.md %}) for choosing the right primitive.
