# Zephyr RTOS Development Environment
#
# This Dockerfile creates a complete build environment for the tutorial examples.
#
# Build:
#   docker build -t zephyr-dev .
#
# Run interactive:
#   docker run -it --rm -v $(pwd):/workdir zephyr-dev bash
#
# Build an example:
#   docker run -it --rm -v $(pwd):/workdir zephyr-dev \
#     west build -b qemu_cortex_m3 part1/hello-world

FROM ubuntu:22.04

ARG ZEPHYR_SDK_VERSION=0.16.5
ARG ZEPHYR_VERSION=3.6.0

# Prevent interactive prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Install dependencies
RUN apt-get update && apt-get install -y --no-install-recommends \
    git \
    cmake \
    ninja-build \
    gperf \
    ccache \
    dfu-util \
    device-tree-compiler \
    wget \
    curl \
    python3 \
    python3-dev \
    python3-pip \
    python3-setuptools \
    python3-tk \
    python3-wheel \
    python3-venv \
    xz-utils \
    file \
    make \
    gcc \
    gcc-multilib \
    g++-multilib \
    libsdl2-dev \
    libmagic1 \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# Create non-root user
RUN useradd -m -s /bin/bash zephyr
USER zephyr
WORKDIR /home/zephyr

# Install West
RUN pip3 install --user west

# Add local bin to PATH
ENV PATH="/home/zephyr/.local/bin:${PATH}"

# Download and install Zephyr SDK
RUN wget -q https://github.com/zephyrproject-rtos/sdk-ng/releases/download/v${ZEPHYR_SDK_VERSION}/zephyr-sdk-${ZEPHYR_SDK_VERSION}_linux-x86_64.tar.xz && \
    tar xf zephyr-sdk-${ZEPHYR_SDK_VERSION}_linux-x86_64.tar.xz && \
    rm zephyr-sdk-${ZEPHYR_SDK_VERSION}_linux-x86_64.tar.xz && \
    cd zephyr-sdk-${ZEPHYR_SDK_VERSION} && \
    ./setup.sh -t arm-zephyr-eabi -t x86_64-zephyr-elf -h -c

# Set SDK environment variable
ENV ZEPHYR_SDK_INSTALL_DIR=/home/zephyr/zephyr-sdk-${ZEPHYR_SDK_VERSION}

# Create Zephyr workspace
RUN mkdir -p ~/zephyrproject && \
    cd ~/zephyrproject && \
    west init -m https://github.com/zephyrproject-rtos/zephyr --mr v${ZEPHYR_VERSION} && \
    west update --narrow -o=--depth=1 && \
    west zephyr-export

# Install Python dependencies
RUN pip3 install --user -r ~/zephyrproject/zephyr/scripts/requirements.txt

# Set Zephyr environment
ENV ZEPHYR_BASE=/home/zephyr/zephyrproject/zephyr
ENV PATH="${ZEPHYR_BASE}/scripts:${PATH}"

# Working directory for examples
WORKDIR /workdir

# Default command - show help
CMD ["bash", "-c", "echo 'Zephyr Development Environment' && echo '' && echo 'Build examples:' && echo '  west build -b <board> <path>' && echo '' && echo 'Available boards:' && echo '  qemu_cortex_m3  - ARM Cortex-M3 emulator' && echo '  qemu_x86        - x86 emulator' && echo '  native_sim      - Native simulator' && echo '  nrf52840dk      - Nordic nRF52840 DK' && echo '' && echo 'Example:' && echo '  west build -b qemu_cortex_m3 part1/hello-world' && echo '  west build -t run' && bash"]
