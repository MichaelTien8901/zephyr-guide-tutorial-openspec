/*
 * Mutex Example
 *
 * Demonstrates mutex for protecting shared resources.
 */

#include <zephyr/kernel.h>

#define STACK_SIZE 1024

K_THREAD_STACK_DEFINE(thread1_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(thread2_stack, STACK_SIZE);

static struct k_thread thread1_data;
static struct k_thread thread2_data;

/* Shared resource protected by mutex */
K_MUTEX_DEFINE(counter_mutex);
static int shared_counter = 0;

/* Thread that increments counter */
void increment_thread(void *p1, void *p2, void *p3)
{
	const char *name = k_thread_name_get(k_current_get());

	for (int i = 0; i < 10; i++) {
		/* Lock mutex before accessing shared resource */
		k_mutex_lock(&counter_mutex, K_FOREVER);

		/* Critical section - safe to access shared_counter */
		int local = shared_counter;
		k_msleep(10);  /* Simulate some work */
		shared_counter = local + 1;

		printk("[%s] Counter: %d -> %d\n", name, local, shared_counter);

		/* Unlock mutex */
		k_mutex_unlock(&counter_mutex);

		/* Small delay between iterations */
		k_msleep(50);
	}

	printk("[%s] Done\n", name);
}

int main(void)
{
	k_tid_t tid1, tid2;

	printk("Mutex Example\n");
	printk("Initial counter: %d\n", shared_counter);

	/* Create two threads that both increment the counter */
	tid1 = k_thread_create(&thread1_data, thread1_stack, STACK_SIZE,
			       increment_thread, NULL, NULL, NULL,
			       5, 0, K_NO_WAIT);
	k_thread_name_set(tid1, "inc1");

	tid2 = k_thread_create(&thread2_data, thread2_stack, STACK_SIZE,
			       increment_thread, NULL, NULL, NULL,
			       5, 0, K_NO_WAIT);
	k_thread_name_set(tid2, "inc2");

	/* Wait for threads to complete */
	k_thread_join(&thread1_data, K_FOREVER);
	k_thread_join(&thread2_data, K_FOREVER);

	printk("Final counter: %d (expected: 20)\n", shared_counter);

	return 0;
}
