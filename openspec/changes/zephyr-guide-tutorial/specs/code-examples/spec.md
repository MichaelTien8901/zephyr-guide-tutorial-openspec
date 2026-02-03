## ADDED Requirements

### Requirement: Example project structure
Each example SHALL be a complete, standalone Zephyr application.

#### Scenario: Minimal project files
- **WHEN** a reader opens an example
- **THEN** they find CMakeLists.txt, prj.conf, and src/main.c at minimum

#### Scenario: Example README
- **WHEN** a reader wants to run an example
- **THEN** they find a README.md with description, build instructions, and expected output

#### Scenario: Board-agnostic where possible
- **WHEN** an example doesn't need specific hardware
- **THEN** it runs on qemu_cortex_m3 or native_posix

### Requirement: Examples organized by topic
Examples SHALL be organized by part and topic matching the documentation structure.

#### Scenario: Directory structure
- **WHEN** a reader looks for examples
- **THEN** they find docs/examples/part{N}/{topic}/ structure

#### Scenario: Example cross-referencing
- **WHEN** documentation mentions an example
- **THEN** it links to the corresponding example directory

### Requirement: Part 1 examples - Getting Started
The guide SHALL include setup verification examples.

#### Scenario: Hello World
- **WHEN** a reader completes setup
- **THEN** examples/part1/hello-world demonstrates basic Zephyr application

#### Scenario: Blinky
- **WHEN** a reader tests GPIO
- **THEN** examples/part1/blinky demonstrates LED blinking

### Requirement: Part 3 examples - Kernel
The guide SHALL include kernel primitive examples.

#### Scenario: Thread creation
- **WHEN** a reader learns threads
- **THEN** examples/part3/threads demonstrates creating and managing threads

#### Scenario: Timer example
- **WHEN** a reader learns timing
- **THEN** examples/part3/timers demonstrates k_timer usage

#### Scenario: Workqueue example
- **WHEN** a reader learns workqueues
- **THEN** examples/part3/workqueue demonstrates k_work patterns

### Requirement: Part 4 examples - Synchronization and IPC
The guide SHALL include synchronization examples.

#### Scenario: Mutex example
- **WHEN** a reader learns mutexes
- **THEN** examples/part4/mutex demonstrates protected shared resource

#### Scenario: Semaphore example
- **WHEN** a reader learns semaphores
- **THEN** examples/part4/semaphore demonstrates producer-consumer

#### Scenario: Message queue example
- **WHEN** a reader learns message passing
- **THEN** examples/part4/msgq demonstrates thread communication

### Requirement: Part 5 examples - Device Drivers
The guide SHALL include driver usage examples.

#### Scenario: GPIO example
- **WHEN** a reader learns GPIO
- **THEN** examples/part5/gpio demonstrates input/output/interrupt

#### Scenario: I2C sensor example
- **WHEN** a reader learns I2C
- **THEN** examples/part5/i2c-sensor demonstrates I2C device communication

#### Scenario: UART example
- **WHEN** a reader learns UART
- **THEN** examples/part5/uart demonstrates serial communication

### Requirement: Part 6 examples - Advanced Topics
The guide SHALL include networking and Bluetooth examples.

#### Scenario: TCP client example
- **WHEN** a reader learns networking
- **THEN** examples/part6/tcp-client demonstrates socket client

#### Scenario: MQTT example
- **WHEN** a reader learns MQTT
- **THEN** examples/part6/mqtt demonstrates pub/sub messaging

#### Scenario: BLE peripheral example
- **WHEN** a reader learns Bluetooth
- **THEN** examples/part6/ble-peripheral demonstrates GATT server

### Requirement: Example testing
Examples SHALL be testable in CI.

#### Scenario: QEMU-based testing
- **WHEN** CI runs
- **THEN** examples that support QEMU build and execute successfully

#### Scenario: Build verification
- **WHEN** CI runs
- **THEN** all examples build without errors for their target boards

### Requirement: Example documentation
Each example SHALL have inline documentation.

#### Scenario: Code comments
- **WHEN** a reader studies example code
- **THEN** key concepts are explained with inline comments

#### Scenario: Expected behavior
- **WHEN** a reader runs an example
- **THEN** the README explains what output to expect
