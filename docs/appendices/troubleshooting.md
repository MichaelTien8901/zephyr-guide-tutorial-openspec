---
layout: default
title: "Troubleshooting"
parent: "Appendices"
nav_order: 3
---

# Troubleshooting Guide

Common issues and solutions for Zephyr development.

## Build Issues

### "west: command not found"

```bash
# Install west
pip3 install --user west

# Add to PATH
export PATH="$HOME/.local/bin:$PATH"
echo 'export PATH="$HOME/.local/bin:$PATH"' >> ~/.bashrc
```

### "ZEPHYR_BASE not set"

```bash
# Source the environment
source ~/zephyrproject/zephyr/zephyr-env.sh

# Or set manually
export ZEPHYR_BASE=~/zephyrproject/zephyr
```

### "Board not found"

```bash
# List available boards
west boards | grep <pattern>

# Check board name spelling
ls $ZEPHYR_BASE/boards/
```

### CMake errors

```bash
# Clean build
west build -p -b <board> <app>

# Or remove build directory
rm -rf build
west build -b <board> <app>
```

### "Could not find toolchain"

```bash
# Set toolchain variables
export ZEPHYR_TOOLCHAIN_VARIANT=zephyr
export ZEPHYR_SDK_INSTALL_DIR=~/zephyr-sdk-0.16.8

# Re-run SDK setup
cd ~/zephyr-sdk-0.16.8
./setup.sh
```

## Flash Issues

### "No device found"

1. Check USB connection
2. Verify board is powered
3. Check udev rules (Linux):
```bash
# Add udev rules for common debug probes
sudo cp $ZEPHYR_SDK_INSTALL_DIR/sysroots/x86_64-pokysdk-linux/usr/share/openocd/contrib/60-openocd.rules /etc/udev/rules.d/
sudo udevadm control --reload-rules
```

### "Permission denied"

```bash
# Add user to dialout group (Linux)
sudo usermod -a -G dialout $USER
# Log out and back in
```

### Flash fails but build succeeds

```bash
# Try different runner
west flash --runner jlink
west flash --runner openocd
west flash --runner nrfjprog
```

## Runtime Issues

### Stack overflow

```ini
# prj.conf - Enable stack protection
CONFIG_STACK_SENTINEL=y
CONFIG_STACK_CANARIES=y

# Increase stack size
CONFIG_MAIN_STACK_SIZE=2048
```

### Heap allocation fails

```ini
# Increase heap size
CONFIG_HEAP_MEM_POOL_SIZE=4096
```

### Printk not showing output

```ini
# Enable console
CONFIG_PRINTK=y
CONFIG_CONSOLE=y
CONFIG_UART_CONSOLE=y
```

Check device tree has correct console:
```dts
/ {
    chosen {
        zephyr,console = &uart0;
    };
};
```

### Thread not running

1. Check priority (lower number = higher priority)
2. Verify stack size is sufficient
3. Check for blocking calls
4. Enable thread monitoring:
```ini
CONFIG_THREAD_MONITOR=y
CONFIG_THREAD_NAME=y
```

## Kconfig Issues

### Option not taking effect

```bash
# Check final configuration
cat build/zephyr/.config | grep <OPTION>

# Use menuconfig to verify dependencies
west build -t menuconfig
```

### "Unknown configuration option"

1. Check spelling
2. Verify the feature is enabled
3. Check Kconfig dependencies

## Device Tree Issues

### "Node not found"

```bash
# Check generated DTS
cat build/zephyr/zephyr.dts | grep <node_name>

# Verify node has status = "okay"
```

### Binding errors

```bash
# Check binding file exists
ls $ZEPHYR_BASE/dts/bindings/ | grep <compatible>
```

## QEMU Issues

### QEMU not starting

```bash
# Check QEMU is installed
which qemu-system-arm

# Install if needed
sudo apt install qemu-system-arm  # Ubuntu/Debian
brew install qemu                  # macOS
```

### QEMU exits immediately

Check for runtime errors in application:
```ini
CONFIG_ASSERT=y
CONFIG_ASSERT_LEVEL=2
```

## Debugging Tips

### Enable verbose logging

```ini
CONFIG_LOG=y
CONFIG_LOG_DEFAULT_LEVEL=4
CONFIG_LOG_MODE_IMMEDIATE=y
```

### Use shell for inspection

```ini
CONFIG_SHELL=y
CONFIG_KERNEL_SHELL=y
```

Shell commands:
```
kernel threads    # List threads
kernel stacks     # Show stack usage
device list       # List devices
```

### GDB debugging

```bash
# Start debug server
west debugserver

# In another terminal
arm-zephyr-eabi-gdb build/zephyr/zephyr.elf
(gdb) target remote localhost:3333
(gdb) monitor reset halt
(gdb) continue
```

## Getting Help

If these solutions don't work:

1. **Search existing issues**: [GitHub Issues](https://github.com/zephyrproject-rtos/zephyr/issues)
2. **Ask on Discord**: [Zephyr Discord](https://discord.com/invite/zephyrproject)
3. **Post to mailing list**: [Zephyr Users](https://lists.zephyrproject.org/g/users)

When reporting issues, include:
- Zephyr version (`west list zephyr`)
- Board name
- prj.conf contents
- Error messages
- Steps to reproduce
