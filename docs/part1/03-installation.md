---
layout: default
title: "Installation"
parent: "Part 1: Getting Started"
nav_order: 3
---

# Installing the Zephyr SDK

This guide covers installing the Zephyr SDK on Linux, macOS, and Windows.

## Prerequisites

All platforms need:
- Python 3.10 or later
- pip (Python package manager)
- CMake 3.20.5 or later
- Ninja build system

## Linux Installation

### Ubuntu/Debian

```bash
# Install dependencies
sudo apt update
sudo apt install --no-install-recommends git cmake ninja-build gperf \
  ccache dfu-util device-tree-compiler wget \
  python3-dev python3-pip python3-setuptools python3-tk python3-wheel \
  xz-utils file make gcc gcc-multilib g++-multilib libsdl2-dev libmagic1

# Install west
pip3 install --user west

# Add ~/.local/bin to PATH (add to ~/.bashrc)
echo 'export PATH="$HOME/.local/bin:$PATH"' >> ~/.bashrc
source ~/.bashrc
```

### Fedora

```bash
# Install dependencies
sudo dnf install git cmake ninja-build gperf ccache dfu-util \
  dtc wget python3-pip python3-tkinter xz file glibc-devel.i686 \
  libstdc++-devel.i686 SDL2-devel

# Install west
pip3 install --user west
```

### Download and Install Zephyr SDK

```bash
# Download SDK (check for latest version at https://github.com/zephyrproject-rtos/sdk-ng/releases)
cd ~
wget https://github.com/zephyrproject-rtos/sdk-ng/releases/download/v0.16.8/zephyr-sdk-0.16.8_linux-x86_64.tar.xz
wget -O - https://github.com/zephyrproject-rtos/sdk-ng/releases/download/v0.16.8/sha256.sum | shasum --check --ignore-missing

# Extract SDK
tar xvf zephyr-sdk-0.16.8_linux-x86_64.tar.xz

# Run setup script
cd zephyr-sdk-0.16.8
./setup.sh

# Register CMake package
sudo cp ~/zephyr-sdk-0.16.8/cmake/zephyr-sdk.cmake /usr/share/cmake-*/Modules/
```

## macOS Installation

### Using Homebrew

```bash
# Install Homebrew if not already installed
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install cmake ninja gperf python3 ccache qemu dtc wget libmagic

# Install west
pip3 install west

# Download SDK
cd ~
wget https://github.com/zephyrproject-rtos/sdk-ng/releases/download/v0.16.8/zephyr-sdk-0.16.8_macos-x86_64.tar.xz

# For Apple Silicon (M1/M2)
# wget https://github.com/zephyrproject-rtos/sdk-ng/releases/download/v0.16.8/zephyr-sdk-0.16.8_macos-aarch64.tar.xz

# Extract and setup
tar xvf zephyr-sdk-0.16.8_macos-x86_64.tar.xz
cd zephyr-sdk-0.16.8
./setup.sh
```

## Windows Installation

### Using WSL2 (Recommended)

Windows Subsystem for Linux 2 provides the best experience for Zephyr development.

```powershell
# In PowerShell as Administrator
wsl --install -d Ubuntu-22.04
```

After restart, open Ubuntu and follow the Linux installation instructions above.

### Native Windows (Advanced)

```powershell
# Install Chocolatey
Set-ExecutionPolicy Bypass -Scope Process -Force
[System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072
iex ((New-Object System.Net.WebClient).DownloadString('https://chocolatey.org/install.ps1'))

# Install dependencies
choco install cmake --installargs 'ADD_CMAKE_TO_PATH=System'
choco install ninja gperf python git dtc-msys2 wget

# Install west
pip3 install west

# Download and extract SDK
cd $HOME
Invoke-WebRequest -Uri "https://github.com/zephyrproject-rtos/sdk-ng/releases/download/v0.16.8/zephyr-sdk-0.16.8_windows-x86_64.7z" -OutFile "zephyr-sdk.7z"
7z x zephyr-sdk.7z

# Run setup
cd zephyr-sdk-0.16.8
setup.cmd
```

## Environment Variables

Add these to your shell configuration:

```bash
# Add to ~/.bashrc or ~/.zshrc
export ZEPHYR_TOOLCHAIN_VARIANT=zephyr
export ZEPHYR_SDK_INSTALL_DIR=~/zephyr-sdk-0.16.8
```

## Verify Installation

```bash
# Check west
west --version

# Check CMake
cmake --version

# Check Python
python3 --version

# Check SDK toolchains
ls $ZEPHYR_SDK_INSTALL_DIR/arm-zephyr-eabi/bin/
```

## Troubleshooting

### "west: command not found"
Ensure `~/.local/bin` is in your PATH:
```bash
export PATH="$HOME/.local/bin:$PATH"
```

### CMake can't find Zephyr SDK
Run the SDK setup script again:
```bash
cd ~/zephyr-sdk-0.16.8
./setup.sh
```

### Permission denied errors
Don't run pip with sudo. Use `--user` flag or a virtual environment.

## Next Steps

With the SDK installed, let's [set up west]({% link part1/04-west-setup.md %}) to manage your Zephyr workspace.
