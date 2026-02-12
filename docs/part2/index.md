---
layout: default
title: "Part 2: Build System"
nav_order: 3
has_children: true
permalink: /part2/
---

# Part 2: Build System Fundamentals

This section covers Zephyr's powerful build system, including west, CMake, Kconfig, and device tree.

## What You'll Learn

- Deep dive into west commands and workspace management
- CMake build system structure and customization
- Kconfig configuration system and troubleshooting
- Device tree fundamentals, overlays, and troubleshooting
- Standard application structure and organization

## Chapters

| Chapter | Description |
|---------|-------------|
| [West Commands]({% link part2/01-west-commands.md %}) | Complete west command reference with workflows |
| [CMake Basics]({% link part2/02-cmake-basics.md %}) | CMakeLists.txt structure and build flow |
| [Kconfig]({% link part2/03-kconfig.md %}) | Configuration system with hierarchy diagrams |
| [Kconfig Troubleshooting]({% link part2/04-kconfig-troubleshooting.md %}) | Common Kconfig errors and how to fix them |
| [Device Tree]({% link part2/05-device-tree.md %}) | DTS fundamentals and syntax |
| [Overlays]({% link part2/06-overlays.md %}) | Device tree overlays and config fragments |
| [Devicetree Troubleshooting]({% link part2/07-devicetree-troubleshooting.md %}) | Decoding DTS errors and debugging strategies |
| [Application Structure]({% link part2/08-application-structure.md %}) | Standard Zephyr app layout |

## Build System Overview

```mermaid
flowchart TB
    subgraph Input["Build Inputs"]
        APP[Application Code]
        CONF[prj.conf]
        DTS[Device Tree]
        KCONFIG[Kconfig Options]
    end

    subgraph BuildSystem["Build System"]
        WEST[West Meta-Tool]
        CMAKE[CMake]
        NINJA[Ninja]
    end

    subgraph Output["Build Outputs"]
        ELF[zephyr.elf]
        HEX[zephyr.hex]
        BIN[zephyr.bin]
    end

    Input --> WEST
    WEST --> CMAKE
    CMAKE --> NINJA
    NINJA --> Output
```

## Prerequisites

Before starting this section, you should have:

- Completed Part 1 (working Zephyr environment)
- Basic understanding of build systems
- Familiarity with configuration files

## Key Concepts

### West
The meta-tool that orchestrates everything - workspace management, building, flashing, and debugging.

### CMake
The build system generator that creates ninja build files from your CMakeLists.txt.

### Kconfig
Linux kernel-style configuration system that lets you enable/disable features and set options.

### Device Tree
Hardware description language that tells Zephyr about your board's peripherals.

{: .note }
Understanding the build system is crucial for effective Zephyr development. Take your time with this section.
