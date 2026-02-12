## Context

This project creates a comprehensive Zephyr RTOS tutorial guide hosted on GitHub Pages. The guide targets developers from beginner to professional level, covering environment setup, core concepts, advanced features, and real-world case studies (OpenBIC, ASPEED RoT).

**Current state**: No documentation site exists yet. The project structure follows the rust-guide-tutorial-openspec reference, using Jekyll with just-the-docs theme and Mermaid diagrams.

**Constraints**:
- Must work with GitHub Pages (Jekyll static site)
- Mermaid diagrams must render correctly with dark theme
- Docker support required for both docs development and Zephyr builds
- Content must progress logically from beginner to professional

## Goals / Non-Goals

**Goals:**
- Create a complete Jekyll documentation site with proper mermaid configuration
- Provide step-by-step setup guides including Docker-based development
- Cover Zephyr fundamentals through advanced topics with clear explanations
- Include flowcharts and diagrams for complex concepts
- Document real-world projects (OpenBIC, ASPEED RoT) as professional case studies
- Provide runnable code examples for each major topic

**Non-Goals:**
- Not a replacement for official Zephyr documentation
- Not covering every Zephyr subsystem exhaustively
- Not providing hardware-specific guides for every supported board
- Not creating production-ready application templates

## Decisions

### 1. Site Structure - Part-based Organization

**Decision**: Organize content into 7 parts plus appendices, mirroring the rust-guide-tutorial structure.

**Rationale**: Part-based organization provides clear progression and allows readers to start at their skill level. Grouping related topics improves navigation.

**Structure**:
```
docs/
├── part1/    # Getting Started (setup, tools, first app)
├── part2/    # Fundamentals (build system, device tree, Kconfig)
├── part3/    # Kernel Essentials (threads, scheduling, timing)
├── part4/    # Synchronization & IPC (mutexes, queues, events)
├── part5/    # Device Drivers (driver model, GPIO, I2C, SPI)
├── part6/    # Advanced Topics (networking, BLE, power mgmt)
├── part7/    # Professional (OpenBIC, ASPEED RoT case studies)
└── appendices/  # Reference material, glossary
```

**Alternatives considered**:
- Topic-based flat structure: Rejected - harder to show progression
- Single long document: Rejected - poor navigation, overwhelming

### 2. Mermaid Configuration - Paper-like Theme

**Decision**: Use custom mermaid configuration with paper-like background colors matching the reference project.

**Rationale**: Dark theme sites need special mermaid styling for readability. The paper-like background (#d5d0c8) provides contrast while being easy on the eyes.

**Implementation**:
- `_includes/head_custom.html` - CSS overrides for mermaid containers
- `_includes/mermaid_config.js` - Mermaid initialization with theme variables

### 3. Docker Strategy - Dual Container Approach

**Decision**: Provide two Docker configurations:
1. `docs/Dockerfile` - Jekyll documentation development
2. `docs/examples/Dockerfile` - Zephyr SDK for building examples

**Rationale**: Separating concerns allows docs contributors to work without full Zephyr SDK, while example builders get a reproducible environment.

**Alternatives considered**:
- Single container with everything: Rejected - too large, slower builds
- No Docker: Rejected - inconsistent environments, harder onboarding

### 4. Example Organization - By Topic with Standalone Projects

**Decision**: Each example is a complete Zephyr application in `docs/examples/<part>/<topic>/`.

**Rationale**: Standalone projects can be built and tested independently. Matching directory structure to parts aids navigation.

**Example structure**:
```
docs/examples/
├── part3/
│   ├── threads/
│   │   ├── CMakeLists.txt
│   │   ├── prj.conf
│   │   └── src/main.c
│   └── semaphores/
└── part5/
    └── gpio-blink/
```

### 5. Content Depth - Conceptual + Practical

**Decision**: Each topic includes:
1. Overview (what and why)
2. When to use (decision guidance)
3. How it works (with mermaid diagrams)
4. Code examples (with explanations)
5. Common pitfalls

**Rationale**: Readers need both conceptual understanding and practical skills. Diagrams clarify complex flows. Pitfalls prevent common mistakes.

### 6. Case Studies - OpenBIC and ASPEED RoT

**Decision**: Part 7 dedicates sections to analyzing these production Zephyr projects:
- OpenBIC: BMC firmware, IPMI/PLDM, sensor framework
- ASPEED RoT: Security architecture, secure boot, PFR

**Rationale**: Real-world examples demonstrate professional Zephyr usage. These projects are open-source and well-documented, making them suitable references.

## Risks / Trade-offs

**[Zephyr version changes]** → Pin to specific Zephyr version (e.g., 3.6.x), document upgrade path. Include version notice in docs.

**[Mermaid rendering issues]** → Test diagrams locally before publishing. Use simple diagram types that render consistently.

**[Example build failures]** → Use GitHub Actions to test examples. Pin toolchain versions in Docker.

**[Content becomes outdated]** → Focus on stable APIs. Link to official docs for volatile features. Add "last updated" dates.

**[Scope creep]** → Stick to non-goals. Advanced hardware-specific content can be appendices or external links.

**[OpenBIC/ASPEED complexity]** → Focus on architecture and patterns, not line-by-line code review. Link to upstream repos for details.
