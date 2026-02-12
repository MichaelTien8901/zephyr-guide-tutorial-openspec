## ADDED Requirements

### Requirement: Thread creation and management
The guide SHALL explain Zephyr threads with lifecycle diagram.

#### Scenario: Thread creation methods
- **WHEN** a reader creates threads
- **THEN** they understand K_THREAD_DEFINE, k_thread_create, and stack allocation

#### Scenario: Thread lifecycle diagram
- **WHEN** viewing the threads section
- **THEN** a mermaid state diagram shows created → ready → running → suspended → terminated

#### Scenario: Thread attributes
- **WHEN** a reader configures threads
- **THEN** they understand priority, stack size, options, and delay start

### Requirement: Scheduling and priorities
The guide SHALL explain the Zephyr scheduler with priority visualization.

#### Scenario: Priority levels
- **WHEN** a reader learns scheduling
- **THEN** they understand cooperative vs preemptive priorities and CONFIG_NUM_PREEMPT_PRIORITIES

#### Scenario: Scheduling algorithm
- **WHEN** a reader studies the scheduler
- **THEN** they understand round-robin within priority and preemption rules

#### Scenario: Priority inversion
- **WHEN** a reader learns about pitfalls
- **THEN** they understand priority inversion and priority inheritance

### Requirement: Interrupt handling (ISRs)
The guide SHALL explain ISR implementation with execution flow diagram.

#### Scenario: ISR definition
- **WHEN** a reader implements an ISR
- **THEN** they understand IRQ_CONNECT, IRQ_DIRECT_CONNECT, and handler signatures

#### Scenario: ISR restrictions
- **WHEN** a reader writes ISR code
- **THEN** they understand what APIs are safe to call from ISR context

#### Scenario: ISR to thread communication diagram
- **WHEN** viewing ISR section
- **THEN** a mermaid sequence diagram shows ISR → semaphore → thread wakeup pattern

### Requirement: Timing services
The guide SHALL cover Zephyr's timing APIs.

#### Scenario: Kernel ticks and timeouts
- **WHEN** a reader uses timing
- **THEN** they understand k_timeout_t, K_MSEC, K_FOREVER, and tick configuration

#### Scenario: Delays and sleep
- **WHEN** a reader needs delays
- **THEN** they use k_sleep, k_msleep, k_busy_wait appropriately

#### Scenario: Timers
- **WHEN** a reader needs periodic callbacks
- **THEN** they understand k_timer_init, k_timer_start, and expiry functions

### Requirement: System threads and workqueues
The guide SHALL explain built-in threads and workqueue pattern.

#### Scenario: System workqueue
- **WHEN** a reader defers work from ISR
- **THEN** they understand k_work, k_work_submit, and system workqueue

#### Scenario: Custom workqueues
- **WHEN** a reader needs isolated work processing
- **THEN** they can create dedicated workqueues with custom priority

### Requirement: Kernel objects overview
The guide SHALL provide a reference diagram of all kernel objects.

#### Scenario: Kernel object taxonomy
- **WHEN** viewing the kernel overview
- **THEN** a mermaid diagram categorizes sync primitives, data passing, and memory objects

#### Scenario: Object selection guide
- **WHEN** a reader chooses a kernel object
- **THEN** a decision flowchart helps select the right primitive for their use case
