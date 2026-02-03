/*
 * BLE Peripheral Example
 *
 * Demonstrates a BLE GATT server with:
 * - Custom sensor service (temperature + humidity)
 * - Notifications
 * - Read/write characteristics
 */

#include <zephyr/kernel.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/services/bas.h>

/* Custom UUIDs for sensor service */
#define BT_UUID_SENSOR_SERVICE_VAL \
	BT_UUID_128_ENCODE(0x12345678, 0x1234, 0x5678, 0x1234, 0x56789abcdef0)
#define BT_UUID_SENSOR_TEMP_VAL \
	BT_UUID_128_ENCODE(0x12345678, 0x1234, 0x5678, 0x1234, 0x56789abcdef1)
#define BT_UUID_SENSOR_HUMID_VAL \
	BT_UUID_128_ENCODE(0x12345678, 0x1234, 0x5678, 0x1234, 0x56789abcdef2)
#define BT_UUID_SENSOR_LED_VAL \
	BT_UUID_128_ENCODE(0x12345678, 0x1234, 0x5678, 0x1234, 0x56789abcdef3)

#define BT_UUID_SENSOR_SERVICE  BT_UUID_DECLARE_128(BT_UUID_SENSOR_SERVICE_VAL)
#define BT_UUID_SENSOR_TEMP     BT_UUID_DECLARE_128(BT_UUID_SENSOR_TEMP_VAL)
#define BT_UUID_SENSOR_HUMID    BT_UUID_DECLARE_128(BT_UUID_SENSOR_HUMID_VAL)
#define BT_UUID_SENSOR_LED      BT_UUID_DECLARE_128(BT_UUID_SENSOR_LED_VAL)

/* Sensor values */
static int16_t temperature = 2500;  /* 25.00°C in 0.01°C units */
static int16_t humidity = 4500;     /* 45.00% in 0.01% units */
static uint8_t led_state;

/* Connection reference */
static struct bt_conn *current_conn;

/* Client configuration for notifications */
static bool temp_notify_enabled;
static bool humid_notify_enabled;

/* Advertising data */
static const struct bt_data ad[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
	BT_DATA_BYTES(BT_DATA_UUID128_ALL, BT_UUID_SENSOR_SERVICE_VAL),
};

/* Scan response data */
static const struct bt_data sd[] = {
	BT_DATA(BT_DATA_NAME_COMPLETE, CONFIG_BT_DEVICE_NAME, sizeof(CONFIG_BT_DEVICE_NAME) - 1),
};

/* Temperature read callback */
static ssize_t read_temperature(struct bt_conn *conn,
				const struct bt_gatt_attr *attr,
				void *buf, uint16_t len, uint16_t offset)
{
	return bt_gatt_attr_read(conn, attr, buf, len, offset,
				 &temperature, sizeof(temperature));
}

/* Humidity read callback */
static ssize_t read_humidity(struct bt_conn *conn,
			     const struct bt_gatt_attr *attr,
			     void *buf, uint16_t len, uint16_t offset)
{
	return bt_gatt_attr_read(conn, attr, buf, len, offset,
				 &humidity, sizeof(humidity));
}

/* LED write callback */
static ssize_t write_led(struct bt_conn *conn,
			 const struct bt_gatt_attr *attr,
			 const void *buf, uint16_t len, uint16_t offset,
			 uint8_t flags)
{
	if (offset + len > sizeof(led_state)) {
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
	}

	memcpy(&led_state + offset, buf, len);
	printk("[BLE] LED state changed to: %s\n", led_state ? "ON" : "OFF");

	/* Actually control LED here */
	/* gpio_pin_set_dt(&led, led_state); */

	return len;
}

/* LED read callback */
static ssize_t read_led(struct bt_conn *conn,
			const struct bt_gatt_attr *attr,
			void *buf, uint16_t len, uint16_t offset)
{
	return bt_gatt_attr_read(conn, attr, buf, len, offset,
				 &led_state, sizeof(led_state));
}

/* CCC changed callbacks */
static void temp_ccc_changed(const struct bt_gatt_attr *attr, uint16_t value)
{
	temp_notify_enabled = (value == BT_GATT_CCC_NOTIFY);
	printk("[BLE] Temperature notifications %s\n",
	       temp_notify_enabled ? "enabled" : "disabled");
}

static void humid_ccc_changed(const struct bt_gatt_attr *attr, uint16_t value)
{
	humid_notify_enabled = (value == BT_GATT_CCC_NOTIFY);
	printk("[BLE] Humidity notifications %s\n",
	       humid_notify_enabled ? "enabled" : "disabled");
}

