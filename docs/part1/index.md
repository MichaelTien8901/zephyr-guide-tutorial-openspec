---
layout: default
title: "Part 1: Getting Started"
nav_order: 2
has_children: true
permalink: /part1/
---

# Part 1: Getting Started

This section guides you through setting up your Zephyr development environment and creating your first application.

## What You'll Learn

- What Zephyr RTOS is and when to use it
- Why Zephyr is a great fit for embedded products
- How to install the Zephyr SDK and toolchains
- Setting up the `west` meta-tool for workspace management
- Configuring Docker for reproducible builds
- Setting up VS Code for Zephyr development
- Building and running your first Zephyr application

## Chapters

| Chapter | Description |
|---------|-------------|
| [Introduction]({% link part1/01-introduction.md %}) | What is Zephyr, history, ecosystem overview |
| [Zephyr for Products]({% link part1/02-zephyr-products.md %}) | Why Zephyr, hardware support, licensing, product considerations |
| [Installation]({% link part1/03-installation.md %}) | Zephyr SDK installation for Linux/macOS/Windows |
| [West Setup]({% link part1/04-west-setup.md %}) | West tool installation and workspace initialization |
| [Toolchains]({% link part1/05-toolchains.md %}) | ARM, RISC-V, x86 toolchain configuration |
| [Docker Setup]({% link part1/06-docker-setup.md %}) | Docker-based Zephyr development environment |
| [IDE Setup]({% link part1/07-ide-setup.md %}) | VS Code extensions and configuration |
| [Hello World]({% link part1/08-hello-world.md %}) | Your first Zephyr application |

## Prerequisites

Before starting this section, ensure you have:

- A computer running Linux, macOS, or Windows (with WSL2)
- Administrator/sudo access for installing software
- At least 10GB of free disk space
- Internet connection for downloading tools

## Time Estimate

Completing this section typically takes 1-2 hours, depending on your internet speed and familiarity with command-line tools.

{: .tip }
If you want to get started quickly without installing everything locally, skip ahead to [Docker Setup]({% link part1/06-docker-setup.md %}) for a containerized environment.
