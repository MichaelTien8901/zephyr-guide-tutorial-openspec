---
layout: default
title: "Testing"
parent: "Part 6: Advanced Topics"
nav_order: 16
---

# Testing in Zephyr

Zephyr provides Ztest framework for unit testing and Twister for test automation.

## Ztest Framework

### Basic Test Structure

```c
#include <zephyr/ztest.h>

/* Test suite setup (optional) */
static void *suite_setup(void)
{
    /* Allocate resources */
    return NULL;
}

/* Test suite teardown (optional) */
static void suite_teardown(void *fixture)
{
    /* Free resources */
}

/* Before each test (optional) */
static void test_before(void *fixture)
{
    /* Reset state */
}

/* After each test (optional) */
static void test_after(void *fixture)
{
    /* Cleanup */
}

/* Test case */
ZTEST(my_suite, test_addition)
{
    int result = 1 + 1;
    zassert_equal(result, 2, "1 + 1 should be 2");
}

ZTEST(my_suite, test_string)
{
    const char *str = "hello";
    zassert_not_null(str, "String should not be null");
    zassert_equal(strlen(str), 5, "String length should be 5");
}

/* Register test suite */
ZTEST_SUITE(my_suite, NULL, suite_setup, test_before, test_after, suite_teardown);
```

### Assertions

```c
/* Basic assertions */
zassert_true(condition, "Message");
zassert_false(condition, "Message");
zassert_is_null(ptr, "Message");
zassert_not_null(ptr, "Message");

/* Equality */
zassert_equal(a, b, "Message");
zassert_not_equal(a, b, "Message");

/* Comparison */
zassert_within(actual, expected, delta, "Message");

/* Memory */
zassert_mem_equal(buf1, buf2, len, "Message");

/* Unconditional fail */
zassert_unreachable("Should not reach here");
```

### Test with Fixture

```c
struct test_fixture {
    uint8_t buffer[64];
    size_t count;
};

static void *fixture_setup(void)
{
    struct test_fixture *f = k_malloc(sizeof(*f));
    zassert_not_null(f, "Failed to allocate fixture");
    memset(f, 0, sizeof(*f));
    return f;
}

static void fixture_teardown(void *fixture)
{
    k_free(fixture);
}

ZTEST_F(my_suite, test_with_fixture)
{
    struct test_fixture *f = fixture;

    f->buffer[0] = 0x42;
    f->count = 1;

    zassert_equal(f->buffer[0], 0x42, "Buffer value incorrect");
    zassert_equal(f->count, 1, "Count incorrect");
}

ZTEST_SUITE(my_suite, NULL, fixture_setup, NULL, NULL, fixture_teardown);
```

### Test File Structure

```
tests/
├── my_feature/
│   ├── CMakeLists.txt
│   ├── prj.conf
│   ├── testcase.yaml
│   └── src/
│       └── main.c
```

#### CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(my_test)

target_sources(app PRIVATE src/main.c)
```

#### prj.conf

```kconfig
CONFIG_ZTEST=y
CONFIG_ZTEST_NEW_API=y
```

#### testcase.yaml

```yaml
tests:
  my_feature.basic:
    tags: unit
    platform_allow:
      - native_sim
      - qemu_cortex_m3
    integration_platforms:
      - native_sim
```

## Running Tests

### Manual Run

```bash
# Build and run on native_sim
west build -b native_sim tests/my_feature
west build -t run

# Or for hardware
west build -b nrf52840dk_nrf52840 tests/my_feature
west flash
```

### Using Twister

```bash
# Run all tests
west twister

# Run specific tests
west twister -T tests/my_feature

# Run on specific platform
west twister -p native_sim -T tests/

# Run with specific tag
west twister --tag unit

# Generate coverage report
west twister --coverage
```

## Mocking

### Mock Functions

```c
#include <zephyr/fff.h>

DEFINE_FFF_GLOBALS;

/* Declare mock for function */
FAKE_VALUE_FUNC(int, sensor_read, const struct device *, int *);

ZTEST(mock_suite, test_sensor_read)
{
    int value;

    /* Set up mock return value */
    sensor_read_fake.return_val = 0;

    /* Call code under test */
    int ret = sensor_read(NULL, &value);

    /* Verify mock was called */
    zassert_equal(sensor_read_fake.call_count, 1, "Should be called once");
    zassert_equal(ret, 0, "Should return 0");
}

