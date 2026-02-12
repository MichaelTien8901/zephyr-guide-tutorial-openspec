---
layout: default
title: "Quality Assurance"
parent: "Part 6: Advanced Topics"
nav_order: 6
---

# Quality Assurance

Beyond testing, Zephyr provides tools to assess and improve the overall quality of your embedded product — from memory footprint to code analysis to supply chain tracking.

## Footprint and Memory Usage

### Build-Time Size Reporting

Every Zephyr build outputs a memory usage summary:

```bash
west build -b your_board your_app

# Output includes:
# Memory region         Used Size  Region Size  %age Used
#            FLASH:      42368 B         1 MB      4.04%
#             SRAM:      11520 B       256 KB      4.39%
```

### Detailed Footprint Analysis

```bash
# Generate a detailed memory report
west build -t ram_report    # RAM usage by symbol
west build -t rom_report    # ROM/Flash usage by symbol

# Output shows per-file and per-symbol breakdown
# Path                                       Size
# src/main.c                                 1024
#   main                                      512
#   sensor_init                               256
#   process_data                              256
# ...
```

### Puncover Integration

[Puncover](https://github.com/HBehrens/puncover) provides a web-based visualization of code size:

```bash
# Install puncover
pip install puncover

# Generate puncover-compatible build
west build -b your_board your_app

# Run puncover server
puncover --elf_file build/zephyr/zephyr.elf --gcc_tools_base arm-zephyr-eabi- --port 5000

# Open http://localhost:5000 in your browser
```

### Tracking Size Over Time

Monitor footprint changes across commits:

```bash
# Compare size between builds
arm-zephyr-eabi-size build/zephyr/zephyr.elf
#    text    data     bss     dec     hex filename
#   38924    3444    8076   50444    c50c build/zephyr/zephyr.elf
```

{: .tip }
Add size reporting to your CI pipeline to catch unexpected size increases early.

### Optimization Techniques

```ini
# Size optimization (smaller but slower)
CONFIG_SIZE_OPTIMIZATIONS=y

# Remove unused sections
CONFIG_HAS_FLASH_LOAD_OFFSET=y

# Strip debug info from release builds
CONFIG_DEBUG_OPTIMIZATIONS=n

# Disable features you don't use
CONFIG_PRINTK=n                  # If using LOG only
CONFIG_BOOT_BANNER=n             # Saves a few hundred bytes
CONFIG_THREAD_NAME=n             # If not debugging threads
```

## Static Code Analysis

### Zephyr's Built-in Checks

Zephyr's build system includes several code quality checks:

```bash
# Run compliance checks (code style, commit messages)
west build -t compliance

# Check for common coding issues
west build -t checkpatch
```

### Clang-Tidy Integration

```bash
# Build with clang-tidy analysis
west build -b your_board your_app -- \
    -DZEPHYR_TOOLCHAIN_VARIANT=llvm \
    -DCONFIG_COMPILER_WARNINGS_AS_ERRORS=y

# Run clang-tidy separately
run-clang-tidy -p build/ src/
```

### Cppcheck

```bash
# Install cppcheck
sudo apt install cppcheck

# Run on your source files
cppcheck --enable=all --suppress=missingIncludeSystem \
    -I build/zephyr/include/generated \
    -I $ZEPHYR_BASE/include \
    src/
```

### MISRA C Compliance

For safety-critical applications, check MISRA C compliance:

```bash
# Use a MISRA-capable checker (cppcheck with MISRA addon, or commercial tools)
cppcheck --addon=misra --suppress=missingIncludeSystem src/
```

{: .note }
Full MISRA compliance is complex. Focus on the rules most relevant to your safety level (SIL/ASIL).

## Code Coverage

Code coverage helps you understand which code paths your tests exercise.

### Using native_sim with gcov

```ini
# prj.conf for coverage build
CONFIG_COVERAGE=y
CONFIG_COVERAGE_GCOV=y
```

```bash
# Build with coverage
west build -b native_sim tests/my_test -- -DCONFIG_COVERAGE=y

# Run tests
./build/zephyr/zephyr.exe

# Generate coverage report
gcovr -r . --html-details coverage.html build/
# Or
lcov --capture --directory build/ --output-file coverage.info
genhtml coverage.info --output-directory coverage_html/
```

### Coverage with Twister

```bash
# Run tests with coverage collection
west twister -p native_sim -T tests/ --coverage

# Coverage report is generated automatically
# Check twister-out/coverage/
```

### Branch Coverage

```bash
# gcovr with branch coverage
gcovr -r . --html-details --branches coverage.html build/

# lcov with branch coverage
lcov --capture --directory build/ --output-file coverage.info --rc lcov_branch_coverage=1
```

### CI Integration

```yaml
# GitHub Actions example
- name: Run tests with coverage
  run: |
    west twister -p native_sim -T tests/ --coverage
    gcovr -r . --xml coverage.xml build/

- name: Upload coverage
  uses: codecov/codecov-action@v3
  with:
    files: coverage.xml
```

## Software Bill of Materials (SBOM)

An SBOM documents all software components in your product — critical for supply chain security and license compliance.

### West Manifest as SBOM Source

Your `west.yml` manifest already tracks all Zephyr modules and their versions:

```yaml
# west.yml
manifest:
  projects:
    - name: zephyr
      revision: v3.6.0
    - name: mcuboot
      revision: v2.0.0
    - name: mbedtls
      revision: mbedtls-3.5.1
```

### Generating SBOM

```bash
# List all west projects and their versions
west list --format "{name} {revision} {url}"

# Generate SPDX document
# (Zephyr is working on native SPDX support)
```

### SPDX Format

SPDX (Software Package Data Exchange) is the standard format for SBOMs:

```json
{
  "spdxVersion": "SPDX-2.3",
  "name": "my-zephyr-product",
  "packages": [
    {
      "name": "zephyr",
      "versionInfo": "3.6.0",
      "downloadLocation": "https://github.com/zephyrproject-rtos/zephyr",
      "licenseConcluded": "Apache-2.0"
    },
    {
      "name": "mbedtls",
      "versionInfo": "3.5.1",
      "licenseConcluded": "Apache-2.0"
    }
  ]
}
```

### License Compliance

```bash
# Check licenses of all dependencies
west list --format "{name}: {url}" | while read line; do
    echo "$line"
done

# Key licenses in the Zephyr ecosystem:
# Zephyr kernel: Apache-2.0
# mbedTLS: Apache-2.0
# MCUboot: Apache-2.0
# LittleFS: BSD-3-Clause
# FatFS: BSD-1-Clause (custom)
```

{: .warning }
Some vendor HALs bundled with Zephyr have different licenses (often BSD or proprietary with restrictions). Always verify before shipping.

## Quality Checklist for Production

Use this checklist before releasing your Zephyr-based product:

### Build Quality
- [ ] No compiler warnings (`CONFIG_COMPILER_WARNINGS_AS_ERRORS=y`)
- [ ] Static analysis clean (cppcheck or clang-tidy)
- [ ] Size optimization applied
- [ ] Debug features disabled

### Test Quality
- [ ] Unit tests pass (Ztest on native_sim)
- [ ] Integration tests pass
- [ ] Code coverage meets target (recommend 80%+)
- [ ] Hardware-in-the-loop tests pass

### Security
- [ ] SBOM generated and reviewed
- [ ] No known CVEs in dependencies
- [ ] Stack overflow protection enabled
- [ ] Secure boot configured (if applicable)

### Documentation
- [ ] API documentation complete
- [ ] Build instructions documented
- [ ] Known issues documented
- [ ] Change log maintained

## Next Steps

Continue to [Working without Hardware]({% link part6/07-working-without-hardware.md %}) to learn about QEMU, native_sim, and emulation strategies.
