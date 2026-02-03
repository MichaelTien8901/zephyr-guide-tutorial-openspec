---
layout: default
title: "Condition Variables"
parent: "Part 4: Synchronization & IPC"
nav_order: 3
---

# Condition Variables

Condition variables allow threads to wait for specific conditions while holding a mutex.

## When to Use

- Waiting for a complex condition (not just a count)
- Need to release mutex while waiting
- Multiple threads waiting for same condition

## Basic Usage

```c
#include <zephyr/kernel.h>

K_MUTEX_DEFINE(my_mutex);
K_CONDVAR_DEFINE(my_condvar);

int shared_data;
bool data_ready;

/* Waiting thread */
void waiter_thread(void *p1, void *p2, void *p3)
{
    k_mutex_lock(&my_mutex, K_FOREVER);

    /* Wait for condition with predicate check */
    while (!data_ready) {
        k_condvar_wait(&my_condvar, &my_mutex, K_FOREVER);
    }

    /* Condition is true, mutex is held */
    process_data(shared_data);

    k_mutex_unlock(&my_mutex);
}

/* Signaling thread */
void producer_thread(void *p1, void *p2, void *p3)
{
    k_mutex_lock(&my_mutex, K_FOREVER);

    shared_data = produce_data();
    data_ready = true;

    k_condvar_signal(&my_condvar);  /* Wake one waiter */

    k_mutex_unlock(&my_mutex);
}
```

## The Predicate Pattern

Always use a while loop, not if:

```c
/* CORRECT - while loop */
while (!condition) {
    k_condvar_wait(&cv, &mutex, K_FOREVER);
}

/* WRONG - if statement */
if (!condition) {
    k_condvar_wait(&cv, &mutex, K_FOREVER);
}
/* May have spurious wakeup or another thread took resource */
```

## Signal vs Broadcast

```c
/* Signal: Wake ONE waiting thread */
k_condvar_signal(&my_condvar);

/* Broadcast: Wake ALL waiting threads */
k_condvar_broadcast(&my_condvar);
```

### When to Use Which

| Scenario | Use |
|----------|-----|
| One resource available | Signal |
| State change affects all | Broadcast |
| Multiple resources available | Broadcast |
| Only one thread can proceed | Signal |

## Bounded Buffer Example

```c
#define BUFFER_SIZE 10

K_MUTEX_DEFINE(buffer_mutex);
K_CONDVAR_DEFINE(not_full);
K_CONDVAR_DEFINE(not_empty);

int buffer[BUFFER_SIZE];
int count = 0;
int in = 0, out = 0;

void produce(int item)
{
    k_mutex_lock(&buffer_mutex, K_FOREVER);

    /* Wait while buffer is full */
    while (count == BUFFER_SIZE) {
        k_condvar_wait(&not_full, &buffer_mutex, K_FOREVER);
    }

    buffer[in] = item;
    in = (in + 1) % BUFFER_SIZE;
    count++;

    k_condvar_signal(&not_empty);
    k_mutex_unlock(&buffer_mutex);
}

int consume(void)
{
    k_mutex_lock(&buffer_mutex, K_FOREVER);

    /* Wait while buffer is empty */
    while (count == 0) {
        k_condvar_wait(&not_empty, &buffer_mutex, K_FOREVER);
    }

    int item = buffer[out];
    out = (out + 1) % BUFFER_SIZE;
    count--;

    k_condvar_signal(&not_full);
    k_mutex_unlock(&buffer_mutex);

    return item;
}
```

## API Reference

```c
/* Initialize */
int k_condvar_init(struct k_condvar *condvar);

/* Wait (releases mutex, waits, re-acquires mutex) */
int k_condvar_wait(struct k_condvar *condvar,
                   struct k_mutex *mutex,
                   k_timeout_t timeout);

/* Signal one waiter */
void k_condvar_signal(struct k_condvar *condvar);

/* Signal all waiters */
void k_condvar_broadcast(struct k_condvar *condvar);
```

## Best Practices

1. **Always use while loop** for predicate check
2. **Hold mutex when signaling** (usually)
3. **Use broadcast for state changes** affecting multiple waiters
4. **Check return value** of wait for timeout

## Next Steps

Learn about [Spinlocks]({% link part4/04-spinlocks.md %}) for ISR-safe synchronization.
