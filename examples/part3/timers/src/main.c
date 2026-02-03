/*
 * Timers Example
 *
 * Demonstrates periodic and one-shot kernel timers.
 */

#include <zephyr/kernel.h>

/* Timer expiry handlers */
void periodic_timer_handler(struct k_timer *timer)
{
	static int count = 0;
	printk("[Periodic] Timer fired! Count: %d\n", ++count);
}

void oneshot_timer_handler(struct k_timer *timer)
{
	printk("[One-shot] Timer fired!\n");
}

void timer_stop_handler(struct k_timer *timer)
{
	printk("[Stop] Timer stopped\n");
}

/* Define timers */
K_TIMER_DEFINE(periodic_timer, periodic_timer_handler, timer_stop_handler);
K_TIMER_DEFINE(oneshot_timer, oneshot_timer_handler, NULL);

int main(void)
{
	printk("Timers Example\n");

	/* Start periodic timer: 500ms initial delay, 1000ms period */
	printk("Starting periodic timer (1s period)\n");
	k_timer_start(&periodic_timer, K_MSEC(500), K_MSEC(1000));

	/* Start one-shot timer: fires once after 3 seconds */
	printk("Starting one-shot timer (3s delay)\n");
	k_timer_start(&oneshot_timer, K_SECONDS(3), K_NO_WAIT);

	/* Let timers run for a while */
	k_sleep(K_SECONDS(10));

	/* Check remaining time */
	printk("Periodic timer remaining: %u ms\n",
	       k_timer_remaining_get(&periodic_timer));

	/* Stop periodic timer */
	printk("Stopping periodic timer\n");
	k_timer_stop(&periodic_timer);

	/* Demonstrate synchronous wait */
	printk("Starting timer and waiting synchronously...\n");
	k_timer_start(&oneshot_timer, K_SECONDS(2), K_NO_WAIT);

	/* Block until timer expires */
	k_timer_status_sync(&oneshot_timer);
	printk("Synchronous wait complete\n");

	printk("Example complete\n");

	return 0;
}
