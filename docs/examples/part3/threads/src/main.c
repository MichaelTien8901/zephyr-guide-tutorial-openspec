/*
 * Threads Example
 *
 * Demonstrates thread creation with different priorities.
 */

#include <zephyr/kernel.h>

#define STACK_SIZE 1024

/* Thread stacks */
K_THREAD_STACK_DEFINE(thread1_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(thread2_stack, STACK_SIZE);

static struct k_thread thread1_data;
static struct k_thread thread2_data;

/* Thread entry functions */
void thread1_entry(void *p1, void *p2, void *p3)
{
	int count = 0;

	while (1) {
		printk("[Thread 1] Count: %d (priority: %d)\n",
		       count++, k_thread_priority_get(k_current_get()));
		k_msleep(1000);
	}
}

void thread2_entry(void *p1, void *p2, void *p3)
{
	int count = 0;

	while (1) {
		printk("[Thread 2] Count: %d (priority: %d)\n",
		       count++, k_thread_priority_get(k_current_get()));
		k_msleep(1500);
	}
}

int main(void)
{
	k_tid_t tid1, tid2;

	printk("Threads Example\n");
	printk("Main thread priority: %d\n",
	       k_thread_priority_get(k_current_get()));

	/* Create thread 1 with priority 5 */
	tid1 = k_thread_create(&thread1_data, thread1_stack, STACK_SIZE,
			       thread1_entry, NULL, NULL, NULL,
			       5, 0, K_NO_WAIT);
	k_thread_name_set(tid1, "thread1");

	/* Create thread 2 with priority 7 (lower priority) */
	tid2 = k_thread_create(&thread2_data, thread2_stack, STACK_SIZE,
			       thread2_entry, NULL, NULL, NULL,
			       7, 0, K_NO_WAIT);
	k_thread_name_set(tid2, "thread2");

	printk("Threads created. Thread 1 (prio 5), Thread 2 (prio 7)\n");

	/* Main thread continues */
	while (1) {
		printk("[Main] Still running...\n");
		k_msleep(5000);
	}

	return 0;
}
