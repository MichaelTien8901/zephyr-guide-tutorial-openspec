---
layout: default
title: "Part 5: Device Drivers"
nav_order: 6
has_children: true
permalink: /part5/
---

# Part 5: Device Drivers

This section covers Zephyr's device driver model and common peripheral interfaces.

## What You'll Learn

- Zephyr's device driver architecture
- Device tree bindings for drivers
- GPIO, I2C, SPI, and UART APIs
- Writing custom device drivers

## Chapters

| Chapter | Description |
|---------|-------------|
| [Driver Model]({% link part5/01-driver-model.md %}) | Device architecture and APIs |
| [Device Tree Bindings]({% link part5/02-devicetree-bindings.md %}) | Connecting DTS to drivers |
| [GPIO]({% link part5/03-gpio.md %}) | Digital I/O and interrupts |
| [I2C]({% link part5/04-i2c.md %}) | I2C bus communication |
| [SPI]({% link part5/05-spi.md %}) | SPI bus communication |
| [UART]({% link part5/06-uart.md %}) | Serial communication |
| [Custom Drivers]({% link part5/07-custom-drivers.md %}) | Writing your own drivers |

## Driver Architecture

```mermaid
flowchart TB
    APP[Application] --> API[Device API]
    API --> DRIVER[Driver Implementation]
    DRIVER --> DT[Device Tree Config]
    DRIVER --> HW[Hardware Registers]

    subgraph Subsystems
        GPIO[GPIO Subsystem]
        I2C[I2C Subsystem]
        SPI[SPI Subsystem]
    end

    API --> Subsystems
```

{: .note }
Understanding the driver model is essential for working with hardware in Zephyr.
