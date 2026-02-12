/*
 * Logging Example
 *
 * Demonstrates the Zephyr logging subsystem with multiple modules,
 * log levels, and data logging.
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

/* Register this file as a log module */
LOG_MODULE_REGISTER(main, LOG_LEVEL_DBG);

/* Simulate a "sensor" module with its own log level */
#define SENSOR_MODULE_NAME sensor
LOG_MODULE_DECLARE(main);

/* Simulated sensor reading */
static int read_sensor(int channel)
{
	static int values[] = {2350, 2400, 2380, 2410, 2395};
	static int idx;

	int val = values[idx % ARRAY_SIZE(values)];
	idx++;

	LOG_DBG("Reading sensor channel %d", channel);

	if (channel < 0 || channel > 3) {
		LOG_ERR("Invalid sensor channel: %d", channel);
		return -EINVAL;
	}

	return val + (channel * 100);
}

/* Process and validate sensor data */
static void process_data(int value)
{
	if (value < 2000) {
		LOG_WRN("Sensor value below threshold: %d", value);
	} else if (value > 3000) {
		LOG_WRN("Sensor value above threshold: %d", value);
	} else {
		LOG_INF("Sensor value OK: %d.%02d C",
			value / 100, value % 100);
	}
}

/* Demonstrate hexdump logging */
static void log_raw_data(void)
{
	uint8_t raw_data[] = {0xDE, 0xAD, 0xBE, 0xEF, 0x01, 0x02, 0x03, 0x04,
			      0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80};

	LOG_INF("Raw sensor register dump:");
	LOG_HEXDUMP_DBG(raw_data, sizeof(raw_data), "registers");
}

int main(void)
{
	int cycle = 0;

	printk("\n");
	printk("Logging Example\n");
	printk("===============\n");
	printk("This example demonstrates LOG_ERR, LOG_WRN, LOG_INF, LOG_DBG\n\n");

	LOG_INF("Application started on %s", CONFIG_BOARD);
	LOG_INF("Log level: %d (0=NONE, 1=ERR, 2=WRN, 3=INF, 4=DBG)",
		LOG_LEVEL_DBG);

	while (1) {
		LOG_INF("--- Cycle %d ---", cycle);

		/* Read from valid channels */
		for (int ch = 0; ch < 3; ch++) {
			int val = read_sensor(ch);
			if (val >= 0) {
				process_data(val);
			}
		}

		/* Trigger an error log every 3rd cycle */
		if (cycle % 3 == 2) {
			int val = read_sensor(-1);
			if (val < 0) {
				LOG_ERR("Sensor read failed (err %d)", val);
			}
		}

		/* Show hexdump every 5th cycle */
		if (cycle % 5 == 0) {
			log_raw_data();
		}

		cycle++;
		k_msleep(2000);
	}

	return 0;
}
