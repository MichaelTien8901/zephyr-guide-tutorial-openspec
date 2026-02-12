## Why

There is no comprehensive, beginner-to-professional guide for the Zephyr RTOS framework that combines structured learning with practical examples and modern documentation practices. Developers need a single resource that covers environment setup (including Docker), core concepts, and advanced topics with clear flowcharts and diagrams to understand complex RTOS patterns.

## What Changes

- Create a complete Jekyll-based documentation site hostable on GitHub Pages
- Provide tools setup and configuration guides including Docker-based development
- Cover Zephyr fundamentals: device tree, Kconfig, west tool, build system
- Document core services: kernel primitives, threads, synchronization, memory management
- Include advanced topics: networking, Bluetooth, power management, custom drivers
- Cover real-world Zephyr projects: OpenBIC (BMC firmware) and ASPEED RoT (Root of Trust) as professional-level case studies
- Add mermaid diagrams and flowcharts for all major concepts
- Provide working code examples for each topic
- Structure content from beginner to professional level

## Capabilities

### New Capabilities

- `tutorial-structure`: Overall site structure, Jekyll configuration, mermaid setup, navigation, and GitHub Pages hosting configuration
- `environment-setup`: Development environment installation including Zephyr SDK, west tool, toolchains, and Docker-based development workflow
- `zephyr-introduction`: Overview of Zephyr RTOS, architecture, supported boards, and ecosystem
- `build-system`: West meta-tool, CMake integration, Kconfig configuration, and device tree basics
- `kernel-fundamentals`: Threads, scheduling, ISRs, timing services, and kernel objects
- `synchronization`: Mutexes, semaphores, condition variables, and thread coordination patterns
- `memory-management`: Memory pools, heaps, slabs, and memory protection (MPU/MMU)
- `inter-thread-communication`: Message queues, FIFOs, LIFOs, pipes, mailboxes, and events
- `device-drivers`: Driver model, device tree bindings, GPIO, I2C, SPI, UART, and custom driver development
- `networking`: Network stack, sockets API, TCP/UDP, MQTT, CoAP, and HTTP client/server
- `bluetooth`: BLE stack, GATT services, advertising, scanning, and mesh networking
- `power-management`: Power states, device power management, tickless idle, and low-power optimization
- `debugging-testing`: Logging, shell, Twister test framework, and debugging techniques
- `openbic-case-study`: OpenBIC project analysis - open-source BMC firmware architecture, IPMI/PLDM protocols, sensor management, and multi-platform support
- `aspeed-rot-case-study`: ASPEED Root of Trust project analysis - security architecture, secure boot, attestation, firmware protection, and PFR implementation
- `code-examples`: Runnable example projects for all major topics with README documentation

### Modified Capabilities

(None - this is a new project)

## Impact

- **New files**: Complete `docs/` directory structure with Jekyll site
- **Dependencies**: Jekyll with just-the-docs theme, Mermaid JS for diagrams
- **Infrastructure**: GitHub Actions workflow for Jekyll deployment
- **Docker**: Development containers for both documentation and Zephyr builds
- **Examples**: Zephyr application examples organized by topic