static void mock_before(void *fixture)
{
    /* Reset all mocks before each test */
    RESET_FAKE(sensor_read);
    FFF_RESET_HISTORY();
}

ZTEST_SUITE(mock_suite, NULL, NULL, mock_before, NULL, NULL);
```

### Kconfig for Mocking

```kconfig
CONFIG_ZTEST=y
CONFIG_ZTEST_MOCKING=y
```

## Integration Testing

### Testing with Hardware

```c
ZTEST(integration, test_gpio)
{
    const struct device *gpio = DEVICE_DT_GET(DT_NODELABEL(gpio0));

    zassert_true(device_is_ready(gpio), "GPIO not ready");

    int ret = gpio_pin_configure(gpio, 13, GPIO_OUTPUT);
    zassert_equal(ret, 0, "Config failed");

    ret = gpio_pin_set(gpio, 13, 1);
    zassert_equal(ret, 0, "Set failed");
}
```

### Testing with Threads

```c
static K_SEM_DEFINE(test_sem, 0, 1);

static void thread_entry(void *p1, void *p2, void *p3)
{
    k_msleep(100);
    k_sem_give(&test_sem);
}

K_THREAD_STACK_DEFINE(test_stack, 1024);
static struct k_thread test_thread;

ZTEST(thread_suite, test_thread_signaling)
{
    k_tid_t tid = k_thread_create(&test_thread, test_stack, 1024,
                                  thread_entry, NULL, NULL, NULL,
                                  K_PRIO_PREEMPT(10), 0, K_NO_WAIT);

    int ret = k_sem_take(&test_sem, K_MSEC(500));
    zassert_equal(ret, 0, "Semaphore should be given");

    k_thread_join(&test_thread, K_FOREVER);
}
```

## Test Configuration

### testcase.yaml Options

```yaml
tests:
  feature.test_name:
    # Platform filtering
    platform_allow:
      - native_sim
      - qemu_cortex_m3
    platform_exclude:
      - nrf52840dk_nrf52840

    # Architecture filtering
    arch_allow:
      - arm
      - x86

    # Tags for grouping
    tags:
      - unit
      - bluetooth

    # Timeout
    timeout: 60

    # Extra configs
    extra_configs:
      - CONFIG_DEBUG=y

    # Skip conditions
    skip: true
    filter: CONFIG_BT

    # Expected to fail
    harness: console
    harness_config:
      type: one_line
      regex:
        - "PASS"
```

## Code Coverage

```bash
# Enable coverage in build
west build -b native_sim -- -DCONFIG_COVERAGE=y

# Run with coverage
west twister --coverage -p native_sim

# Generate HTML report
genhtml twister-out/coverage/coverage.info -o coverage_report
```

## Complete Test Example

```c
#include <zephyr/ztest.h>
#include <zephyr/kernel.h>

/* Module under test */
#include "my_module.h"

struct test_fixture {
    struct my_module_config config;
    struct my_module_data data;
};

static void *fixture_setup(void)
{
    struct test_fixture *f = k_malloc(sizeof(*f));
    zassert_not_null(f);

    f->config.param1 = 10;
    f->config.param2 = 20;
    my_module_init(&f->data, &f->config);

    return f;
}

static void fixture_teardown(void *fixture)
{
    struct test_fixture *f = fixture;
    my_module_cleanup(&f->data);
    k_free(f);
}

ZTEST_F(my_module, test_init)
{
    zassert_true(fixture->data.initialized);
}

ZTEST_F(my_module, test_process)
{
    int result = my_module_process(&fixture->data, 5);
    zassert_equal(result, 50, "5 * 10 should be 50");
}

ZTEST_F(my_module, test_boundary)
{
    int result = my_module_process(&fixture->data, 0);
    zassert_equal(result, 0);

    result = my_module_process(&fixture->data, INT_MAX);
    zassert_true(result > 0);
}

ZTEST_SUITE(my_module, NULL, fixture_setup, NULL, NULL, fixture_teardown);
```

## Best Practices

1. **Test one thing per test** - Keep tests focused
2. **Use fixtures** - Share setup/teardown code
3. **Test edge cases** - Boundaries, null inputs
4. **Use meaningful names** - Describe what's being tested
5. **Run on native_sim** - Fast iteration
6. **Use Twister** - For CI/CD automation

## Next Steps

Continue to [Part 7: Professional Projects]({% link part7/index.md %}) for real-world case studies.
