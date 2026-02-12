/*
 * Native Simulator Example
 *
 * Demonstrates a Zephyr application designed to run on native_sim,
 * useful for development and testing without hardware.
 *
 * Build: west build -b native_sim examples/part6/native-sim
 * Run:   ./build/zephyr/zephyr.exe
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

#define STACK_SIZE 1024
#define MAX_CYCLES 10

K_THREAD_STACK_DEFINE(worker_stack, STACK_SIZE);
static struct k_thread worker_data;

/* Shared state */
static K_MUTEX_DEFINE(state_mutex);
static int event_count;

/* Simulated sensor that works on native_sim */
static int simulate_sensor_read(void)
{
	/* Use uptime as a pseudo-random source for simulation */
	int base = 2200;
	int variation = (int)(k_uptime_get_32() % 400);
	return base + variation;
}

/* Worker thread: simulates periodic sensor collection */
static void worker_entry(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1);
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

	LOG_INF("Worker thread started");

	for (int i = 0; i < MAX_CYCLES; i++) {
		int reading = simulate_sensor_read();

		k_mutex_lock(&state_mutex, K_FOREVER);
		event_count++;
		k_mutex_unlock(&state_mutex);

		LOG_INF("Sensor reading %d: %d.%02d C",
			i + 1, reading / 100, reading % 100);

		if (reading > 2500) {
			LOG_WRN("High temperature alert: %d.%02d C",
				reading / 100, reading % 100);
		}

		k_msleep(500);
	}

	LOG_INF("Worker thread completed %d readings", MAX_CYCLES);
}

int main(void)
{
	printk("\n");
	printk("Native Simulator Example\n");
	printk("========================\n");
	printk("Board: %s\n\n", CONFIG_BOARD);

	LOG_INF("Application starting on native_sim");
	LOG_INF("This runs as a Linux process - no hardware needed!");

	/* Create worker thread */
	k_tid_t tid = k_thread_create(&worker_data, worker_stack, STACK_SIZE,
				      worker_entry, NULL, NULL, NULL,
				      5, 0, K_NO_WAIT);
	k_thread_name_set(tid, "worker");

	/* Wait for worker to finish */
	k_thread_join(&worker_data, K_FOREVER);

	k_mutex_lock(&state_mutex, K_FOREVER);
	LOG_INF("Total events processed: %d", event_count);
	k_mutex_unlock(&state_mutex);

	LOG_INF("Application complete - exiting");

	return 0;
}
