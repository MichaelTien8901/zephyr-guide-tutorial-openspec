## ADDED Requirements

### Requirement: Mutex documentation
The guide SHALL explain mutexes with usage patterns and pitfall warnings.

#### Scenario: Mutex basics
- **WHEN** a reader uses mutexes
- **THEN** they understand k_mutex_init, k_mutex_lock, k_mutex_unlock

#### Scenario: Priority inheritance
- **WHEN** a reader learns mutex behavior
- **THEN** they understand Zephyr mutexes implement priority inheritance automatically

#### Scenario: Mutex vs spinlock decision
- **WHEN** a reader chooses synchronization
- **THEN** they understand when to use mutex (thread context) vs spinlock (ISR-safe)

### Requirement: Semaphore documentation
The guide SHALL explain counting and binary semaphores with diagrams.

#### Scenario: Semaphore operations
- **WHEN** a reader uses semaphores
- **THEN** they understand k_sem_init, k_sem_give, k_sem_take with timeout

#### Scenario: Producer-consumer pattern
- **WHEN** viewing semaphore patterns
- **THEN** a mermaid sequence diagram shows producer-consumer synchronization

#### Scenario: Binary semaphore as signal
- **WHEN** a reader needs thread signaling
- **THEN** they understand using semaphore with max count 1

### Requirement: Condition variables
The guide SHALL explain condition variables for complex synchronization.

#### Scenario: Condition variable basics
- **WHEN** a reader uses condvars
- **THEN** they understand k_condvar_init, k_condvar_wait, k_condvar_signal

#### Scenario: Wait with predicate pattern
- **WHEN** a reader implements condition waits
- **THEN** they understand the while-loop predicate check pattern

### Requirement: Spinlocks for ISR-safe synchronization
The guide SHALL explain spinlocks and their appropriate use cases.

#### Scenario: Spinlock usage
- **WHEN** a reader needs ISR-safe locking
- **THEN** they understand k_spinlock and k_spin_lock/k_spin_unlock

#### Scenario: Spinlock cautions
- **WHEN** a reader uses spinlocks
- **THEN** they understand to minimize critical section duration

### Requirement: Atomic operations
The guide SHALL document atomic APIs for lock-free programming.

#### Scenario: Atomic types and operations
- **WHEN** a reader uses atomics
- **THEN** they understand atomic_t, atomic_set, atomic_get, atomic_inc

#### Scenario: Compare and swap
- **WHEN** a reader implements lock-free algorithms
- **THEN** they understand atomic_cas and its usage patterns

### Requirement: Synchronization decision flowchart
The guide SHALL provide a decision tree for choosing synchronization primitives.

#### Scenario: Primitive selection
- **WHEN** a reader needs synchronization
- **THEN** a mermaid flowchart guides: ISR context? → spinlock/semaphore; Thread only? → mutex/condvar

### Requirement: Common pitfalls and deadlock prevention
The guide SHALL document synchronization pitfalls with examples.

#### Scenario: Deadlock scenarios
- **WHEN** a reader learns about deadlocks
- **THEN** they understand lock ordering and deadlock prevention strategies

#### Scenario: Priority inversion examples
- **WHEN** a reader studies priority issues
- **THEN** they understand unbounded priority inversion and mitigation
