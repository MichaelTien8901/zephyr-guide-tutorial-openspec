# Zephyr Guide Tutorial - Code Examples

This directory contains working code examples that accompany the Zephyr Guide Tutorial.

## Structure

```
examples/
├── part1/              # Getting Started
│   ├── hello-world/    # First Zephyr application
│   └── blinky/         # LED blink with GPIO
├── part3/              # Kernel Essentials
│   ├── threads/        # Thread creation and management
│   ├── timers/         # k_timer usage
│   └── workqueue/      # k_work patterns
├── part4/              # Synchronization & IPC
│   ├── mutex/          # Protected shared resource
│   ├── semaphore/      # Producer-consumer pattern
│   ├── msgq/           # Message queue communication
│   └── zbus/           # Publish-subscribe messaging
├── part5/              # Device Drivers
│   ├── gpio/           # GPIO input/output/interrupt
│   ├── i2c-sensor/     # I2C device communication
│   └── uart/           # Serial communication
├── part6/              # Advanced Topics
│   ├── tcp-client/     # TCP socket client
│   ├── mqtt/           # MQTT pub/sub
│   └── ble-peripheral/ # BLE GATT server
└── Dockerfile          # Build environment
```

## Prerequisites

- Zephyr SDK installed (or use Docker)
- West tool configured
- Compatible development board (or QEMU/native_sim)

## Building Examples

### Using Docker (Recommended)

```bash
# Build the Docker image
docker build -t zephyr-dev .

# Build an example
docker run --rm -v $(pwd):/workdir zephyr-dev \
    west build -b native_sim examples/part1/hello-world

# Run the example (for native_sim)
docker run --rm -v $(pwd):/workdir zephyr-dev \
    ./build/zephyr/zephyr.exe
```

### Using Local Installation

```bash
# Set up Zephyr environment
source ~/zephyrproject/zephyr/zephyr-env.sh

# Build an example
west build -b <board> examples/part1/hello-world

# Flash to board
west flash
```

## Example Board Targets

| Board | Target Name | Notes |
|-------|-------------|-------|
| Native Simulator | `native_sim` | No hardware needed |
| QEMU Cortex-M3 | `qemu_cortex_m3` | Emulated ARM |
| nRF52840 DK | `nrf52840dk_nrf52840` | Nordic BLE board |
| STM32 Nucleo F401RE | `nucleo_f401re` | STM32 board |

## Example Index

### Part 1: Getting Started

| Example | Description | Boards |
|---------|-------------|--------|
| hello-world | Basic printk and main loop | All |
| blinky | LED blink using GPIO | Boards with LEDs |

### Part 3: Kernel Essentials

| Example | Description | Boards |
|---------|-------------|--------|
| threads | Multiple threads with different priorities | All |
| timers | Periodic and one-shot timers | All |
| workqueue | Deferred work from ISR | All |

### Part 4: Synchronization & IPC

| Example | Description | Boards |
|---------|-------------|--------|
| mutex | Protect shared counter | All |
| semaphore | Producer-consumer with bounded buffer | All |
| msgq | Sensor data via message queue | All |
| zbus | Publish-subscribe sensor data | All |

### Part 5: Device Drivers

| Example | Description | Boards |
|---------|-------------|--------|
| gpio | Button input, LED output, interrupt | Boards with buttons/LEDs |
| i2c-sensor | Read temperature sensor | Boards with I2C sensor |
| uart | Echo server over serial | All with UART |

### Part 6: Advanced Topics

| Example | Description | Boards |
|---------|-------------|--------|
| tcp-client | Simple HTTP GET request | Boards with networking |
| mqtt | Publish sensor data to broker | Boards with networking |
| ble-peripheral | Heart rate sensor service | BLE-capable boards |

## Troubleshooting

### Build Errors

1. **Missing board definition**: Ensure `ZEPHYR_BASE` is set
2. **Module not found**: Run `west update` to fetch modules
3. **Toolchain not found**: Check SDK installation path

### Runtime Errors

1. **Stack overflow**: Increase `CONFIG_MAIN_STACK_SIZE`
2. **No output**: Check `CONFIG_CONSOLE` and baud rate
3. **Assertion failure**: Enable `CONFIG_ASSERT` for details

## License

These examples are provided under the Apache 2.0 license.
