/*
 * MQTT Example
 *
 * Demonstrates MQTT publish and subscribe operations.
 * Publishes sensor data and subscribes to control topics.
 */

#include <zephyr/kernel.h>
#include <zephyr/net/socket.h>
#include <zephyr/net/mqtt.h>
#include <zephyr/random/random.h>
#include <string.h>
#include <errno.h>

/* MQTT broker configuration */
#define MQTT_BROKER_ADDR    "192.168.1.100"
#define MQTT_BROKER_PORT    1883
#define MQTT_CLIENT_ID      "zephyr_client"

/* Topics */
#define TOPIC_SENSOR       "sensors/temperature"
#define TOPIC_CONTROL      "control/led"

/* Buffers */
#define MQTT_RX_BUF_SIZE   256
#define MQTT_TX_BUF_SIZE   256

static uint8_t rx_buffer[MQTT_RX_BUF_SIZE];
static uint8_t tx_buffer[MQTT_TX_BUF_SIZE];

/* MQTT client */
static struct mqtt_client client;
static struct sockaddr_storage broker;

/* Connection state */
static bool connected;
static K_SEM_DEFINE(mqtt_sem, 0, 1);

/* Received message handler */
static void handle_message(const struct mqtt_publish_param *pub)
{
	uint8_t payload[64];
	int len;

	len = pub->message.payload.len;
	if (len > sizeof(payload) - 1) {
		len = sizeof(payload) - 1;
	}

	/* Read payload - in real code, handle fragmented messages */
	memcpy(payload, pub->message.payload.data, len);
	payload[len] = '\0';

	printk("[MQTT] Received on '%.*s': %s\n",
	       pub->message.topic.topic.size,
	       (char *)pub->message.topic.topic.utf8,
	       payload);

	/* Handle control messages */
	if (strncmp((char *)pub->message.topic.topic.utf8,
		    TOPIC_CONTROL, pub->message.topic.topic.size) == 0) {
		if (strcmp((char *)payload, "on") == 0) {
			printk("[MQTT] LED control: ON\n");
			/* gpio_pin_set_dt(&led, 1); */
		} else if (strcmp((char *)payload, "off") == 0) {
			printk("[MQTT] LED control: OFF\n");
			/* gpio_pin_set_dt(&led, 0); */
		}
	}
}

/* MQTT event handler */
static void mqtt_evt_handler(struct mqtt_client *c,
			     const struct mqtt_evt *evt)
{
	switch (evt->type) {
	case MQTT_EVT_CONNACK:
		if (evt->result == 0) {
			printk("[MQTT] Connected to broker\n");
			connected = true;
			k_sem_give(&mqtt_sem);
		} else {
			printk("[MQTT] Connection failed: %d\n", evt->result);
		}
		break;

	case MQTT_EVT_DISCONNECT:
		printk("[MQTT] Disconnected\n");
		connected = false;
		break;

	case MQTT_EVT_PUBLISH:
		handle_message(&evt->param.publish);

		/* Send PUBACK for QoS 1 */
		if (evt->param.publish.message.topic.qos == MQTT_QOS_1_AT_LEAST_ONCE) {
			struct mqtt_puback_param puback = {
				.message_id = evt->param.publish.message_id
			};
			mqtt_publish_qos1_ack(c, &puback);
		}
		break;

	case MQTT_EVT_PUBACK:
		printk("[MQTT] PUBACK received, msg_id: %d\n",
		       evt->param.puback.message_id);
		break;

	case MQTT_EVT_SUBACK:
		printk("[MQTT] SUBACK received, msg_id: %d\n",
		       evt->param.suback.message_id);
		break;

	case MQTT_EVT_PINGRESP:
		printk("[MQTT] PINGRESP\n");
		break;

	default:
		printk("[MQTT] Event: %d\n", evt->type);
		break;
	}
}

