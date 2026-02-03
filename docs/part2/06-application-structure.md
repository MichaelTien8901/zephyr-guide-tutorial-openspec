---
layout: default
title: "Application Structure"
parent: "Part 2: Build System"
nav_order: 6
---

# Zephyr Application Structure

This guide covers best practices for organizing Zephyr applications, from simple projects to complex multi-module systems.

## Minimal Application

The simplest Zephyr application:

```
my-app/
├── CMakeLists.txt
├── prj.conf
└── src/
    └── main.c
```

### CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(my_app)

target_sources(app PRIVATE src/main.c)
```

### prj.conf

```ini
CONFIG_PRINTK=y
```

### src/main.c

```c
#include <zephyr/kernel.h>

int main(void)
{
    printk("Hello from my app!\n");
    return 0;
}
```

## Standard Application

A typical application with more organization:

```
my-app/
├── CMakeLists.txt
├── prj.conf
├── Kconfig                    # App-specific options
├── README.md
├── src/
│   ├── main.c
│   ├── app_config.h
│   └── modules/
│       ├── sensors.c
│       ├── sensors.h
│       ├── comms.c
│       └── comms.h
├── include/
│   └── app/
│       └── api.h
├── boards/
│   ├── nrf52840dk_nrf52840.overlay
│   ├── nrf52840dk_nrf52840.conf
│   └── stm32f4_disco.overlay
└── dts/
    └── bindings/
        └── my-device.yaml
```

### CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(my_app)

# Include directories
target_include_directories(app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Source files
target_sources(app PRIVATE
    src/main.c
    src/modules/sensors.c
    src/modules/comms.c
)
```

## Large Application Structure

For complex projects:

```
my-app/
├── CMakeLists.txt
├── prj.conf
├── Kconfig
├── VERSION                    # Semantic version
├── west.yml                   # For multi-repo (optional)
│
├── src/
│   ├── main.c
│   ├── CMakeLists.txt
│   │
│   ├── drivers/               # Custom drivers
│   │   ├── CMakeLists.txt
│   │   ├── my_sensor.c
│   │   └── my_sensor.h
│   │
│   ├── services/              # Application services
│   │   ├── CMakeLists.txt
│   │   ├── data_manager.c
│   │   └── data_manager.h
│   │
│   └── lib/                   # Internal libraries
│       ├── CMakeLists.txt
│       ├── protocol.c
│       └── protocol.h
│
├── include/
│   └── app/
│       ├── api.h
│       ├── config.h
│       └── types.h
│
├── boards/
│   ├── nrf52840dk_nrf52840.overlay
│   ├── nrf52840dk_nrf52840.conf
│   └── custom_board/          # Custom board definition
│       ├── custom_board.dts
│       ├── custom_board_defconfig
│       └── Kconfig.board
│
├── dts/
│   └── bindings/
│       └── vendor,my-sensor.yaml
│
├── tests/
│   ├── CMakeLists.txt
│   ├── prj.conf
│   ├── testcase.yaml
│   └── src/
│       └── main.c
│
├── samples/                   # Usage examples
│   └── basic/
│       ├── CMakeLists.txt
│       ├── prj.conf
│       └── src/main.c
│
└── doc/                       # Documentation
    └── api.md
```

## Workspace Topologies

### In-Tree Application (T1)

Application inside Zephyr workspace:

```
zephyrproject/
├── zephyr/
├── modules/
└── apps/              # Your applications
    └── my-app/
```

**Pros**: Simple setup, automatic ZEPHYR_BASE
**Cons**: Mixed with Zephyr updates

### Out-of-Tree Application (T2)

Application in separate directory:

```
~/projects/
├── zephyrproject/     # Zephyr workspace
└── my-app/            # Your application
```

**Build**:
```bash
export ZEPHYR_BASE=~/projects/zephyrproject/zephyr
west build -b board ~/projects/my-app
```

**Pros**: Clean separation, independent versioning
**Cons**: Need to manage ZEPHYR_BASE

