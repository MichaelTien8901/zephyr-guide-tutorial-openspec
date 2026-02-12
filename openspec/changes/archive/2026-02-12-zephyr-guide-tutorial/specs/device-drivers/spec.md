## ADDED Requirements

### Requirement: Driver model overview
The guide SHALL explain Zephyr's device driver architecture with diagram.

#### Scenario: Driver model diagram
- **WHEN** viewing driver section
- **THEN** a mermaid diagram shows device tree → driver binding → device API hierarchy

#### Scenario: Driver initialization
- **WHEN** a reader understands driver init
- **THEN** they understand init levels, priorities, and DEVICE_DT_DEFINE

#### Scenario: Device API pattern
- **WHEN** a reader uses device APIs
- **THEN** they understand device_get_binding, device_is_ready, and subsystem APIs

### Requirement: Device tree bindings for drivers
The guide SHALL explain how to connect device tree to drivers.

#### Scenario: Compatible string matching
- **WHEN** a reader writes a driver
- **THEN** they understand DT_DRV_COMPAT and compatible property matching

#### Scenario: Binding YAML files
- **WHEN** a reader defines hardware properties
- **THEN** they understand dts/bindings/*.yaml structure and property definitions

#### Scenario: Instance iteration
- **WHEN** a driver supports multiple instances
- **THEN** they understand DT_INST_FOREACH_STATUS_OKAY and per-instance configuration

### Requirement: GPIO driver usage
The guide SHALL explain GPIO with practical examples.

#### Scenario: GPIO configuration
- **WHEN** a reader uses GPIO
- **THEN** they understand gpio_pin_configure, GPIO_OUTPUT, GPIO_INPUT flags

#### Scenario: GPIO read/write
- **WHEN** a reader controls GPIO
- **THEN** they understand gpio_pin_get, gpio_pin_set, gpio_pin_toggle

#### Scenario: GPIO interrupts
- **WHEN** a reader uses GPIO interrupts
- **THEN** they understand gpio_pin_interrupt_configure and callback registration

#### Scenario: LED blink example
- **WHEN** viewing GPIO section
- **THEN** a complete LED blink example demonstrates GPIO output

### Requirement: I2C driver usage
The guide SHALL explain I2C communication patterns.

#### Scenario: I2C configuration
- **WHEN** a reader uses I2C
- **THEN** they understand device tree I2C bus and device configuration

#### Scenario: I2C transfers
- **WHEN** a reader communicates via I2C
- **THEN** they understand i2c_write, i2c_read, i2c_write_read, i2c_burst_read

#### Scenario: I2C sensor example
- **WHEN** viewing I2C section
- **THEN** a complete example reads data from an I2C sensor

### Requirement: SPI driver usage
The guide SHALL explain SPI communication patterns.

#### Scenario: SPI configuration
- **WHEN** a reader uses SPI
- **THEN** they understand spi_config, frequency, mode, and chip select

#### Scenario: SPI transfers
- **WHEN** a reader communicates via SPI
- **THEN** they understand spi_transceive, tx/rx buffers, and spi_buf_set

### Requirement: UART driver usage
The guide SHALL explain UART for serial communication.

#### Scenario: UART polling mode
- **WHEN** a reader uses simple UART
- **THEN** they understand uart_poll_out, uart_poll_in for basic I/O

#### Scenario: UART interrupt mode
- **WHEN** a reader uses async UART
- **THEN** they understand uart_irq_callback_set and interrupt-driven communication

#### Scenario: UART async API
- **WHEN** a reader needs DMA-based UART
- **THEN** they understand uart_rx_enable, uart_tx and async callbacks

### Requirement: Custom driver development
The guide SHALL explain how to write a custom device driver.

#### Scenario: Driver structure
- **WHEN** a reader creates a driver
- **THEN** they understand driver data, config structures, and API tables

#### Scenario: Driver registration
- **WHEN** a reader registers a driver
- **THEN** they understand DEVICE_DT_INST_DEFINE and init function requirements

#### Scenario: Custom driver example
- **WHEN** viewing custom driver section
- **THEN** a complete example implements a simple custom peripheral driver

### Requirement: Driver testing
The guide SHALL explain driver testing approaches.

#### Scenario: Ztest for drivers
- **WHEN** a reader tests drivers
- **THEN** they understand mock devices and integration testing with Ztest
