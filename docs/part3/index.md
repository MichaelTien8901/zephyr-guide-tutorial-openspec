---
layout: default
title: "Part 3: Kernel Essentials"
nav_order: 4
has_children: true
permalink: /part3/
---

# Part 3: Kernel Essentials

This section covers Zephyr's kernel fundamentals - threads, scheduling, interrupts, timing, and kernel objects.

## What You'll Learn

- Creating and managing threads
- Understanding the scheduler and priorities
- Implementing interrupt handlers (ISRs)
- Using timing services (delays, timers)
- Working with workqueues
- Choosing the right kernel objects

## Chapters

| Chapter | Description |
|---------|-------------|
| [Threads]({% link part3/01-threads.md %}) | Thread creation, lifecycle, and attributes |
| [Scheduling]({% link part3/02-scheduling.md %}) | Scheduler, priorities, and preemption |
| [Interrupts]({% link part3/03-interrupts.md %}) | ISR implementation and restrictions |
| [Timing]({% link part3/04-timing.md %}) | Kernel ticks, delays, sleep, timers |
| [Workqueues]({% link part3/05-workqueues.md %}) | System workqueue and k_work |
| [Kernel Objects]({% link part3/06-kernel-objects.md %}) | Object taxonomy and selection |

## Kernel Architecture Overview

```mermaid
flowchart TB
    subgraph Threads["Thread Management"]
        T1[Thread 1]
        T2[Thread 2]
        T3[Thread 3]
    end

    subgraph Scheduler["Scheduler"]
        READY[Ready Queue]
        RUNNING[Running Thread]
        PEND[Pending Threads]
    end

    subgraph Services["Kernel Services"]
        TIMING[Timing]
        SYNC[Synchronization]
        IPC[IPC]
    end

    Threads --> Scheduler
    Scheduler --> Services
```

## Prerequisites

Before starting this section, you should have:

- Completed Parts 1-2 (working build environment)
- Understanding of C programming
- Basic knowledge of concurrent programming concepts

{: .note }
The kernel is the heart of Zephyr. Understanding these concepts is essential for writing reliable embedded applications.
