---
layout: default
title: Home
nav_order: 1
description: "Zephyr RTOS Programming Guide - Learn Zephyr from beginner to professional"
permalink: /
---

# Zephyr RTOS Programming Guide

Welcome to the comprehensive Zephyr RTOS programming guide. This tutorial takes you from complete beginner to professional-level embedded systems development, covering everything from basic setup to real-world case studies like OpenBIC and ASPEED Root of Trust.

## What You'll Learn

- **Getting Started**: Set up your development environment with Docker, west, and IDE integration
- **Build System**: Master west, CMake, Kconfig, and device tree fundamentals
- **Kernel Essentials**: Threads, scheduling, interrupts, timing, and kernel objects
- **Synchronization & IPC**: Mutexes, semaphores, message queues, and inter-thread communication
- **Device Drivers**: GPIO, I2C, SPI, UART, and writing custom drivers
- **Advanced Topics**: Networking, Bluetooth LE, power management, debugging, and testing
- **Professional Case Studies**: OpenBIC BMC firmware and ASPEED RoT security architecture

## Quick Start

1. [Introduction to Zephyr]({% link part1/01-introduction.md %})
2. [Install Zephyr SDK]({% link part1/02-installation.md %})
3. [Set up West]({% link part1/03-west-setup.md %})
4. [Write Hello World]({% link part1/07-hello-world.md %})

## How This Guide is Organized

| Section | Description | Difficulty |
|---------|-------------|------------|
| [Part 1: Getting Started]({% link part1/index.md %}) | Environment setup, tooling, first program | Beginner |
| [Part 2: Build System]({% link part2/index.md %}) | West, CMake, Kconfig, device tree | Beginner |
| [Part 3: Kernel Essentials]({% link part3/index.md %}) | Threads, scheduling, timing, workqueues | Intermediate |
| [Part 4: Synchronization & IPC]({% link part4/index.md %}) | Mutexes, semaphores, queues, events | Intermediate |
| [Part 5: Device Drivers]({% link part5/index.md %}) | Driver model, GPIO, I2C, SPI, UART | Intermediate |
| [Part 6: Advanced Topics]({% link part6/index.md %}) | Networking, Bluetooth, power, debugging | Advanced |
| [Part 7: Case Studies]({% link part7/index.md %}) | OpenBIC, ASPEED RoT analysis | Professional |
| [Appendices]({% link appendices/index.md %}) | Glossary, references, troubleshooting | Reference |

## Prerequisites

- Basic C programming knowledge
- Command line familiarity
- Understanding of embedded systems concepts (helpful but not required)

## Sample Code

All examples in this guide are tested and available in the [examples/](https://github.com/MichaelTien8901/zephyr-guide-tutorial-openspec/tree/main/examples) directory. Each example includes:

- Complete, buildable Zephyr application
- Detailed comments explaining the code
- README with build and run instructions

## Docker Quick Start

Don't want to install Zephyr locally? Use our Docker environment:

```bash
docker-compose up -d
docker-compose exec zephyr west build -b qemu_cortex_m3 samples/hello_world
docker-compose exec zephyr west build -t run
```

See [Docker Setup]({% link part1/05-docker-setup.md %}) for details.

## Contributing

Found an error? Want to add content? [Open an issue](https://github.com/MichaelTien8901/zephyr-guide-tutorial-openspec/issues) on GitHub.

---

{: .note }
This guide is community-maintained and not officially affiliated with the Zephyr Project or Linux Foundation. For official documentation, visit [docs.zephyrproject.org](https://docs.zephyrproject.org/).
