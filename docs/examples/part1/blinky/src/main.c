/*
 * Blinky Example
 *
 * Demonstrates GPIO output by blinking an LED.
 * Uses devicetree aliases for board-portable code.
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

/* Get LED from devicetree alias */
#define LED0_NODE DT_ALIAS(led0)

#if !DT_NODE_HAS_STATUS(LED0_NODE, okay)
#error "LED0 not defined in devicetree"
#endif

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

int main(void)
{
	int ret;

	if (!gpio_is_ready_dt(&led)) {
		printk("LED device not ready\n");
		return -1;
	}

	ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		printk("Failed to configure LED: %d\n", ret);
		return -1;
	}

	printk("Blinking LED on %s pin %d\n", led.port->name, led.pin);

	while (1) {
		gpio_pin_toggle_dt(&led);
		k_msleep(500);
	}

	return 0;
}
