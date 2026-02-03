---
layout: default
title: "Spinlocks"
parent: "Part 4: Synchronization & IPC"
nav_order: 4
---

# Spinlocks and Atomics

Spinlocks provide ISR-safe synchronization through busy-waiting. Atomics provide lock-free operations.

## Spinlocks

### When to Use

- ISR and thread share data
- Very short critical sections
- Cannot afford to sleep

### Basic Usage

```c
#include <zephyr/kernel.h>
#include <zephyr/spinlock.h>

K_SPINLOCK_DEFINE(my_lock);

volatile int shared_counter;

void thread_function(void)
{
    k_spinlock_key_t key = k_spin_lock(&my_lock);

    /* Critical section - keep it SHORT */
    shared_counter++;

    k_spin_unlock(&my_lock, key);
}

void my_isr(const void *arg)
{
    k_spinlock_key_t key = k_spin_lock(&my_lock);

    /* Same lock works in ISR */
    shared_counter++;

    k_spin_unlock(&my_lock, key);
}
```

### Key Points

- `k_spin_lock()` disables interrupts and returns state
- `k_spin_unlock()` restores previous interrupt state
- On SMP: actually spins waiting for lock
- On UP: just disables interrupts

## Atomic Operations

For simple operations, atomics avoid locks entirely.

### Atomic Types

```c
#include <zephyr/sys/atomic.h>

atomic_t counter = ATOMIC_INIT(0);
atomic_ptr_t ptr = ATOMIC_PTR_INIT(NULL);
```

### Atomic Operations

```c
/* Set and get */
atomic_set(&counter, 10);
atomic_val_t val = atomic_get(&counter);

/* Arithmetic */
atomic_inc(&counter);           /* counter++ */
atomic_dec(&counter);           /* counter-- */
atomic_add(&counter, 5);        /* counter += 5 */
atomic_sub(&counter, 3);        /* counter -= 3 */

/* Bitwise */
atomic_or(&counter, 0x0F);      /* counter |= 0x0F */
atomic_and(&counter, 0xF0);     /* counter &= 0xF0 */
atomic_xor(&counter, 0xFF);     /* counter ^= 0xFF */

/* Compare and swap */
atomic_val_t old = 5;
if (atomic_cas(&counter, old, 10)) {
    /* counter was 5, now is 10 */
}
```

### Atomic Bit Operations

```c
ATOMIC_DEFINE(flags, 32);  /* 32 bits */

/* Set/clear/test bits */
atomic_set_bit(flags, 3);       /* Set bit 3 */
atomic_clear_bit(flags, 3);     /* Clear bit 3 */
bool set = atomic_test_bit(flags, 3);  /* Test bit 3 */

/* Test and set/clear */
bool was_set = atomic_test_and_set_bit(flags, 3);
bool was_set = atomic_test_and_clear_bit(flags, 3);
```

## Spinlock vs Mutex vs Atomic

| Feature | Spinlock | Mutex | Atomic |
|---------|----------|-------|--------|
| ISR-safe | Yes | No | Yes |
| Blocking type | Busy-wait | Sleep | None |
| Critical section | Very short | Can be longer | Single operation |
| Priority inheritance | No | Yes | N/A |

## Common Patterns

### ISR-Thread Shared Counter

```c
K_SPINLOCK_DEFINE(counter_lock);
volatile uint32_t event_count;

void event_isr(const void *arg)
{
    k_spinlock_key_t key = k_spin_lock(&counter_lock);
    event_count++;
    k_spin_unlock(&counter_lock, key);
}

uint32_t get_event_count(void)
{
    k_spinlock_key_t key = k_spin_lock(&counter_lock);
    uint32_t count = event_count;
    k_spin_unlock(&counter_lock, key);
    return count;
}
```

### Lock-Free Flag with Atomics

```c
atomic_t data_ready = ATOMIC_INIT(0);

void producer(void)
{
    prepare_data();
    atomic_set(&data_ready, 1);
}

void consumer(void)
{
    while (atomic_get(&data_ready) == 0) {
        k_yield();
    }
    process_data();
    atomic_set(&data_ready, 0);
}
```

## Best Practices

1. **Keep spinlock sections minimal** - Microseconds, not milliseconds
2. **Use atomics for simple operations** - Counters, flags
3. **Don't sleep while holding spinlock** - System will hang
4. **Prefer mutex for complex operations** - If not ISR context

## Next Steps

Learn about [Message Queues]({% link part4/05-message-queues.md %}) for data passing.
