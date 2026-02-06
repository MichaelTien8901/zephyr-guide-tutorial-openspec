/*
 * Message Queue Example
 *
 * Demonstrates fixed-size message passing between threads.
 */

#include <zephyr/kernel.h>
#include <zephyr/random/random.h>

#define STACK_SIZE 1024

K_THREAD_STACK_DEFINE(producer_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(consumer_stack, STACK_SIZE);

static struct k_thread producer_thread;
static struct k_thread consumer_thread;

/* Message structure */
struct sensor_msg {
	uint32_t timestamp;
	int16_t temperature;
	int16_t humidity;
};

/* Define message queue: message size, max count, alignment */
K_MSGQ_DEFINE(sensor_msgq, sizeof(struct sensor_msg), 10, 4);

/* Simulated sensor reading using hardware RNG */
static int16_t read_temperature(void)
{
	static int16_t temp = 200;  /* Start at 20.0°C */
	temp += (sys_rand32_get() % 10) - 5;  /* Random variation */
	return temp;
}

static int16_t read_humidity(void)
{
	static int16_t humidity = 500;  /* Start at 50.0% */
	humidity += (sys_rand32_get() % 20) - 10;
	return humidity;
}

void producer_entry(void *p1, void *p2, void *p3)
{
	struct sensor_msg msg;

	for (int i = 0; i < 20; i++) {
		/* Read sensors */
		msg.timestamp = k_uptime_get_32();
		msg.temperature = read_temperature();
		msg.humidity = read_humidity();

		/* Send message */
		int ret = k_msgq_put(&sensor_msgq, &msg, K_NO_WAIT);
		if (ret == 0) {
			printk("[Producer] Sent: temp=%d.%d°C, hum=%d.%d%% @ %u\n",
			       msg.temperature / 10, msg.temperature % 10,
			       msg.humidity / 10, msg.humidity % 10,
			       msg.timestamp);
		} else {
			printk("[Producer] Queue full, dropping message\n");
		}

		k_msleep(200);
	}

	printk("[Producer] Done\n");
}

void consumer_entry(void *p1, void *p2, void *p3)
{
	struct sensor_msg msg;

	while (1) {
		/* Wait for message with timeout */
		int ret = k_msgq_get(&sensor_msgq, &msg, K_MSEC(1000));

		if (ret == 0) {
			printk("[Consumer] Received: temp=%d.%d°C, hum=%d.%d%% @ %u\n",
			       msg.temperature / 10, msg.temperature % 10,
			       msg.humidity / 10, msg.humidity % 10,
			       msg.timestamp);

			/* Simulate processing time */
			k_msleep(300);
		} else if (ret == -EAGAIN) {
			printk("[Consumer] Timeout waiting for message\n");

			/* Check if producer is done */
			if (k_msgq_num_used_get(&sensor_msgq) == 0) {
				break;
			}
		}
	}

	printk("[Consumer] Done\n");
}

int main(void)
{
	printk("Message Queue Example\n");
	printk("Queue capacity: %u messages\n", k_msgq_num_free_get(&sensor_msgq));

	k_thread_create(&producer_thread, producer_stack, STACK_SIZE,
			producer_entry, NULL, NULL, NULL,
			5, 0, K_NO_WAIT);
	k_thread_name_set(&producer_thread, "producer");

	k_thread_create(&consumer_thread, consumer_stack, STACK_SIZE,
			consumer_entry, NULL, NULL, NULL,
			6, 0, K_NO_WAIT);
	k_thread_name_set(&consumer_thread, "consumer");

	k_thread_join(&producer_thread, K_FOREVER);
	k_thread_join(&consumer_thread, K_FOREVER);

	printk("Example complete\n");

	return 0;
}
