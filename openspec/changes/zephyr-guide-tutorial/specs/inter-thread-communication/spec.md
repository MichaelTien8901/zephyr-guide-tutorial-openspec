## ADDED Requirements

### Requirement: Message queues documentation
The guide SHALL explain message queues with producer-consumer diagram.

#### Scenario: Message queue basics
- **WHEN** a reader uses message queues
- **THEN** they understand K_MSGQ_DEFINE, k_msgq_put, k_msgq_get

#### Scenario: Message queue flow diagram
- **WHEN** viewing message queue section
- **THEN** a mermaid diagram shows fixed-size message passing between threads

#### Scenario: Queue sizing and overflow
- **WHEN** a reader configures queues
- **THEN** they understand message size, queue depth, and blocking behavior

### Requirement: FIFO documentation
The guide SHALL explain FIFOs for variable-size data passing.

#### Scenario: FIFO basics
- **WHEN** a reader uses FIFOs
- **THEN** they understand K_FIFO_DEFINE, k_fifo_put, k_fifo_get

#### Scenario: FIFO data structure requirements
- **WHEN** a reader passes data via FIFO
- **THEN** they understand the reserved word for internal linking

#### Scenario: FIFO vs message queue decision
- **WHEN** a reader chooses data passing
- **THEN** they understand FIFO for variable data, msgq for fixed-size copies

### Requirement: LIFO documentation
The guide SHALL explain LIFO (stack) data structures.

#### Scenario: LIFO basics
- **WHEN** a reader uses LIFOs
- **THEN** they understand K_LIFO_DEFINE, k_lifo_put, k_lifo_get

#### Scenario: LIFO use cases
- **WHEN** a reader considers LIFO
- **THEN** they understand stack-based processing scenarios

### Requirement: Pipes documentation
The guide SHALL explain byte-stream pipes.

#### Scenario: Pipe basics
- **WHEN** a reader uses pipes
- **THEN** they understand K_PIPE_DEFINE, k_pipe_put, k_pipe_get

#### Scenario: Pipe buffering
- **WHEN** a reader configures pipes
- **THEN** they understand ring buffer behavior and partial transfers

### Requirement: Mailboxes documentation
The guide SHALL explain mailboxes for synchronous message passing.

#### Scenario: Mailbox basics
- **WHEN** a reader uses mailboxes
- **THEN** they understand k_mbox_put, k_mbox_get, and message structure

#### Scenario: Synchronous exchange diagram
- **WHEN** viewing mailbox section
- **THEN** a mermaid sequence diagram shows synchronous send-receive handshake

#### Scenario: Mailbox vs async alternatives
- **WHEN** a reader chooses messaging
- **THEN** they understand mailbox for synchronous, queue/FIFO for async

### Requirement: Events (poll) documentation
The guide SHALL explain the event/poll mechanism.

#### Scenario: Kernel events
- **WHEN** a reader uses events
- **THEN** they understand k_event, k_event_post, k_event_wait

#### Scenario: Polling multiple objects
- **WHEN** a reader waits on multiple sources
- **THEN** they understand k_poll and k_poll_event for multiplexing

#### Scenario: Poll example diagram
- **WHEN** viewing poll section
- **THEN** a mermaid diagram shows thread waiting on semaphore + FIFO + signal

### Requirement: IPC selection flowchart
The guide SHALL provide decision guidance for choosing IPC mechanism.

#### Scenario: IPC decision tree
- **WHEN** a reader needs inter-thread communication
- **THEN** a mermaid flowchart guides: data copy? → msgq; pointer passing? → FIFO; synchronous? → mailbox

### Requirement: Data passing patterns
The guide SHALL document common IPC patterns with examples.

#### Scenario: Buffer pool pattern
- **WHEN** a reader implements zero-copy
- **THEN** they understand memory slab + FIFO pattern for buffer management

#### Scenario: Command-response pattern
- **WHEN** a reader implements request-reply
- **THEN** they understand mailbox or dual-queue patterns
