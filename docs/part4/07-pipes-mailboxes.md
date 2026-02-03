---
layout: default
title: "Pipes and Mailboxes"
parent: "Part 4: Synchronization & IPC"
nav_order: 7
---

# Pipes and Mailboxes

Pipes provide byte-stream communication. Mailboxes provide synchronous message passing.

## Pipes

Pipes transfer arbitrary byte streams between threads.

```c
#include <zephyr/kernel.h>

K_PIPE_DEFINE(my_pipe, 256, 4);  /* 256 bytes, 4-byte aligned */

/* Writer */
void writer_thread(void *p1, void *p2, void *p3)
{
    uint8_t data[] = "Hello, Pipe!";
    size_t bytes_written;

    k_pipe_put(&my_pipe, data, sizeof(data),
               &bytes_written, sizeof(data), K_FOREVER);
}

/* Reader */
void reader_thread(void *p1, void *p2, void *p3)
{
    uint8_t buffer[64];
    size_t bytes_read;

    k_pipe_get(&my_pipe, buffer, sizeof(buffer),
               &bytes_read, 1, K_FOREVER);

    printk("Read %zu bytes\n", bytes_read);
}
```

### Pipe API

```c
int k_pipe_put(struct k_pipe *pipe, void *data, size_t size,
               size_t *bytes_written, size_t min_xfer,
               k_timeout_t timeout);

int k_pipe_get(struct k_pipe *pipe, void *data, size_t size,
               size_t *bytes_read, size_t min_xfer,
               k_timeout_t timeout);

void k_pipe_flush(struct k_pipe *pipe);
```

## Mailboxes

Mailboxes provide synchronous, acknowledged message passing.

```c
K_MBOX_DEFINE(my_mbox);

/* Sender */
void sender_thread(void *p1, void *p2, void *p3)
{
    struct k_mbox_msg msg;
    char data[] = "Hello";

    msg.info = 42;
    msg.size = sizeof(data);
    msg.tx_data = data;
    msg.tx_target_thread = K_ANY;

    k_mbox_put(&my_mbox, &msg, K_FOREVER);
    /* Blocks until receiver gets message */
}

/* Receiver */
void receiver_thread(void *p1, void *p2, void *p3)
{
    struct k_mbox_msg msg;
    char buffer[64];

    msg.size = sizeof(buffer);
    msg.rx_source_thread = K_ANY;

    k_mbox_get(&my_mbox, &msg, buffer, K_FOREVER);

    printk("Got %zu bytes, info=%d\n", msg.size, msg.info);
}
```

## When to Use What

| Use Case | Primitive |
|----------|-----------|
| Byte stream | Pipe |
| Synchronous request-response | Mailbox |
| Async fixed-size messages | Message Queue |
| Async variable-size pointers | FIFO |

## Next Steps

Learn about [Events and Polling]({% link part4/08-events-polling.md %}).