/* Configure MQTT client */
static int app_mqtt_init(void)
{
	struct sockaddr_in *broker4 = (struct sockaddr_in *)&broker;

	/* Set broker address */
	broker4->sin_family = AF_INET;
	broker4->sin_port = htons(MQTT_BROKER_PORT);
	inet_pton(AF_INET, MQTT_BROKER_ADDR, &broker4->sin_addr);

	/* Initialize client using Zephyr MQTT API */
	mqtt_client_init(&client);

	/* Client configuration */
	client.broker = &broker;
	client.evt_cb = mqtt_evt_handler;
	client.client_id.utf8 = (uint8_t *)MQTT_CLIENT_ID;
	client.client_id.size = strlen(MQTT_CLIENT_ID);
	client.protocol_version = MQTT_VERSION_3_1_1;

	/* Buffers */
	client.rx_buf = rx_buffer;
	client.rx_buf_size = sizeof(rx_buffer);
	client.tx_buf = tx_buffer;
	client.tx_buf_size = sizeof(tx_buffer);

	/* Transport */
	client.transport.type = MQTT_TRANSPORT_NON_SECURE;

	return 0;
}

/* Connect to MQTT broker */
static int app_mqtt_connect(void)
{
	int ret;

	/* Call Zephyr MQTT connect API */
	ret = mqtt_connect(&client);
	if (ret != 0) {
		printk("[MQTT] Connect failed: %d\n", ret);
		return ret;
	}

	printk("[MQTT] Connecting to %s:%d...\n",
	       MQTT_BROKER_ADDR, MQTT_BROKER_PORT);

	/* Wait for CONNACK */
	k_sem_take(&mqtt_sem, K_SECONDS(10));

	if (!connected) {
		printk("[MQTT] Connection timeout\n");
		return -ETIMEDOUT;
	}

	return 0;
}

/* Subscribe to control topic */
static int mqtt_subscribe_control(void)
{
	struct mqtt_topic topic = {
		.topic.utf8 = TOPIC_CONTROL,
		.topic.size = strlen(TOPIC_CONTROL),
		.qos = MQTT_QOS_1_AT_LEAST_ONCE
	};
	struct mqtt_subscription_list sub = {
		.list = &topic,
		.list_count = 1,
		.message_id = sys_rand32_get() & 0xFFFF
	};

	printk("[MQTT] Subscribing to '%s'...\n", TOPIC_CONTROL);

	return mqtt_subscribe(&client, &sub);
}

/* Publish sensor data */
static int mqtt_publish_sensor(int32_t temperature)
{
	char payload[32];
	int len;

	len = snprintf(payload, sizeof(payload), "{\"temp\":%d.%02d}",
		       temperature / 100, abs(temperature % 100));

	struct mqtt_publish_param param = {
		.message.topic.qos = MQTT_QOS_1_AT_LEAST_ONCE,
		.message.topic.topic.utf8 = TOPIC_SENSOR,
		.message.topic.topic.size = strlen(TOPIC_SENSOR),
		.message.payload.data = payload,
		.message.payload.len = len,
		.message_id = sys_rand32_get() & 0xFFFF,
		.dup_flag = 0,
		.retain_flag = 0
	};

	printk("[MQTT] Publishing to '%s': %s\n", TOPIC_SENSOR, payload);

	return mqtt_publish(&client, &param);
}

/* MQTT input processing thread */
static void mqtt_input_thread(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1);
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

	struct pollfd fds[1];

	while (1) {
		if (!connected) {
			k_sleep(K_SECONDS(1));
			continue;
		}

		fds[0].fd = client.transport.tcp.sock;
		fds[0].events = POLLIN;

		int ret = poll(fds, 1, 1000);
		if (ret > 0 && (fds[0].revents & POLLIN)) {
			mqtt_input(&client);
		}

		/* Send keepalive */
		mqtt_live(&client);
	}
}

K_THREAD_DEFINE(mqtt_tid, 2048, mqtt_input_thread, NULL, NULL, NULL, 7, 0, 0);

/* Simulate temperature reading */
static int32_t read_temperature(void)
{
	static int32_t temp = 2500;  /* 25.00°C */
	temp += (sys_rand32_get() % 100) - 50;
	return temp;
}

int main(void)
{
	int ret;

	printk("\n");
	printk("MQTT Example\n");
	printk("============\n\n");

	/* Wait for network */
	k_sleep(K_SECONDS(5));

	/* Initialize MQTT client */
	app_mqtt_init();

	/* Connect to broker */
	ret = app_mqtt_connect();
	if (ret != 0) {
		printk("Failed to connect to MQTT broker\n");
		return ret;
	}

	/* Subscribe to control topic */
	mqtt_subscribe_control();

	/* Publish sensor data periodically */
	while (1) {
		if (connected) {
			int32_t temp = read_temperature();
			mqtt_publish_sensor(temp);
		}

		k_sleep(K_SECONDS(10));
	}

	return 0;
}
