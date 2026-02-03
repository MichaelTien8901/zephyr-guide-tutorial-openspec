## 1. Project Infrastructure Setup

- [x] 1.1 Create docs/ directory structure with part1-part7 and appendices folders
- [x] 1.2 Create _config.yml with just-the-docs theme, mermaid config, and site settings
- [x] 1.3 Create Gemfile with Jekyll and just-the-docs dependencies
- [x] 1.4 Create _includes/head_custom.html with mermaid CSS styling (paper-like background)
- [x] 1.5 Create _includes/mermaid_config.js with mermaid initialization and theme variables
- [x] 1.6 Create docs/index.md home page with guide overview and navigation
- [x] 1.7 Create docs/Dockerfile for Jekyll documentation development
- [x] 1.8 Create docs/docker-compose.yml for local documentation preview
- [x] 1.9 Create .github/workflows/jekyll.yml for GitHub Pages deployment
- [x] 1.10 Create .devcontainer/devcontainer.json for VS Code DevContainer support

## 2. Part 1 - Getting Started

- [x] 2.1 Create part1/index.md with section overview and chapter listing
- [x] 2.2 Create part1/01-introduction.md - What is Zephyr, history, ecosystem overview
- [x] 2.3 Create part1/02-installation.md - Zephyr SDK installation for Linux/macOS/Windows
- [x] 2.4 Create part1/03-west-setup.md - West tool installation, workspace initialization
- [x] 2.5 Create part1/04-toolchains.md - ARM, RISC-V, x86 toolchain configuration
- [x] 2.6 Create part1/05-docker-setup.md - Docker-based Zephyr development environment
- [x] 2.7 Create part1/06-ide-setup.md - VS Code extensions and configuration
- [x] 2.8 Create part1/07-hello-world.md - First Zephyr application with verification steps

## 3. Part 2 - Build System Fundamentals

- [x] 3.1 Create part2/index.md with build system section overview
- [x] 3.2 Create part2/01-west-commands.md - West command reference with flowchart
- [x] 3.3 Create part2/02-cmake-basics.md - CMakeLists.txt structure and build flow diagram
- [x] 3.4 Create part2/03-kconfig.md - Kconfig system with configuration hierarchy diagram
- [x] 3.5 Create part2/04-device-tree.md - Device tree fundamentals with DTS syntax examples
- [x] 3.6 Create part2/05-overlays.md - Device tree overlays and configuration fragments
- [x] 3.7 Create part2/06-application-structure.md - Standard Zephyr app layout and organization

## 4. Part 3 - Kernel Essentials

- [x] 4.1 Create part3/index.md with kernel section overview
- [x] 4.2 Create part3/01-threads.md - Thread creation, lifecycle diagram, attributes
- [x] 4.3 Create part3/02-scheduling.md - Scheduler, priorities, preemption with diagrams
- [x] 4.4 Create part3/03-interrupts.md - ISR implementation, restrictions, ISR-to-thread diagram
- [x] 4.5 Create part3/04-timing.md - Kernel ticks, delays, sleep, timers
- [x] 4.6 Create part3/05-workqueues.md - System workqueue, custom workqueues, k_work
- [x] 4.7 Create part3/06-kernel-objects.md - Object taxonomy diagram and selection flowchart

## 5. Part 4 - Synchronization and IPC

- [x] 5.1 Create part4/index.md with synchronization section overview
- [x] 5.2 Create part4/01-mutexes.md - Mutex usage, priority inheritance, vs spinlock
- [x] 5.3 Create part4/02-semaphores.md - Counting/binary semaphores with producer-consumer diagram
- [x] 5.4 Create part4/03-condition-variables.md - Condvar usage and predicate pattern
- [x] 5.5 Create part4/04-spinlocks.md - ISR-safe locking, atomic operations
- [x] 5.6 Create part4/05-message-queues.md - Message queue usage with flow diagram
- [x] 5.7 Create part4/06-fifos-lifos.md - FIFO/LIFO for variable-size data passing
- [x] 5.8 Create part4/07-pipes-mailboxes.md - Byte streams and synchronous messaging
- [x] 5.9 Create part4/08-events-polling.md - Events, k_poll, multiplexing diagram
- [x] 5.10 Create part4/09-ipc-selection.md - IPC decision flowchart and patterns
- [x] 5.11 Create part4/10-zbus.md - Zbus publish-subscribe messaging (added by request)

## 6. Part 5 - Device Drivers

- [x] 6.1 Create part5/index.md with device driver section overview
- [x] 6.2 Create part5/01-driver-model.md - Driver architecture diagram, device API pattern
- [x] 6.3 Create part5/02-device-tree-bindings.md - Compatible strings, bindings YAML, macros
- [x] 6.4 Create part5/03-gpio.md - GPIO configuration, read/write, interrupts with LED example
- [x] 6.5 Create part5/04-i2c.md - I2C configuration, transfers, sensor example
- [x] 6.6 Create part5/05-spi.md - SPI configuration and transfer patterns
- [x] 6.7 Create part5/06-uart.md - UART polling, interrupt, async API modes
- [x] 6.8 Create part5/07-custom-drivers.md - Writing custom drivers with complete example

