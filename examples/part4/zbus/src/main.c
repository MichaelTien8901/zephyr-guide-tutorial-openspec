/*
 * Zbus Example
 *
 * Demonstrates publish-subscribe messaging with Zbus.
 */

#include <zephyr/kernel.h>
#include <zephyr/zbus/zbus.h>

/* Message structure */
struct sensor_data {
	int32_t temperature;  /* milli-Celsius */
	int32_t humidity;     /* milli-percent */
	uint32_t timestamp;
};

/* Define channel */
ZBUS_CHAN_DEFINE(sensor_chan,
		 struct sensor_data,
		 NULL,  /* User data */
		 NULL,  /* Validator */
		 ZBUS_OBSERVERS_EMPTY,
		 ZBUS_MSG_INIT(.temperature = 0, .humidity = 0, .timestamp = 0));

/* Listener - called synchronously in publisher context */
void display_listener(const struct zbus_channel *chan)
{
	const struct sensor_data *msg = zbus_chan_const_msg(chan);

	printk("[Display] Temp: %d.%03d C, Humidity: %d.%01d%% @ %u\n",
	       msg->temperature / 1000, abs(msg->temperature % 1000),
	       msg->humidity / 1000, abs(msg->humidity % 100),
	       msg->timestamp);
}

ZBUS_LISTENER_DEFINE(display_lis, display_listener);

/* Subscriber - has own message queue */
ZBUS_SUBSCRIBER_DEFINE(logger_sub, 8);

/* Subscriber thread */
void logger_thread_entry(void *p1, void *p2, void *p3)
{
	const struct zbus_channel *chan;

	printk("[Logger] Thread started\n");

	while (1) {
		if (zbus_sub_wait(&logger_sub, &chan, K_FOREVER) == 0) {
			struct sensor_data msg;
			zbus_chan_read(chan, &msg, K_NO_WAIT);

			printk("[Logger] Logged: temp=%d, hum=%d, ts=%u\n",
			       msg.temperature, msg.humidity, msg.timestamp);
		}
	}
}

K_THREAD_DEFINE(logger_tid, 1024, logger_thread_entry, NULL, NULL, NULL, 7, 0, 0);

/* Add observers to channel */
ZBUS_CHAN_ADD_OBS(sensor_chan, display_lis, 0);
ZBUS_CHAN_ADD_OBS(sensor_chan, logger_sub, 1);

/* Simulated sensor reading */
static int32_t read_temperature(void)
{
	static int32_t temp = 22000;  /* 22.000°C */
	temp += (sys_rand32_get() % 1000) - 500;
	return temp;
}

static int32_t read_humidity(void)
{
	static int32_t hum = 45000;  /* 45.000% */
	hum += (sys_rand32_get() % 2000) - 1000;
	return hum;
}

int main(void)
{
	struct sensor_data msg;

	printk("Zbus Publish-Subscribe Example\n");

	/* Publish sensor data periodically */
	for (int i = 0; i < 10; i++) {
		msg.temperature = read_temperature();
		msg.humidity = read_humidity();
		msg.timestamp = k_uptime_get_32();

		printk("\n[Publisher] Publishing sensor data...\n");

		int ret = zbus_chan_pub(&sensor_chan, &msg, K_MSEC(100));
		if (ret != 0) {
			printk("[Publisher] Publish failed: %d\n", ret);
		}

		k_sleep(K_SECONDS(2));
	}

	printk("\nExample complete\n");

	return 0;
}