### Application with Manifest (T3)

Application as west workspace root:

```
my-app/
├── west.yml           # Manifest pointing to Zephyr
├── CMakeLists.txt
├── prj.conf
├── src/
└── deps/              # Downloaded by west
    ├── zephyr/
    └── modules/
```

**west.yml**:
```yaml
manifest:
  remotes:
    - name: zephyrproject-rtos
      url-base: https://github.com/zephyrproject-rtos

  projects:
    - name: zephyr
      remote: zephyrproject-rtos
      revision: v3.6.0
      import: true

  self:
    path: .
```

**Pros**: Pinned Zephyr version, reproducible
**Cons**: More complex setup

## Module Pattern

### Creating a Module

```
my_module/
├── CMakeLists.txt
├── Kconfig
├── zephyr/
│   └── module.yml
└── src/
    ├── my_module.c
    └── my_module.h
```

**zephyr/module.yml**:
```yaml
build:
  cmake: .
  kconfig: Kconfig
```

**CMakeLists.txt**:
```cmake
if(CONFIG_MY_MODULE)
  zephyr_library()
  zephyr_library_sources(src/my_module.c)
  zephyr_include_directories(src)
endif()
```

**Kconfig**:
```kconfig
config MY_MODULE
    bool "My custom module"
    help
      Enable my custom module.
```

## Configuration Organization

### prj.conf Sections

```ini
# ===== Core System =====
CONFIG_PRINTK=y
CONFIG_LOG=y

# ===== Networking =====
CONFIG_NETWORKING=y
CONFIG_NET_IPV4=y

# ===== Application =====
CONFIG_MY_APP_FEATURE=y
CONFIG_MY_APP_BUFFER_SIZE=512
```

### Multiple Config Files

```cmake
# CMakeLists.txt
set(CONF_FILE
    ${CMAKE_CURRENT_SOURCE_DIR}/prj.conf
    ${CMAKE_CURRENT_SOURCE_DIR}/debug.conf  # Optional debug config
)
```

## Best Practices

### 1. Separate Concerns

```
src/
├── main.c           # Entry point only
├── app_init.c       # Initialization
├── app_tasks.c      # Task definitions
└── modules/         # Feature modules
```

### 2. Use Header Guards

```c
/* include/app/api.h */
#ifndef APP_API_H_
#define APP_API_H_

/* API declarations */

#endif /* APP_API_H_ */
```

### 3. Document Board Requirements

```markdown
<!-- boards/README.md -->
# Board Requirements

## nrf52840dk_nrf52840
- I2C0 for sensor (P0.26, P0.27)
- GPIO P0.13 for LED

## stm32f4_disco
- I2C1 for sensor (PB6, PB7)
- GPIO PA5 for LED
```

### 4. Version Your Application

Create `VERSION` file:
```
VERSION_MAJOR = 1
VERSION_MINOR = 2
PATCHLEVEL = 3
```

Use in CMakeLists.txt:
```cmake
include(${ZEPHYR_BASE}/cmake/app/version.cmake)
message(STATUS "App version: ${APP_VERSION_STRING}")
```

### 5. Use Sample and Test Directories

Provide examples:
```
samples/
└── basic/
    ├── README.md
    ├── CMakeLists.txt
    └── src/main.c
```

Include tests:
```
tests/
├── unit/
└── integration/
```

## Build Variants

### Debug vs Release

```cmake
# CMakeLists.txt
if(CONFIG_DEBUG_OPTIMIZATIONS)
    target_compile_definitions(app PRIVATE DEBUG_BUILD)
endif()
```

```bash
# Build debug
west build -b board -- -DCONFIG_DEBUG_OPTIMIZATIONS=y

# Build release
west build -b board -- -DCONFIG_SIZE_OPTIMIZATIONS=y
```

## Next Steps

Continue to [Part 3]({% link part3/index.md %}) to learn about kernel essentials.