## 7. Part 6 - Advanced Topics

- [x] 7.1 Create part6/index.md with advanced topics section overview
- [x] 7.2 Create part6/01-networking-overview.md - Network stack architecture diagram
- [x] 7.3 Create part6/02-sockets.md - BSD sockets API, TCP/UDP client/server examples
- [x] 7.4 Create part6/03-mqtt.md - MQTT client connection, pub/sub with example
- [x] 7.5 Create part6/04-coap.md - CoAP basics, server resources, observe pattern
- [x] 7.6 Create part6/05-http.md - HTTP client/server basics
- [x] 7.7 Create part6/06-tls.md - TLS/DTLS configuration, secure socket example
- [x] 7.8 Create part6/07-ble-overview.md - Bluetooth stack architecture diagram
- [x] 7.9 Create part6/08-ble-advertising.md - Advertising, scan response, modes
- [x] 7.10 Create part6/09-ble-scanning.md - Scanning, filtering, central role
- [x] 7.11 Create part6/10-ble-gatt-server.md - GATT service definition with custom example
- [x] 7.12 Create part6/11-ble-gatt-client.md - Service discovery, read/write, notifications
- [x] 7.13 Create part6/12-ble-mesh.md - Bluetooth Mesh introduction and basic example
- [x] 7.14 Create part6/13-power-management.md - PM architecture, states, state diagram
- [x] 7.15 Create part6/14-device-pm.md - Per-device PM, tickless kernel, policy
- [x] 7.16 Create part6/15-debugging.md - Logging, shell, GDB, fault analysis
- [x] 7.17 Create part6/16-testing.md - Twister, Ztest, test organization

## 8. Part 7 - Professional Case Studies

- [x] 8.1 Create part7/index.md with case studies section overview
- [x] 8.2 Create part7/01-openbic-overview.md - OpenBIC introduction, architecture diagram
- [x] 8.3 Create part7/02-openbic-architecture.md - Module organization, platform abstraction
- [x] 8.4 Create part7/03-openbic-protocols.md - IPMI/PLDM implementation analysis
- [x] 8.5 Create part7/04-openbic-sensors.md - Sensor framework, monitoring diagram
- [x] 8.6 Create part7/05-openbic-lessons.md - Design patterns and lessons learned
- [x] 8.7 Create part7/06-aspeed-rot-overview.md - ASPEED RoT introduction, security architecture
- [x] 8.8 Create part7/07-aspeed-rot-secure-boot.md - Secure boot flow diagram, verification
- [x] 8.9 Create part7/08-aspeed-rot-attestation.md - Attestation, DICE/SPDM, sequence diagram
- [x] 8.10 Create part7/09-aspeed-rot-pfr.md - Platform Firmware Resilience, recovery patterns
- [x] 8.11 Create part7/10-aspeed-rot-lessons.md - Security patterns and lessons learned

## 9. Appendices

- [x] 9.1 Create appendices/index.md with appendices section overview
- [x] 9.2 Create appendices/glossary.md - Zephyr terminology definitions
- [x] 9.3 Create appendices/references.md - External resources, official docs links
- [x] 9.4 Create appendices/troubleshooting.md - Common issues and solutions

## 10. Code Examples

- [x] 10.1 Create examples/README.md with examples overview and structure
- [x] 10.2 Create examples/part1/hello-world/ - CMakeLists.txt, prj.conf, src/main.c, README.md
- [x] 10.3 Create examples/part1/blinky/ - LED blink example with GPIO
- [x] 10.4 Create examples/part3/threads/ - Thread creation and management example
- [x] 10.5 Create examples/part3/timers/ - k_timer usage example
- [x] 10.6 Create examples/part3/workqueue/ - k_work patterns example
- [x] 10.7 Create examples/part4/mutex/ - Protected shared resource example
- [x] 10.8 Create examples/part4/semaphore/ - Producer-consumer example
- [x] 10.9 Create examples/part4/msgq/ - Message queue communication example
- [x] 10.10 Create examples/part4/zbus/ - Zbus publish-subscribe example
- [x] 10.11 Create examples/part5/gpio/ - GPIO input/output/interrupt example
- [x] 10.12 Create examples/part5/i2c-sensor/ - I2C device communication example
- [x] 10.13 Create examples/part5/uart/ - Serial communication example
- [x] 10.14 Create examples/part6/tcp-client/ - TCP socket client example
- [x] 10.15 Create examples/part6/mqtt/ - MQTT pub/sub example
- [x] 10.16 Create examples/part6/ble-peripheral/ - BLE GATT server example
- [x] 10.17 Create examples/Dockerfile for Zephyr SDK build environment
- [x] 10.18 Create examples/west.yml manifest for version pinning

## 11. Final Integration

- [ ] 11.1 Verify all internal {% link %} tags resolve correctly
- [ ] 11.2 Verify all mermaid diagrams render with paper-like theme
- [ ] 11.3 Test Jekyll build locally with docker-compose
- [ ] 11.4 Verify navigation structure and search functionality
- [ ] 11.5 Test example builds with Zephyr Docker environment
- [ ] 11.6 Create git remote setup for origin and deploy repositories
