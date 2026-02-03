---
layout: default
title: "Glossary"
parent: "Appendices"
nav_order: 1
---

# Glossary

Common terms and concepts in Zephyr development.

## A

**API (Application Programming Interface)**
: Set of functions and macros that applications use to interact with Zephyr.

**Atomic**
: Operations that complete without interruption, used for lock-free programming.

## B

**Board**
: Hardware platform definition including SoC and board-specific configurations.

**BSP (Board Support Package)**
: Collection of files that define a board's hardware configuration.

## C

**CMake**
: Build system generator used by Zephyr to create build files.

**Cooperative Thread**
: Thread with negative priority that only yields voluntarily.

**CONFIG_***
: Kconfig configuration symbols, prefixed with CONFIG_.

## D

**Device Tree (DTS)**
: Data structure describing hardware configuration.

**DEVICE_DT_DEFINE**
: Macro to define a device instance from device tree.

**DT_** (macros)**
: Device tree accessor macros like DT_NODELABEL, DT_PROP.

## F

**FIFO**
: First-in-first-out queue for passing data between threads.

## G

**GPIO (General Purpose Input/Output)**
: Digital pins that can be configured as inputs or outputs.

## H

**HAL (Hardware Abstraction Layer)**
: Layer that abstracts hardware-specific details.

## I

**I2C (Inter-Integrated Circuit)**
: Two-wire serial bus for connecting peripherals.

**ISR (Interrupt Service Routine)**
: Function that handles hardware interrupts.

## K

**Kconfig**
: Configuration system inherited from Linux kernel.

**Kernel Objects**
: Synchronization primitives like mutexes, semaphores, queues.

**k_*** (functions)**
: Kernel API functions prefixed with k_.

## L

**LIFO**
: Last-in-first-out stack for data passing.

## M

**MCU (Microcontroller Unit)**
: Integrated circuit with CPU, memory, and peripherals.

**Mutex**
: Mutual exclusion object for protecting shared resources.

**MSGQ (Message Queue)**
: Fixed-size message passing mechanism.

## P

**Preemptive Thread**
: Thread that can be interrupted by higher priority threads.

**Priority**
: Thread scheduling priority (lower number = higher priority).

**prj.conf**
: Project configuration file for Kconfig options.

## R

**RTOS (Real-Time Operating System)**
: Operating system with deterministic timing guarantees.

## S

**Scheduler**
: Kernel component that decides which thread runs.

**Semaphore**
: Counting synchronization primitive.

**SoC (System on Chip)**
: Integrated circuit containing CPU and peripherals.

**SPI (Serial Peripheral Interface)**
: Four-wire serial bus for high-speed communication.

**Spinlock**
: Busy-wait synchronization primitive, ISR-safe.

## T

**Thread**
: Basic unit of execution in Zephyr.

**Tick**
: Basic time unit in the kernel (CONFIG_SYS_CLOCK_TICKS_PER_SEC).

**Twister**
: Zephyr's test framework.

## U

**UART (Universal Asynchronous Receiver-Transmitter)**
: Serial communication interface.

## W

**West**
: Zephyr's meta-tool for workspace management, building, and flashing.

**Workqueue**
: Mechanism for deferred work execution in thread context.

## Z

**Zephyr SDK**
: Software Development Kit containing toolchains and tools.

**Ztest**
: Unit testing framework for Zephyr applications.
