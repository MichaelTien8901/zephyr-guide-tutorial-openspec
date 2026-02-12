---
layout: default
title: "Zephyr for Embedded Products"
parent: "Part 1: Getting Started"
nav_order: 2
---

# Zephyr for Embedded Products

Before diving into installation and setup, it's worth understanding why Zephyr is an excellent choice for embedded products and what the ecosystem looks like.

## Why Use Zephyr?

Zephyr has rapidly become one of the most popular choices for embedded product development. Here's why:

### Open Source with Permissive Licensing

Zephyr is released under the **Apache 2.0 license**, which means:

- **No royalties** — no per-device fees
- **Commercial-friendly** — use in proprietary products without releasing your source code
- **Patent grant** — contributors grant patent rights
- **No copyleft obligations** — unlike GPL-based RTOSes

{: .note }
Some third-party components bundled with Zephyr (e.g., certain drivers or libraries) may have different licenses. Always check the `LICENSE` file in each module.

### Vendor-Neutral Governance

Zephyr is a **Linux Foundation project**, not owned by any single chip vendor. This means:

- No vendor lock-in
- Multiple silicon vendors contribute and maintain support
- Roadmap driven by community needs, not a single company's priorities

### Modern Architecture

Unlike legacy RTOSes that evolved from simpler kernels, Zephyr was designed from the ground up for modern connected devices:

- **Configurable kernel** — enable only what you need
- **Native networking** — TCP/IP, Bluetooth, 802.15.4, LoRa, and more
- **Security-first** — memory protection, secure boot support, crypto APIs
- **Multi-architecture** — ARM, RISC-V, x86, ARC, Xtensa, and more

## Hardware Support

Zephyr supports **600+ boards** across multiple architectures:

| Architecture | Example SoCs | Typical Use Cases |
|:--|:--|:--|
| ARM Cortex-M | STM32, nRF52/53, NXP i.MX RT | IoT sensors, wearables, motor control |
| ARM Cortex-A | i.MX 8M | Gateway devices, multimedia |
| RISC-V | ESP32-C3, GD32VF103, SiFive | Low-cost IoT, education |
| x86 | Intel Quark, Elkhart Lake | Industrial, edge computing |
| Xtensa | ESP32, Intel ADSP | Audio, Wi-Fi devices |
| ARC | Synopsys ARC EM/HS | Specialized embedded |

### Vendor Ecosystem

Major silicon vendors actively contribute to Zephyr:

- **Nordic Semiconductor** — nRF Connect SDK built on Zephyr
- **NXP** — MCUXpresso SDK with Zephyr support
- **STMicroelectronics** — STM32 full family support
- **Intel** — x86 and RISC-V platforms
- **Espressif** — ESP32 series support
- **Microchip** — SAM family support
- **Texas Instruments** — CC13xx/CC26xx support

## Ecosystem Overview

Zephyr provides a complete platform, not just a kernel:

```
┌─────────────────────────────────────────────┐
│              Application Layer              │
├─────────────────────────────────────────────┤
│  Networking  │  Bluetooth  │  USB  │ Storage│
│  (TCP/IP,    │  (BLE, Mesh,│       │ (FS,   │
│   MQTT,CoAP) │   Audio)    │       │  NVS)  │
├─────────────────────────────────────────────┤
│  Logging  │  Shell  │  Settings  │  DFU     │
├─────────────────────────────────────────────┤
│         Device Driver Framework             │
├─────────────────────────────────────────────┤
│              Kernel (Scheduler,             │
│         Threads, Sync, Memory Mgmt)         │
├─────────────────────────────────────────────┤
│     Hardware Abstraction Layer (HAL)        │
├─────────────────────────────────────────────┤
│              Hardware (SoC / Board)         │
└─────────────────────────────────────────────┘
```

### Key Subsystems

- **Kernel** — threads, scheduling, synchronization, timers
- **Networking** — full TCP/IP stack, MQTT, CoAP, HTTP, WebSocket, LwM2M
- **Bluetooth** — BLE, Bluetooth Mesh, Bluetooth Audio (LE Audio)
- **USB** — device and host support
- **File Systems** — FAT, LittleFS
- **Non-Volatile Storage** — settings subsystem, NVS
- **Device Firmware Update** — MCUboot integration
- **Security** — Mbed TLS, PSA Crypto, Trusted Firmware-M

## Product Considerations

When using Zephyr for a commercial product, consider:

### Release Lifecycle

Zephyr follows a **time-based release cycle**:

- **Major releases** — every ~4 months (e.g., 3.6, 3.7, 4.0)
- **LTS (Long-Term Support)** — designated releases with 2+ years of security patches
- **Security advisories** — published for critical vulnerabilities

{: .tip }
For production products, prefer **LTS releases** and subscribe to the [Zephyr security mailing list](https://lists.zephyrproject.org/g/security) for advisories.

### Certification

Zephyr can be used in certified products:

- **Bluetooth SIG** — Zephyr's BLE stack is qualified
- **Thread Group** — OpenThread integration is certified
- **IEC 62443** — security certification paths available
- **Safety** — functional safety working group active (IEC 61508)

### Security

Zephyr provides multiple security mechanisms:

- **Memory Protection Unit (MPU)** — hardware isolation between threads
- **User/Supervisor mode** — privilege separation
- **Stack canaries** — stack overflow detection
- **Secure boot** — MCUboot integration
- **Crypto** — hardware-accelerated cryptography where available
- **PSA Certified** — Platform Security Architecture compliance

### Software Updates

Over-the-air (OTA) and field updates are supported through:

- **MCUboot** — secure bootloader with rollback support
- **SMP (Simple Management Protocol)** — device management over BLE, UART, or UDP
- **LwM2M** — standardized IoT device management
- **hawkBit** — Eclipse IoT update server integration

## Product Examples

Zephyr is used in production by companies large and small:

| Product/Project | Company | Use Case |
|:--|:--|:--|
| nRF Connect SDK | Nordic Semiconductor | Complete BLE/Thread/Matter SDK |
| OpenBIC | Meta (Facebook) | BMC firmware for data centers |
| ChromeOS EC | Google | Embedded controller firmware |
| Golioth | Golioth | IoT cloud platform SDK |
| Pinnacle 100 | Laird Connectivity | Industrial IoT gateway |
| Air Quality Monitor | Various | Environmental sensing |

{: .note }
Part 7 of this guide provides in-depth case studies of OpenBIC and ASPEED Root of Trust — two real-world production uses of Zephyr.

## Zephyr vs. Other RTOSes

| Feature | Zephyr | FreeRTOS | Mbed OS | RIOT |
|:--|:--|:--|:--|:--|
| License | Apache 2.0 | MIT | Apache 2.0 | LGPL |
| Governance | Linux Foundation | AWS | ARM (archived) | Community |
| Networking | Native full stack | Add-on (FreeRTOS+TCP) | Native | Native |
| Bluetooth | Full stack | No | BLE only | Partial |
| Board support | 600+ | Vendor-specific | Limited | 200+ |
| Build system | CMake/west | Vendor IDE | Mbed CLI | Make |
| Device tree | Yes | No | No | No |

## Getting Started

Now that you understand why Zephyr is a great choice for embedded products, let's set up your development environment.

Continue to [Installation]({% link part1/03-installation.md %}) to install the Zephyr SDK.
