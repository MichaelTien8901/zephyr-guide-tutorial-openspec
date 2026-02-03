/*
 * UART Example
 *
 * Demonstrates UART communication with interrupt-driven receive
 * and ring buffer for data handling.
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/sys/ring_buffer.h>
#include <string.h>

/* UART device */
#define UART_DEVICE_NODE DT_CHOSEN(zephyr_console)

static const struct device *uart_dev = DEVICE_DT_GET(UART_DEVICE_NODE);

/* Ring buffer for received data */
#define RX_RING_BUF_SIZE 256
RING_BUF_DECLARE(rx_ring_buf, RX_RING_BUF_SIZE);

/* Semaphore to signal data available */
static K_SEM_DEFINE(rx_sem, 0, 1);

/* Statistics */
static uint32_t rx_count;
static uint32_t tx_count;

/* UART interrupt callback */
static void uart_cb(const struct device *dev, void *user_data)
{
	ARG_UNUSED(user_data);

	while (uart_irq_update(dev) && uart_irq_is_pending(dev)) {
		if (uart_irq_rx_ready(dev)) {
			uint8_t buf[64];
			int len;

			len = uart_fifo_read(dev, buf, sizeof(buf));
			if (len > 0) {
				ring_buf_put(&rx_ring_buf, buf, len);
				rx_count += len;
				k_sem_give(&rx_sem);
			}
		}
	}
}

/* Send string via UART */
static void uart_send(const char *str)
{
	size_t len = strlen(str);

	for (size_t i = 0; i < len; i++) {
		uart_poll_out(uart_dev, str[i]);
	}
	tx_count += len;
}

/* Send string with newline */
static void uart_println(const char *str)
{
	uart_send(str);
	uart_send("\r\n");
}

/* Process received data */
static void process_rx_data(void)
{
	uint8_t buf[64];
	int len;

	len = ring_buf_get(&rx_ring_buf, buf, sizeof(buf) - 1);
	if (len > 0) {
		buf[len] = '\0';

		/* Echo back with prefix */
		uart_send("Echo: ");

		for (int i = 0; i < len; i++) {
			if (buf[i] == '\r' || buf[i] == '\n') {
				uart_send("\r\n");
			} else {
				uart_poll_out(uart_dev, buf[i]);
				tx_count++;
			}
		}
	}
}

/* Command handler */
static void handle_command(const char *cmd, size_t len)
{
	if (len >= 4 && strncmp(cmd, "help", 4) == 0) {
		uart_println("");
		uart_println("Available commands:");
		uart_println("  help  - Show this help");
		uart_println("  stats - Show statistics");
		uart_println("  clear - Clear statistics");
		uart_println("  hello - Greeting");
	} else if (len >= 5 && strncmp(cmd, "stats", 5) == 0) {
		char buf[64];
		uart_println("");
		snprintf(buf, sizeof(buf), "RX bytes: %u", rx_count);
		uart_println(buf);
		snprintf(buf, sizeof(buf), "TX bytes: %u", tx_count);
		uart_println(buf);
	} else if (len >= 5 && strncmp(cmd, "clear", 5) == 0) {
		rx_count = 0;
		tx_count = 0;
		uart_println("");
		uart_println("Statistics cleared");
	} else if (len >= 5 && strncmp(cmd, "hello", 5) == 0) {
		uart_println("");
		uart_println("Hello from Zephyr UART!");
	}
}

/* Main processing thread */
static void uart_thread_entry(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1);
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

	static char cmd_buf[64];
	static size_t cmd_pos = 0;

	while (1) {
		/* Wait for data */
		k_sem_take(&rx_sem, K_FOREVER);

		/* Process all available data */
		uint8_t c;
		while (ring_buf_get(&rx_ring_buf, &c, 1) == 1) {
			/* Echo character */
			uart_poll_out(uart_dev, c);
			tx_count++;

			if (c == '\r' || c == '\n') {
				if (cmd_pos > 0) {
					uart_send("\r\n");
					handle_command(cmd_buf, cmd_pos);
					cmd_pos = 0;
				}
				uart_send("> ");
			} else if (c == 0x7F || c == 0x08) {
				/* Backspace */
				if (cmd_pos > 0) {
					cmd_pos--;
					uart_send("\b \b");
				}
			} else if (cmd_pos < sizeof(cmd_buf) - 1) {
				cmd_buf[cmd_pos++] = c;
			}
		}
	}
}

K_THREAD_DEFINE(uart_tid, 1024, uart_thread_entry, NULL, NULL, NULL, 7, 0, 0);

int main(void)
{
	if (!device_is_ready(uart_dev)) {
		printk("UART device not ready\n");
		return -1;
	}

	/* Set up interrupt callback */
	uart_irq_callback_user_data_set(uart_dev, uart_cb, NULL);

	/* Enable RX interrupt */
	uart_irq_rx_enable(uart_dev);

	printk("\n");
	printk("UART Example\n");
	printk("============\n");
	printk("Device: %s\n", uart_dev->name);
	printk("\n");
	printk("Type 'help' for available commands\n");
	printk("\n");
	uart_send("> ");

	/* Main thread can do other work */
	while (1) {
		k_sleep(K_SECONDS(30));
		printk("[Status] RX: %u, TX: %u bytes\n", rx_count, tx_count);
	}

	return 0;
}
