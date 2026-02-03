---
layout: default
title: "Part 7: Case Studies"
nav_order: 8
has_children: true
permalink: /part7/
---

# Part 7: Professional Case Studies

This section analyzes real-world Zephyr projects to demonstrate professional-level patterns and architectures.

## What You'll Learn

- OpenBIC: Open-source BMC firmware architecture
- ASPEED RoT: Security-focused Zephyr implementation
- Design patterns from production systems
- Best practices for large-scale Zephyr projects

## Case Studies

### OpenBIC - BMC Firmware

| Chapter | Description |
|---------|-------------|
| [OpenBIC Overview]({% link part7/01-openbic-overview.md %}) | Introduction and architecture |
| [Architecture]({% link part7/02-openbic-architecture.md %}) | Module organization |
| [Protocols]({% link part7/03-openbic-protocols.md %}) | IPMI/PLDM implementation |
| [Sensors]({% link part7/04-openbic-sensors.md %}) | Sensor framework |
| [Lessons]({% link part7/05-openbic-lessons.md %}) | Design patterns |

### ASPEED RoT - Root of Trust

| Chapter | Description |
|---------|-------------|
| [RoT Overview]({% link part7/06-aspeed-rot-overview.md %}) | Security architecture |
| [Secure Boot]({% link part7/07-aspeed-rot-secure-boot.md %}) | Boot verification |
| [Attestation]({% link part7/08-aspeed-rot-attestation.md %}) | DICE/SPDM |
| [PFR]({% link part7/09-aspeed-rot-pfr.md %}) | Platform Firmware Resilience |
| [Lessons]({% link part7/10-aspeed-rot-lessons.md %}) | Security patterns |

## Why Study These Projects?

```mermaid
flowchart LR
    subgraph Learning["Learning Value"]
        ARCH[Architecture Patterns]
        SCALE[Scalability]
        PROD[Production Quality]
    end

    subgraph OpenBIC["OpenBIC"]
        BMC[BMC Firmware]
        IPMI[IPMI/PLDM]
        SENSOR[Sensor Mgmt]
    end

    subgraph RoT["ASPEED RoT"]
        SEC[Security]
        BOOT[Secure Boot]
        ATTEST[Attestation]
    end

    OpenBIC --> Learning
    RoT --> Learning
```

{: .important }
These case studies demonstrate how Zephyr is used in demanding production environments.
