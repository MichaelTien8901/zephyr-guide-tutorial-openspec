---
layout: default
title: "Part 6: Advanced Topics"
nav_order: 7
has_children: true
permalink: /part6/
---

# Part 6: Advanced Topics

This section covers debugging, testing, quality assurance, networking, Bluetooth, and power management.

## What You'll Learn

- Logging, shell, and tracing subsystems
- Debugging and testing techniques
- Quality assurance and working without hardware
- Network stack and socket programming
- MQTT, CoAP, and HTTP protocols
- Bluetooth LE and Mesh
- Power management strategies

## Chapters

### Debugging, Testing & Quality
| Chapter | Description |
|---------|-------------|
| [Logging]({% link part6/01-logging.md %}) | Logging subsystem, log levels, APIs, backends |
| [Shell]({% link part6/02-shell.md %}) | Interactive shell, built-in and custom commands |
| [Tracing]({% link part6/03-tracing.md %}) | CTF, SystemView, performance analysis |
| [Debugging]({% link part6/04-debugging.md %}) | GDB, fault handling, memory debugging |
| [Testing]({% link part6/05-testing.md %}) | Twister and Ztest |
| [Quality Assurance]({% link part6/06-quality-assurance.md %}) | Footprint, static analysis, code coverage, SBOM |
| [Working without Hardware]({% link part6/07-working-without-hardware.md %}) | QEMU, native_sim, device emulation |

### Networking
| Chapter | Description |
|---------|-------------|
| [Networking Overview]({% link part6/08-networking-overview.md %}) | Network stack architecture |
| [Sockets]({% link part6/09-sockets.md %}) | TCP/UDP client/server |
| [MQTT]({% link part6/10-mqtt.md %}) | IoT messaging protocol |
| [CoAP]({% link part6/11-coap.md %}) | Constrained networks |
| [HTTP]({% link part6/12-http.md %}) | Web client/server |
| [TLS]({% link part6/13-tls.md %}) | Secure communication |

### Bluetooth
| Chapter | Description |
|---------|-------------|
| [BLE Overview]({% link part6/14-ble-overview.md %}) | Bluetooth stack |
| [Advertising]({% link part6/15-ble-advertising.md %}) | BLE advertising |
| [Scanning]({% link part6/16-ble-scanning.md %}) | Central role |
| [GATT Server]({% link part6/17-ble-gatt-server.md %}) | Custom services |
| [GATT Client]({% link part6/18-ble-gatt-client.md %}) | Service discovery |
| [Mesh]({% link part6/19-ble-mesh.md %}) | Bluetooth Mesh |

### Power Management
| Chapter | Description |
|---------|-------------|
| [Power Management]({% link part6/20-power-management.md %}) | PM architecture |
| [Device PM]({% link part6/21-device-pm.md %}) | Per-device PM |

{: .note }
These advanced topics build on the fundamentals from Parts 1-5.
