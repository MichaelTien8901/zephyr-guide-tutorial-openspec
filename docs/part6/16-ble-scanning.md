---
layout: default
title: "BLE Scanning"
parent: "Part 6: Advanced Topics"
nav_order: 16
---

# BLE Scanning

Scanning discovers nearby BLE devices and their advertising data.

## Basic Scanning

```c
#include <zephyr/bluetooth/bluetooth.h>

static void device_found(const bt_addr_le_t *addr, int8_t rssi,
                         uint8_t type, struct net_buf_simple *ad)
{
    char addr_str[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(addr, addr_str, sizeof(addr_str));
    printk("Found: %s (RSSI %d)\n", addr_str, rssi);
}

int start_scanning(void)
{
    struct bt_le_scan_param param = {
        .type = BT_LE_SCAN_TYPE_ACTIVE,  /* Request scan response */
        .options = BT_LE_SCAN_OPT_NONE,
        .interval = BT_GAP_SCAN_FAST_INTERVAL,  /* 60ms */
        .window = BT_GAP_SCAN_FAST_WINDOW,      /* 30ms */
    };

    int err = bt_le_scan_start(&param, device_found);
    if (err) {
        printk("Scan start failed: %d\n", err);
        return err;
    }

    printk("Scanning started\n");
    return 0;
}
```

## Scan Parameters

```c
/* Scan types */
BT_LE_SCAN_TYPE_PASSIVE  /* Just listen */
BT_LE_SCAN_TYPE_ACTIVE   /* Request scan response */

/* Scan options */
BT_LE_SCAN_OPT_NONE
BT_LE_SCAN_OPT_FILTER_DUPLICATE  /* Filter repeated advertisements */
BT_LE_SCAN_OPT_FILTER_ACCEPT_LIST  /* Only accepted devices */

/* Common presets */
BT_LE_SCAN_PASSIVE   /* Passive, no duplicates */
BT_LE_SCAN_ACTIVE    /* Active, no duplicates */
```

## Parsing Advertising Data

```c
static bool parse_ad_data(struct bt_data *data, void *user_data)
{
    switch (data->type) {
    case BT_DATA_NAME_COMPLETE:
    case BT_DATA_NAME_SHORTENED:
        printk("Name: %.*s\n", data->data_len, data->data);
        break;

    case BT_DATA_UUID16_ALL:
    case BT_DATA_UUID16_SOME:
        for (int i = 0; i < data->data_len; i += 2) {
            uint16_t uuid = data->data[i] | (data->data[i + 1] << 8);
            printk("UUID16: 0x%04x\n", uuid);
        }
        break;

    case BT_DATA_MANUFACTURER_DATA:
        uint16_t company = data->data[0] | (data->data[1] << 8);
        printk("Manufacturer: 0x%04x\n", company);
        break;

    case BT_DATA_TX_POWER:
        printk("TX Power: %d dBm\n", (int8_t)data->data[0]);
        break;
    }

    return true;  /* Continue parsing */
}

static void device_found(const bt_addr_le_t *addr, int8_t rssi,
                         uint8_t type, struct net_buf_simple *ad)
{
    bt_data_parse(ad, parse_ad_data, NULL);
}
```

## Filtering by Service UUID

```c
static const struct bt_uuid_16 target_uuid = BT_UUID_INIT_16(0x180D);  /* Heart Rate */

static bool find_service_uuid(struct bt_data *data, void *user_data)
{
    bool *found = user_data;

    if (data->type == BT_DATA_UUID16_ALL || data->type == BT_DATA_UUID16_SOME) {
        for (int i = 0; i < data->data_len; i += 2) {
            uint16_t uuid = data->data[i] | (data->data[i + 1] << 8);
            if (uuid == BT_UUID_16(&target_uuid)->val) {
                *found = true;
                return false;  /* Stop parsing */
            }
        }
    }

    return true;
}

static void device_found(const bt_addr_le_t *addr, int8_t rssi,
                         uint8_t type, struct net_buf_simple *ad)
{
    bool has_hr_service = false;

    bt_data_parse(ad, find_service_uuid, &has_hr_service);

    if (has_hr_service) {
        printk("Found Heart Rate device!\n");
        /* Connect to this device */
    }
}
```

## Connecting to Scanned Device

```c
static struct bt_conn *default_conn;

static void device_found(const bt_addr_le_t *addr, int8_t rssi,
                         uint8_t type, struct net_buf_simple *ad)
{
    char addr_str[BT_ADDR_LE_STR_LEN];
    int err;

    /* Check if this is the device we want */
    if (!is_target_device(addr, ad)) {
        return;
    }

    /* Stop scanning before connecting */
    bt_le_scan_stop();

    bt_addr_le_to_str(addr, addr_str, sizeof(addr_str));
    printk("Connecting to %s\n", addr_str);

    err = bt_conn_le_create(addr, BT_CONN_LE_CREATE_CONN,
                            BT_LE_CONN_PARAM_DEFAULT,
                            &default_conn);
    if (err) {
        printk("Connection failed: %d\n", err);
        /* Resume scanning */
        start_scanning();
    }
}
```

## Scan and Connect Pattern

```c
static K_SEM_DEFINE(ble_ready, 0, 1);
static struct bt_conn *conn;

static void connected(struct bt_conn *c, uint8_t err)
{
    if (err) {
        printk("Connection failed: %d\n", err);
        return;
    }

    conn = bt_conn_ref(c);
    printk("Connected\n");
    k_sem_give(&ble_ready);
}

static void disconnected(struct bt_conn *c, uint8_t reason)
{
    if (conn) {
        bt_conn_unref(conn);
        conn = NULL;
    }
    printk("Disconnected: %d\n", reason);
}

BT_CONN_CB_DEFINE(conn_cb) = {
    .connected = connected,
    .disconnected = disconnected,
};

static void device_found(const bt_addr_le_t *addr, int8_t rssi,
                         uint8_t type, struct net_buf_simple *ad)
{
    if (is_target(addr, ad)) {
        bt_le_scan_stop();
        bt_conn_le_create(addr, BT_CONN_LE_CREATE_CONN,
                         BT_LE_CONN_PARAM_DEFAULT, &conn);
    }
}

void scan_and_connect(void)
{
    bt_le_scan_start(BT_LE_SCAN_ACTIVE, device_found);

    /* Wait for connection */
    k_sem_take(&ble_ready, K_SECONDS(30));

    if (conn) {
        /* Connected, proceed with GATT operations */
    }
}
```

## Extended Scanning (BLE 5.0)

```c
/* CONFIG_BT_EXT_ADV=y */

struct bt_le_scan_param ext_param = {
    .type = BT_LE_SCAN_TYPE_ACTIVE,
    .options = BT_LE_SCAN_OPT_CODED,  /* Scan coded PHY */
    .interval = BT_GAP_SCAN_FAST_INTERVAL,
    .window = BT_GAP_SCAN_FAST_WINDOW,
};

bt_le_scan_start(&ext_param, device_found);
```

## Stop Scanning

```c
int err = bt_le_scan_stop();
if (err) {
    printk("Scan stop failed: %d\n", err);
}
```

## Best Practices

1. **Stop scanning before connecting** - Required by the stack
2. **Filter duplicates** - Reduce callbacks for same device
3. **Use appropriate interval/window** - Balance speed vs power
4. **Parse advertising data efficiently** - Early exit when target found
5. **Handle scan timeout** - Resume or notify user

## Next Steps

Learn about [GATT Server]({% link part6/17-ble-gatt-server.md %}) for creating services.
