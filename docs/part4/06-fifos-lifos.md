---
layout: default
title: "FIFOs and LIFOs"
parent: "Part 4: Synchronization & IPC"
nav_order: 6
---

# FIFOs and LIFOs

FIFOs (First-In-First-Out) and LIFOs (Last-In-First-Out) pass pointers between threads, allowing variable-size data.

## FIFO (Queue)

```c
#include <zephyr/kernel.h>

struct data_item {
    void *fifo_reserved;  /* Required: used by kernel */
    uint8_t data[64];
    size_t len;
};

K_FIFO_DEFINE(my_fifo);

/* Producer */
void producer(void)
{
    struct data_item *item = k_malloc(sizeof(*item));
    item->len = fill_data(item->data);
    k_fifo_put(&my_fifo, item);  /* ISR-safe */
}

/* Consumer */
void consumer_thread(void *p1, void *p2, void *p3)
{
    while (1) {
        struct data_item *item = k_fifo_get(&my_fifo, K_FOREVER);
        process_data(item->data, item->len);
        k_free(item);
    }
}
```

## LIFO (Stack)

```c
K_LIFO_DEFINE(my_lifo);

/* Same structure requirement */
struct work_item {
    void *lifo_reserved;
    int priority;
    void (*func)(void);
};

/* Push */
k_lifo_put(&my_lifo, item);

/* Pop */
struct work_item *item = k_lifo_get(&my_lifo, K_FOREVER);
```

## FIFO vs LIFO vs Message Queue

| Feature | FIFO | LIFO | Message Queue |
|---------|------|------|---------------|
| Order | First-in-first-out | Last-in-first-out | First-in-first-out |
| Data | Pointers | Pointers | Copies |
| Size | Variable | Variable | Fixed |
| Allocation | User manages | User manages | Built-in |

## Buffer Pool Pattern

```c
#define NUM_BUFFERS 8
#define BUFFER_SIZE 128

struct buffer {
    void *fifo_reserved;
    uint8_t data[BUFFER_SIZE];
};

static struct buffer buffer_pool[NUM_BUFFERS];
K_FIFO_DEFINE(free_buffers);

void init_buffer_pool(void)
{
    for (int i = 0; i < NUM_BUFFERS; i++) {
        k_fifo_put(&free_buffers, &buffer_pool[i]);
    }
}

struct buffer *alloc_buffer(k_timeout_t timeout)
{
    return k_fifo_get(&free_buffers, timeout);
}

void free_buffer(struct buffer *buf)
{
    k_fifo_put(&free_buffers, buf);
}
```

## API Reference

```c
/* FIFO */
void k_fifo_put(struct k_fifo *fifo, void *data);
void *k_fifo_get(struct k_fifo *fifo, k_timeout_t timeout);
void *k_fifo_peek_head(struct k_fifo *fifo);
int k_fifo_is_empty(struct k_fifo *fifo);

/* LIFO */
void k_lifo_put(struct k_lifo *lifo, void *data);
void *k_lifo_get(struct k_lifo *lifo, k_timeout_t timeout);
```

## Best Practices

1. **Always include reserved field** - First field in structure
2. **Manage memory carefully** - Producer allocates, consumer frees
3. **Use buffer pools** - Avoid dynamic allocation
4. **Check for NULL** - Get can return NULL on timeout

## Next Steps

Learn about [Pipes and Mailboxes]({% link part4/07-pipes-mailboxes.md %}).
