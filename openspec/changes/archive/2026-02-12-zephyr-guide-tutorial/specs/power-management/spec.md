## ADDED Requirements

### Requirement: Power management overview
The guide SHALL explain Zephyr's power management architecture with state diagram.

#### Scenario: PM architecture diagram
- **WHEN** viewing power management section
- **THEN** a mermaid state diagram shows active → idle → suspend → off states

#### Scenario: PM configuration
- **WHEN** a reader enables power management
- **THEN** they understand CONFIG_PM, CONFIG_PM_DEVICE, CONFIG_PM_POLICY options

### Requirement: System power states
The guide SHALL explain system-level power states.

#### Scenario: Power state definitions
- **WHEN** a reader learns power states
- **THEN** they understand PM_STATE_ACTIVE, PM_STATE_RUNTIME_IDLE, PM_STATE_SUSPEND_TO_IDLE, PM_STATE_SUSPEND_TO_RAM

#### Scenario: State transition triggers
- **WHEN** a reader understands transitions
- **THEN** they understand how kernel idle triggers state transitions

#### Scenario: Residency requirements
- **WHEN** a reader configures states
- **THEN** they understand minimum residency and exit latency constraints

### Requirement: Device power management
The guide SHALL explain per-device power management.

#### Scenario: Device PM API
- **WHEN** a reader implements device PM
- **THEN** they understand pm_device_action_run, PM_DEVICE_DT_DEFINE

#### Scenario: Device PM states
- **WHEN** a reader manages device power
- **THEN** they understand PM_DEVICE_STATE_ACTIVE, SUSPENDED, OFF

#### Scenario: Device PM in drivers
- **WHEN** a reader writes PM-aware drivers
- **THEN** they understand implementing pm_action callbacks

### Requirement: Tickless idle
The guide SHALL explain tickless kernel operation.

#### Scenario: Tickless configuration
- **WHEN** a reader enables tickless
- **THEN** they understand CONFIG_TICKLESS_KERNEL and its power benefits

#### Scenario: Timer programming
- **WHEN** a reader understands tickless
- **THEN** they understand how kernel programs next wakeup dynamically

### Requirement: Power policy
The guide SHALL explain power policy customization.

#### Scenario: Built-in policies
- **WHEN** a reader uses default policy
- **THEN** they understand residency-based automatic state selection

#### Scenario: Custom policy
- **WHEN** a reader customizes policy
- **THEN** they understand implementing pm_policy_next_state

#### Scenario: Policy constraints
- **WHEN** a reader needs to prevent deep sleep
- **THEN** they understand pm_policy_state_lock and application constraints

### Requirement: Low-power optimization techniques
The guide SHALL document optimization patterns.

#### Scenario: Peripheral shutdown
- **WHEN** a reader optimizes power
- **THEN** they understand disabling unused peripherals and clocks

#### Scenario: Reducing wakeup frequency
- **WHEN** a reader reduces power
- **THEN** they understand batching work, longer sleep intervals

#### Scenario: RAM retention
- **WHEN** a reader uses suspend modes
- **THEN** they understand RAM retention vs full shutdown trade-offs

### Requirement: Power measurement guidance
The guide SHALL explain how to measure power consumption.

#### Scenario: Measurement setup
- **WHEN** a reader measures power
- **THEN** they understand current measurement techniques and tools

#### Scenario: Power profiling
- **WHEN** a reader profiles application
- **THEN** they understand identifying power-hungry code paths

### Requirement: Power management examples
The guide SHALL provide practical PM examples.

#### Scenario: Simple PM example
- **WHEN** viewing PM section
- **THEN** an example shows basic system PM configuration

#### Scenario: Device PM example
- **WHEN** viewing device PM section
- **THEN** an example shows device suspend/resume with sensor
