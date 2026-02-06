/*
 * TCP Client Example
 *
 * Demonstrates TCP socket communication with a server.
 * Connects, sends a message, receives response, and disconnects.
 */

#include <zephyr/kernel.h>
#include <zephyr/net/socket.h>
#include <zephyr/net/net_if.h>
#include <zephyr/net/dhcpv4.h>
#include <errno.h>
#include <string.h>

/* Server configuration - change for your environment */
#define SERVER_ADDR  "192.168.1.100"
#define SERVER_PORT  8080

#define RECV_BUF_SIZE 256

/* Wait for network interface to be ready */
static int wait_for_network(void)
{
	struct net_if *iface = net_if_get_default();

	if (!iface) {
		printk("No network interface found\n");
		return -ENODEV;
	}

	printk("Waiting for network...\n");

	/* Start DHCP */
	net_dhcpv4_start(iface);

	/* Wait for address - check if interface is up and has addresses */
	int attempts = 0;
	while (attempts < 30) {
		if (net_if_is_up(iface)) {
			struct in_addr addr = {0};
			/* Get first unicast address */
			if (net_if_ipv4_get_global_addr(iface, NET_ADDR_PREFERRED)) {
				printk("Network is ready\n");
				return 0;
			}
		}

		k_sleep(K_SECONDS(1));
		attempts++;
	}

	printk("DHCP timeout - using static configuration\n");
	return 0;  /* Continue anyway, may have static config */
}

/* Create and connect socket */
static int connect_to_server(void)
{
	struct sockaddr_in addr;
	int sock;
	int ret;

	/* Create socket */
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock < 0) {
		printk("Failed to create socket: %d\n", errno);
		return -errno;
	}

	printk("Socket created\n");

	/* Set up server address */
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(SERVER_PORT);

	ret = inet_pton(AF_INET, SERVER_ADDR, &addr.sin_addr);
	if (ret != 1) {
		printk("Invalid server address: %s\n", SERVER_ADDR);
		close(sock);
		return -EINVAL;
	}

	/* Connect to server */
	printk("Connecting to %s:%d...\n", SERVER_ADDR, SERVER_PORT);

	ret = connect(sock, (struct sockaddr *)&addr, sizeof(addr));
	if (ret < 0) {
		printk("Connection failed: %d\n", errno);
		close(sock);
		return -errno;
	}

	printk("Connected!\n");
	return sock;
}

/* Send HTTP GET request and receive response */
static int http_get(int sock, const char *path)
{
	char buf[RECV_BUF_SIZE];
	int len;
	int total = 0;

	/* Build HTTP request */
	len = snprintf(buf, sizeof(buf),
		       "GET %s HTTP/1.1\r\n"
		       "Host: %s\r\n"
		       "Connection: close\r\n"
		       "\r\n",
		       path, SERVER_ADDR);

	printk("Sending request:\n%s", buf);

	/* Send request */
	int sent = send(sock, buf, len, 0);
	if (sent < 0) {
		printk("Send failed: %d\n", errno);
		return -errno;
	}

	printk("Sent %d bytes\n", sent);
	printk("\nResponse:\n");
	printk("=========\n");

	/* Receive response */
	while ((len = recv(sock, buf, sizeof(buf) - 1, 0)) > 0) {
		buf[len] = '\0';
		printk("%s", buf);
		total += len;
	}

	printk("\n=========\n");
	printk("Received %d bytes total\n", total);

	if (len < 0) {
		printk("Receive error: %d\n", errno);
		return -errno;
	}

	return total;
}

/* Simple echo test */
static int echo_test(int sock)
{
	const char *message = "Hello from Zephyr TCP client!\n";
	char buf[RECV_BUF_SIZE];
	int len;

	printk("Sending: %s", message);

	/* Send message */
	len = send(sock, message, strlen(message), 0);
	if (len < 0) {
		printk("Send failed: %d\n", errno);
		return -errno;
	}

	printk("Sent %d bytes\n", len);

	/* Set receive timeout */
	struct timeval timeout = {
		.tv_sec = 5,
		.tv_usec = 0
	};
	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

	/* Receive response */
	len = recv(sock, buf, sizeof(buf) - 1, 0);
	if (len > 0) {
		buf[len] = '\0';
		printk("Received: %s\n", buf);
	} else if (len == 0) {
		printk("Server closed connection\n");
	} else {
		printk("Receive error: %d\n", errno);
	}

	return len;
}

int main(void)
{
	int sock;
	int ret;

	printk("\n");
	printk("TCP Client Example\n");
	printk("==================\n\n");

	/* Wait for network to be ready */
	ret = wait_for_network();
	if (ret < 0) {
		printk("Network initialization failed\n");
		return ret;
	}

	/* Run demo loop */
	int count = 0;
	while (1) {
		printk("\n--- Connection attempt %d ---\n\n", ++count);

		/* Connect to server */
		sock = connect_to_server();
		if (sock < 0) {
			printk("Will retry in 10 seconds...\n");
			k_sleep(K_SECONDS(10));
			continue;
		}

		/* Run echo test */
		echo_test(sock);

		/* Close connection */
		printk("\nClosing connection\n");
		close(sock);

		/* Wait before next attempt */
		printk("Next connection in 30 seconds...\n");
		k_sleep(K_SECONDS(30));
	}

	return 0;
}
