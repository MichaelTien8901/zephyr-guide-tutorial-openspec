/*
 * Workqueue Example
 *
 * Demonstrates k_work for deferred execution and k_work_delayable.
 */

#include <zephyr/kernel.h>

/* Work handlers */
void simple_work_handler(struct k_work *work)
{
	printk("[Work] Simple work executed in thread context\n");
	/* Safe to do blocking operations here */
	k_msleep(100);
	printk("[Work] Simple work complete\n");
}

void delayed_work_handler(struct k_work *work)
{
	printk("[Delayed] Delayed work executed\n");
}

struct sensor_work_ctx {
	struct k_work work;
	int sensor_value;
};

void sensor_work_handler(struct k_work *work)
{
	struct sensor_work_ctx *ctx = CONTAINER_OF(work, struct sensor_work_ctx, work);
	printk("[Sensor] Processing sensor value: %d\n", ctx->sensor_value);
}

/* Define work items */
K_WORK_DEFINE(simple_work, simple_work_handler);
K_WORK_DELAYABLE_DEFINE(delayed_work, delayed_work_handler);

static struct sensor_work_ctx sensor_ctx;

/* Simulate ISR that submits work */
void simulate_isr(void)
{
	printk("[ISR] Submitting work from ISR context\n");

	/* Submit immediate work */
	k_work_submit(&simple_work);

	/* Submit work with context */
	sensor_ctx.sensor_value = 42;
	k_work_submit(&sensor_ctx.work);
}

int main(void)
{
	printk("Workqueue Example\n");

	/* Initialize work with context */
	k_work_init(&sensor_ctx.work, sensor_work_handler);

	/* Submit simple work */
	printk("Submitting simple work\n");
	k_work_submit(&simple_work);

	/* Submit delayed work (runs after 2 seconds) */
	printk("Scheduling delayed work (2s)\n");
	k_work_schedule(&delayed_work, K_SECONDS(2));

	/* Wait a bit */
	k_msleep(500);

	/* Simulate ISR submitting work */
	simulate_isr();

	/* Cancel delayed work if still pending */
	k_msleep(1000);
	if (k_work_delayable_is_pending(&delayed_work)) {
		printk("Delayed work still pending, letting it run\n");
	}

	/* Wait for all work to complete */
	k_msleep(3000);

	/* Demonstrate work busy check */
	k_work_submit(&simple_work);
	if (k_work_is_pending(&simple_work)) {
		printk("Work is pending\n");
	}

	k_msleep(500);
	printk("Example complete\n");

	return 0;
}
