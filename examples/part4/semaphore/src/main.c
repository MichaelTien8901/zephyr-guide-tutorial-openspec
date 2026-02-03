/*
 * Semaphore Example
 *
 * Demonstrates producer-consumer pattern with counting semaphore.
 */

#include <zephyr/kernel.h>

#define STACK_SIZE 1024
#define BUFFER_SIZE 5

K_THREAD_STACK_DEFINE(producer_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(consumer_stack, STACK_SIZE);

static struct k_thread producer_thread;
static struct k_thread consumer_thread;

/* Bounded buffer */
static int buffer[BUFFER_SIZE];
static int write_idx = 0;
static int read_idx = 0;

/* Semaphores for synchronization */
K_SEM_DEFINE(empty_slots, BUFFER_SIZE, BUFFER_SIZE);  /* Available slots */
K_SEM_DEFINE(full_slots, 0, BUFFER_SIZE);             /* Items in buffer */
K_MUTEX_DEFINE(buffer_mutex);                          /* Protect buffer access */

void producer_entry(void *p1, void *p2, void *p3)
{
	for (int i = 1; i <= 15; i++) {
		/* Wait for empty slot */
		k_sem_take(&empty_slots, K_FOREVER);

		/* Lock buffer */
		k_mutex_lock(&buffer_mutex, K_FOREVER);

		/* Produce item */
		buffer[write_idx] = i;
		printk("[Producer] Produced: %d at index %d\n", i, write_idx);
		write_idx = (write_idx + 1) % BUFFER_SIZE;

		k_mutex_unlock(&buffer_mutex);

		/* Signal that buffer has item */
		k_sem_give(&full_slots);

		/* Simulate production time */
		k_msleep(100);
	}

	printk("[Producer] Done producing\n");
}

void consumer_entry(void *p1, void *p2, void *p3)
{
	for (int i = 0; i < 15; i++) {
		/* Wait for item in buffer */
		k_sem_take(&full_slots, K_FOREVER);

		/* Lock buffer */
		k_mutex_lock(&buffer_mutex, K_FOREVER);

		/* Consume item */
		int item = buffer[read_idx];
		printk("[Consumer] Consumed: %d from index %d\n", item, read_idx);
		read_idx = (read_idx + 1) % BUFFER_SIZE;

		k_mutex_unlock(&buffer_mutex);

		/* Signal that slot is empty */
		k_sem_give(&empty_slots);

		/* Simulate consumption time (slower than production) */
		k_msleep(200);
	}

	printk("[Consumer] Done consuming\n");
}

int main(void)
{
	printk("Semaphore Producer-Consumer Example\n");
	printk("Buffer size: %d\n", BUFFER_SIZE);

	/* Create producer and consumer threads */
	k_thread_create(&producer_thread, producer_stack, STACK_SIZE,
			producer_entry, NULL, NULL, NULL,
			5, 0, K_NO_WAIT);
	k_thread_name_set(&producer_thread, "producer");

	k_thread_create(&consumer_thread, consumer_stack, STACK_SIZE,
			consumer_entry, NULL, NULL, NULL,
			6, 0, K_NO_WAIT);
	k_thread_name_set(&consumer_thread, "consumer");

	/* Wait for completion */
	k_thread_join(&producer_thread, K_FOREVER);
	k_thread_join(&consumer_thread, K_FOREVER);

	printk("Example complete\n");

	return 0;
}
