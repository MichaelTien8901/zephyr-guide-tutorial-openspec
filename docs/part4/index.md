---
layout: default
title: "Part 4: Synchronization & IPC"
nav_order: 5
has_children: true
permalink: /part4/
---

# Part 4: Synchronization and Inter-Thread Communication

This section covers kernel objects for thread synchronization and data passing in detail.

## What You'll Learn

- Mutexes for mutual exclusion
- Semaphores for signaling and counting
- Condition variables for complex synchronization
- Spinlocks and atomic operations
- Message queues, FIFOs, LIFOs
- Pipes and mailboxes
- Events and polling

## Chapters

| Chapter | Description |
|---------|-------------|
| [Mutexes]({% link part4/01-mutexes.md %}) | Mutual exclusion and priority inheritance |
| [Semaphores]({% link part4/02-semaphores.md %}) | Counting and binary semaphores |
| [Condition Variables]({% link part4/03-condition-variables.md %}) | Wait with predicate pattern |
| [Spinlocks]({% link part4/04-spinlocks.md %}) | ISR-safe locking and atomics |
| [Message Queues]({% link part4/05-message-queues.md %}) | Fixed-size message passing |
| [FIFOs and LIFOs]({% link part4/06-fifos-lifos.md %}) | Variable-size data passing |
| [Pipes and Mailboxes]({% link part4/07-pipes-mailboxes.md %}) | Byte streams and sync messaging |
| [Events and Polling]({% link part4/08-events-polling.md %}) | Multi-event waiting |
| [IPC Selection Guide]({% link part4/09-ipc-selection.md %}) | Decision flowcharts |

## When to Use What

```mermaid
flowchart TD
    A[Need to communicate?] --> B{Protect shared data?}
    B -->|Yes| C{ISR context?}
    B -->|No| D{Pass data?}

    C -->|Yes| E[Spinlock]
    C -->|No| F[Mutex]

    D -->|Yes| G{Fixed size?}
    D -->|No| H{Signal event?}

    G -->|Yes| I[Message Queue]
    G -->|No| J[FIFO/LIFO]

    H -->|Yes| K[Semaphore/Events]
    H -->|No| L[Condition Variable]
```

{: .note }
Choosing the right synchronization primitive is crucial for correctness and performance.
