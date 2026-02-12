/*
 * Tracing Example
 *
 * Demonstrates a multi-threaded application with tracing enabled.
 * Build with CTF tracing to capture thread scheduling, ISR, and
 * synchronization events for analysis with tools like Babeltrace.
 *
 * Build: west build -b qemu_cortex_m3 examples/part6/tracing
 * Trace: west build -t run  (CTF data is output on the trace backend)
 */

#include <zephyr/kernel.h>

#define STACK_SIZE 1024

K_THREAD_STACK_DEFINE(producer_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(consumer_stack, STACK_SIZE);

static struct k_thread producer_data;
static struct k_thread consumer_data;

/* Synchronization objects (traced by CTF) */
static K_SEM_DEFINE(data_ready, 0, 1);
static K_MUTEX_DEFINE(data_mutex);

/* Shared data */
static int shared_value;

/* Producer: generates data and signals consumer */
static void producer_entry(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1);
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

	for (int i = 0; i < 20; i++) {
		/* Simulate work (this shows up in trace timeline) */
		k_busy_wait(1000);

		/* Update shared data under mutex */
		k_mutex_lock(&data_mutex, K_FOREVER);
		shared_value = i * 10;
		k_mutex_unlock(&data_mutex);

		/* Signal consumer */
		k_sem_give(&data_ready);

		printk("[Producer] Produced value: %d\n", i * 10);

		/* Variable delay to create interesting trace patterns */
		k_msleep(100 + (i % 3) * 50);
	}

	printk("[Producer] Done\n");
}

/* Consumer: waits for data and processes it */
static void consumer_entry(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1);
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

	for (int i = 0; i < 20; i++) {
		/* Wait for producer signal (traced as semaphore take) */
		k_sem_take(&data_ready, K_FOREVER);

		/* Read shared data under mutex */
		k_mutex_lock(&data_mutex, K_FOREVER);
		int val = shared_value;
		k_mutex_unlock(&data_mutex);

		/* Simulate processing */
		k_busy_wait(500);

		printk("[Consumer] Consumed value: %d\n", val);
	}

	printk("[Consumer] Done\n");
}

int main(void)
{
	k_tid_t tid1, tid2;

	printk("\n");
	printk("Tracing Example\n");
	printk("================\n");
	printk("Board: %s\n", CONFIG_BOARD);
	printk("CTF tracing is %s\n\n",
	       IS_ENABLED(CONFIG_TRACING_CTF) ? "ENABLED" : "disabled");

	/* Create producer (higher priority) */
	tid1 = k_thread_create(&producer_data, producer_stack, STACK_SIZE,
			       producer_entry, NULL, NULL, NULL,
			       5, 0, K_NO_WAIT);
	k_thread_name_set(tid1, "producer");

	/* Create consumer (lower priority) */
	tid2 = k_thread_create(&consumer_data, consumer_stack, STACK_SIZE,
			       consumer_entry, NULL, NULL, NULL,
			       6, 0, K_NO_WAIT);
	k_thread_name_set(tid2, "consumer");

	/* Wait for both to finish */
	k_thread_join(&producer_data, K_FOREVER);
	k_thread_join(&consumer_data, K_FOREVER);

	printk("\nTracing example complete.\n");
	printk("Analyze CTF output with: babeltrace <trace-dir>\n");

	return 0;
}