/* GATT service definition */
BT_GATT_SERVICE_DEFINE(sensor_svc,
	BT_GATT_PRIMARY_SERVICE(BT_UUID_SENSOR_SERVICE),

	/* Temperature characteristic */
	BT_GATT_CHARACTERISTIC(BT_UUID_SENSOR_TEMP,
			       BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY,
			       BT_GATT_PERM_READ,
			       read_temperature, NULL, &temperature),
	BT_GATT_CCC(temp_ccc_changed, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),

	/* Humidity characteristic */
	BT_GATT_CHARACTERISTIC(BT_UUID_SENSOR_HUMID,
			       BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY,
			       BT_GATT_PERM_READ,
			       read_humidity, NULL, &humidity),
	BT_GATT_CCC(humid_ccc_changed, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),

	/* LED control characteristic */
	BT_GATT_CHARACTERISTIC(BT_UUID_SENSOR_LED,
			       BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE,
			       BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
			       read_led, write_led, &led_state),
);

/* Connection callbacks */
static void connected(struct bt_conn *conn, uint8_t err)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	if (err) {
		printk("[BLE] Connection failed: %s (err %d)\n", addr, err);
		return;
	}

	printk("[BLE] Connected: %s\n", addr);
	current_conn = bt_conn_ref(conn);
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
	printk("[BLE] Disconnected: %s (reason %d)\n", addr, reason);

	if (current_conn) {
		bt_conn_unref(current_conn);
		current_conn = NULL;
	}

	temp_notify_enabled = false;
	humid_notify_enabled = false;
}

BT_CONN_CB_DEFINE(conn_callbacks) = {
	.connected = connected,
	.disconnected = disconnected,
};

/* Start advertising */
static int start_advertising(void)
{
	int err;

	err = bt_le_adv_start(BT_LE_ADV_CONN, ad, ARRAY_SIZE(ad),
			      sd, ARRAY_SIZE(sd));
	if (err) {
		printk("[BLE] Advertising failed to start (err %d)\n", err);
		return err;
	}

	printk("[BLE] Advertising started\n");
	return 0;
}

/* Send temperature notification */
static void notify_temperature(void)
{
	if (!current_conn || !temp_notify_enabled) {
		return;
	}

	int err = bt_gatt_notify(current_conn, &sensor_svc.attrs[1],
				 &temperature, sizeof(temperature));
	if (err) {
		printk("[BLE] Temperature notify failed (err %d)\n", err);
	}
}

/* Send humidity notification */
static void notify_humidity(void)
{
	if (!current_conn || !humid_notify_enabled) {
		return;
	}

	int err = bt_gatt_notify(current_conn, &sensor_svc.attrs[4],
				 &humidity, sizeof(humidity));
	if (err) {
		printk("[BLE] Humidity notify failed (err %d)\n", err);
	}
}

/* Update simulated sensor values */
static void update_sensors(void)
{
	temperature += (sys_rand32_get() % 100) - 50;
	humidity += (sys_rand32_get() % 200) - 100;

	/* Clamp values */
	if (temperature < 0) temperature = 0;
	if (temperature > 5000) temperature = 5000;
	if (humidity < 0) humidity = 0;
	if (humidity > 10000) humidity = 10000;
}

int main(void)
{
	int err;

	printk("\n");
	printk("BLE Peripheral Example\n");
	printk("======================\n\n");

	/* Initialize Bluetooth */
	err = bt_enable(NULL);
	if (err) {
		printk("[BLE] Bluetooth init failed (err %d)\n", err);
		return err;
	}

	printk("[BLE] Bluetooth initialized\n");

	/* Start advertising */
	err = start_advertising();
	if (err) {
		return err;
	}

	printk("[BLE] Waiting for connections...\n");
	printk("[BLE] Device name: %s\n\n", CONFIG_BT_DEVICE_NAME);

	/* Main loop - update sensors and send notifications */
	while (1) {
		k_sleep(K_SECONDS(2));

		/* Update sensor values */
		update_sensors();

		printk("Sensors: Temp=%d.%02d C, Humid=%d.%02d%%\n",
		       temperature / 100, abs(temperature % 100),
		       humidity / 100, abs(humidity % 100));

		/* Send notifications if enabled */
		notify_temperature();
		notify_humidity();

		/* Update battery level (for demo) */
		static uint8_t battery = 100;
		battery = (battery > 0) ? battery - 1 : 100;
		bt_bas_set_battery_level(battery);
	}

	return 0;
}
