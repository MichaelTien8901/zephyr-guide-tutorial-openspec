/*
 * Hello World Example
 *
 * This is the simplest Zephyr application demonstrating basic printk output.
 */

#include <zephyr/kernel.h>

int main(void)
{
	printk("Hello World! %s\n", CONFIG_BOARD);

	int count = 0;
	while (1) {
		printk("Counter: %d\n", count++);
		k_sleep(K_SECONDS(1));
	}

	return 0;
}
