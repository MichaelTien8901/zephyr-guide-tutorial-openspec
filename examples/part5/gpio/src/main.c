/*
 * GPIO Example
 *
 * Demonstrates GPIO input, output, and interrupt handling.
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

/* Get LED and button from devicetree */
#define LED0_NODE DT_ALIAS(led0)
#define SW0_NODE  DT_ALIAS(sw0)

#if !DT_NODE_HAS_STATUS(LED0_NODE, okay)
#error "LED0 not defined"
#endif

#if !DT_NODE_HAS_STATUS(SW0_NODE, okay)
#error "SW0 not defined"
#endif

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(SW0_NODE, gpios);

static struct gpio_callback button_cb_data;
static struct k_work button_work;

/* Work handler for button processing */
void button_work_handler(struct k_work *work)
{
	static int press_count = 0;

	press_count++;
	printk("Button pressed! Count: %d\n", press_count);

	/* Toggle LED */
	gpio_pin_toggle_dt(&led);
}

/* Button interrupt callback */
void button_pressed(const struct device *dev, struct gpio_callback *cb,
		    uint32_t pins)
{
	/* Submit work to handle button press (don't do heavy work in ISR) */
	k_work_submit(&button_work);
}

int main(void)
{
	int ret;

	printk("GPIO Example\n");

	/* Check device readiness */
	if (!gpio_is_ready_dt(&led)) {
		printk("LED device not ready\n");
		return -1;
	}

	if (!gpio_is_ready_dt(&button)) {
		printk("Button device not ready\n");
		return -1;
	}

	/* Configure LED as output */
	ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		printk("Failed to configure LED: %d\n", ret);
		return -1;
	}

	/* Configure button as input with interrupt */
	ret = gpio_pin_configure_dt(&button, GPIO_INPUT);
	if (ret < 0) {
		printk("Failed to configure button: %d\n", ret);
		return -1;
	}

	ret = gpio_pin_interrupt_configure_dt(&button, GPIO_INT_EDGE_TO_ACTIVE);
	if (ret < 0) {
		printk("Failed to configure button interrupt: %d\n", ret);
		return -1;
	}

	/* Initialize work item */
	k_work_init(&button_work, button_work_handler);

	/* Set up button callback */
	gpio_init_callback(&button_cb_data, button_pressed, BIT(button.pin));
	gpio_add_callback(button.port, &button_cb_data);

	printk("Press the button to toggle LED\n");
	printk("LED pin: %s %d\n", led.port->name, led.pin);
	printk("Button pin: %s %d\n", button.port->name, button.pin);

	/* Initial LED state */
	gpio_pin_set_dt(&led, 0);

	/* Main loop - blink LED slowly when idle */
	while (1) {
		k_sleep(K_SECONDS(5));
		printk("Still running... (press button to interact)\n");
	}

	return 0;
}
