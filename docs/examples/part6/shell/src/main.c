/*
 * Shell Example
 *
 * Demonstrates custom shell commands with subcommands,
 * arguments, and dynamic data access.
 */

#include <zephyr/kernel.h>
#include <zephyr/shell/shell.h>
#include <stdlib.h>

/* Application state accessible from shell */
static int led_state;
static int sensor_interval_ms = 1000;
static int sensor_reading;

/* ---- LED commands ---- */

static int cmd_led_on(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	led_state = 1;
	/* gpio_pin_set_dt(&led, 1); */
	shell_print(sh, "LED turned ON");
	return 0;
}

static int cmd_led_off(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	led_state = 0;
	/* gpio_pin_set_dt(&led, 0); */
	shell_print(sh, "LED turned OFF");
	return 0;
}

static int cmd_led_status(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	shell_print(sh, "LED is %s", led_state ? "ON" : "OFF");
	return 0;
}

/* Register "led" command with subcommands */
SHELL_STATIC_SUBCMD_SET_CREATE(led_cmds,
	SHELL_CMD(on, NULL, "Turn LED on", cmd_led_on),
	SHELL_CMD(off, NULL, "Turn LED off", cmd_led_off),
	SHELL_CMD(status, NULL, "Show LED state", cmd_led_status),
	SHELL_SUBCMD_SET_END
);
SHELL_CMD_REGISTER(led, &led_cmds, "LED control commands", NULL);

/* ---- Sensor commands ---- */

static int cmd_sensor_read(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	shell_print(sh, "Temperature: %d.%02d C",
		    sensor_reading / 100, sensor_reading % 100);
	return 0;
}

static int cmd_sensor_interval(const struct shell *sh, size_t argc, char **argv)
{
	if (argc < 2) {
		shell_print(sh, "Current interval: %d ms", sensor_interval_ms);
		return 0;
	}

	int val = atoi(argv[1]);
	if (val < 100 || val > 60000) {
		shell_error(sh, "Interval must be 100-60000 ms");
		return -EINVAL;
	}

	sensor_interval_ms = val;
	shell_print(sh, "Sensor interval set to %d ms", sensor_interval_ms);
	return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sensor_cmds,
	SHELL_CMD(read, NULL, "Read current sensor value", cmd_sensor_read),
	SHELL_CMD_ARG(interval, NULL, "Get/set interval [ms]",
		      cmd_sensor_interval, 1, 1),
	SHELL_SUBCMD_SET_END
);
SHELL_CMD_REGISTER(sensor, &sensor_cmds, "Sensor commands", NULL);

/* ---- Simple top-level command ---- */

static int cmd_uptime(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	int64_t ms = k_uptime_get();
	int sec = (int)(ms / 1000);
	int min = sec / 60;

	shell_print(sh, "Uptime: %dm %ds (%lld ms)", min, sec % 60, ms);
	return 0;
}

SHELL_CMD_REGISTER(uptime, NULL, "Show system uptime", cmd_uptime);

/* ---- Background sensor thread ---- */

static void sensor_thread(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1);
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

	sensor_reading = 2500;

	while (1) {
		/* Simulate sensor drift */
		sensor_reading += (k_cycle_get_32() % 11) - 5;

		k_msleep(sensor_interval_ms);
	}
}

K_THREAD_DEFINE(sensor_tid, 512, sensor_thread, NULL, NULL, NULL, 7, 0, 0);

int main(void)
{
	printk("\n");
	printk("Shell Example\n");
	printk("=============\n");
	printk("Type 'help' to see available commands.\n");
	printk("Try: led on, led status, sensor read, uptime\n\n");

	return 0;
}
