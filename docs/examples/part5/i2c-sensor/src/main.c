/*
 * I2C Sensor Example
 *
 * Demonstrates I2C communication with a temperature sensor.
 * Uses a simulated sensor if no hardware is present.
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(i2c_sensor, LOG_LEVEL_DBG);

/* Common temperature sensor addresses */
#define TMP102_ADDR     0x48
#define TEMP_REG        0x00
#define CONFIG_REG      0x01

/* Get I2C device from devicetree */
#define I2C_NODE DT_ALIAS(i2c_0)

#if !DT_NODE_HAS_STATUS(I2C_NODE, okay)
#warning "I2C0 not defined, using simulation mode"
#define SIMULATION_MODE 1
#else
#define SIMULATION_MODE 0
#endif

#if !SIMULATION_MODE
static const struct device *i2c_dev = DEVICE_DT_GET(I2C_NODE);
#endif

/* Read temperature from TMP102 sensor */
static int read_temperature(int16_t *temp_raw)
{
#if SIMULATION_MODE
	/* Simulate temperature reading */
	static int16_t sim_temp = 2500;  /* 25.00°C in 0.01°C units */
	sim_temp += (sys_rand32_get() % 100) - 50;
	*temp_raw = sim_temp;
	return 0;
#else
	uint8_t buf[2];
	int ret;

	/* Read temperature register */
	ret = i2c_burst_read(i2c_dev, TMP102_ADDR, TEMP_REG, buf, 2);
	if (ret < 0) {
		LOG_ERR("Failed to read temperature: %d", ret);
		return ret;
	}

	/* TMP102: 12-bit value, left-aligned in 2 bytes */
	*temp_raw = ((buf[0] << 8) | buf[1]) >> 4;

	/* Convert to 0.01°C units (TMP102 resolution is 0.0625°C) */
	*temp_raw = (*temp_raw * 625) / 100;

	return 0;
#endif
}

/* Configure sensor */
static int configure_sensor(void)
{
#if SIMULATION_MODE
	LOG_INF("Running in simulation mode (no I2C hardware)");
	return 0;
#else
	uint8_t config[3];
	int ret;

	if (!device_is_ready(i2c_dev)) {
		LOG_ERR("I2C device not ready");
		return -ENODEV;
	}

	LOG_INF("I2C device: %s", i2c_dev->name);

	/* Configure TMP102: continuous conversion, 12-bit resolution */
	config[0] = CONFIG_REG;
	config[1] = 0x60;  /* 12-bit, continuous */
	config[2] = 0xA0;

	ret = i2c_write(i2c_dev, config, 3, TMP102_ADDR);
	if (ret < 0) {
		LOG_WRN("Sensor not found at 0x%02x (using simulation)", TMP102_ADDR);
		return ret;
	}

	LOG_INF("Sensor configured at address 0x%02x", TMP102_ADDR);
	return 0;
#endif
}

/* I2C bus scan */
static void scan_i2c_bus(void)
{
#if !SIMULATION_MODE
	uint8_t dummy;

	LOG_INF("Scanning I2C bus...");

	for (uint8_t addr = 0x08; addr < 0x78; addr++) {
		int ret = i2c_read(i2c_dev, &dummy, 1, addr);
		if (ret == 0) {
			LOG_INF("  Device found at 0x%02x", addr);
		}
	}

	LOG_INF("Scan complete");
#else
	LOG_INF("I2C scan skipped (simulation mode)");
#endif
}

int main(void)
{
	int16_t temperature;
	int ret;

	printk("I2C Sensor Example\n");
	printk("==================\n\n");

	/* Configure sensor */
	ret = configure_sensor();
	if (ret < 0 && !SIMULATION_MODE) {
		LOG_WRN("Continuing with simulated readings");
	}

	/* Scan bus for devices */
	scan_i2c_bus();

	printk("\nReading temperature every 2 seconds:\n");

	/* Main loop - read and display temperature */
	while (1) {
		ret = read_temperature(&temperature);
		if (ret == 0) {
			int whole = temperature / 100;
			int frac = abs(temperature % 100);
			LOG_INF("Temperature: %d.%02d C", whole, frac);
		} else {
			LOG_ERR("Read failed: %d", ret);
		}

		k_sleep(K_SECONDS(2));
	}

	return 0;
}
