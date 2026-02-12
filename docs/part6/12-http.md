---
layout: default
title: "HTTP"
parent: "Part 6: Advanced Topics"
nav_order: 12
---

# HTTP Client and Server

Zephyr provides HTTP client capabilities for REST APIs and web services.

## Kconfig

```kconfig
CONFIG_NETWORKING=y
CONFIG_NET_SOCKETS=y
CONFIG_NET_TCP=y
CONFIG_HTTP_CLIENT=y

# For HTTPS
CONFIG_NET_SOCKETS_SOCKOPT_TLS=y
CONFIG_TLS_CREDENTIALS=y
```

## HTTP Client

### Simple GET Request

```c
#include <zephyr/net/http/client.h>
#include <zephyr/net/socket.h>

#define SERVER_ADDR "192.168.1.100"
#define SERVER_PORT 80

static uint8_t recv_buf[1024];

void http_response_cb(struct http_response *rsp,
                      enum http_final_call final_data,
                      void *user_data)
{
    if (final_data == HTTP_DATA_MORE) {
        printk("Partial data: %zd bytes\n", rsp->data_len);
    } else if (final_data == HTTP_DATA_FINAL) {
        printk("All data received: %zd bytes\n", rsp->data_len);
        printk("Status: %s\n", rsp->http_status);
    }

    /* Response body is in rsp->body_frag_start with length rsp->body_frag_len */
    if (rsp->body_frag_len > 0) {
        printk("Body: %.*s\n", rsp->body_frag_len, rsp->body_frag_start);
    }
}

int http_get(const char *path)
{
    struct sockaddr_in addr;
    int sock;
    int ret;

    /* Create socket and connect */
    sock = zsock_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock < 0) {
        return -errno;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(SERVER_PORT);
    zsock_inet_pton(AF_INET, SERVER_ADDR, &addr.sin_addr);

    ret = zsock_connect(sock, (struct sockaddr *)&addr, sizeof(addr));
    if (ret < 0) {
        zsock_close(sock);
        return -errno;
    }

    /* Set up HTTP request */
    struct http_request req = { 0 };
    req.method = HTTP_GET;
    req.url = path;
    req.host = SERVER_ADDR;
    req.protocol = "HTTP/1.1";
    req.response = http_response_cb;
    req.recv_buf = recv_buf;
    req.recv_buf_len = sizeof(recv_buf);

    ret = http_client_req(sock, &req, 5000, NULL);

    zsock_close(sock);
    return ret;
}
```

### POST Request with JSON

```c
int http_post_json(const char *path, const char *json_data)
{
    struct sockaddr_in addr;
    int sock;
    int ret;

    sock = zsock_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(SERVER_PORT);
    zsock_inet_pton(AF_INET, SERVER_ADDR, &addr.sin_addr);
    zsock_connect(sock, (struct sockaddr *)&addr, sizeof(addr));

    /* Headers */
    static const char *headers[] = {
        "Content-Type: application/json\r\n",
        NULL
    };

    struct http_request req = { 0 };
    req.method = HTTP_POST;
    req.url = path;
    req.host = SERVER_ADDR;
    req.protocol = "HTTP/1.1";
    req.payload = json_data;
    req.payload_len = strlen(json_data);
    req.header_fields = headers;
    req.response = http_response_cb;
    req.recv_buf = recv_buf;
    req.recv_buf_len = sizeof(recv_buf);

    ret = http_client_req(sock, &req, 5000, NULL);

    zsock_close(sock);
    return ret;
}

/* Usage */
http_post_json("/api/sensor", "{\"temperature\": 23.5, \"humidity\": 45}");
```

### HTTPS with TLS

```c
#include <zephyr/net/tls_credentials.h>

static const char ca_cert[] =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIDxTCCAq2gAwIBAgIQAqxcJmoLQ...\n"
    "-----END CERTIFICATE-----\n";

#define TLS_SEC_TAG 1

int https_get(const char *host, const char *path)
{
    struct sockaddr_in addr;
    int sock;
    int ret;

    /* Add CA certificate */
    tls_credential_add(TLS_SEC_TAG, TLS_CREDENTIAL_CA_CERTIFICATE,
                       ca_cert, sizeof(ca_cert));

    /* Create TLS socket */
    sock = zsock_socket(AF_INET, SOCK_STREAM, IPPROTO_TLS_1_2);

    /* Configure TLS */
    sec_tag_t sec_tag_list[] = { TLS_SEC_TAG };
    zsock_setsockopt(sock, SOL_TLS, TLS_SEC_TAG_LIST,
                     sec_tag_list, sizeof(sec_tag_list));
    zsock_setsockopt(sock, SOL_TLS, TLS_HOSTNAME,
                     host, strlen(host) + 1);

    /* Connect (port 443) */
    addr.sin_family = AF_INET;
    addr.sin_port = htons(443);
    zsock_inet_pton(AF_INET, host, &addr.sin_addr);
    zsock_connect(sock, (struct sockaddr *)&addr, sizeof(addr));

    /* HTTP request over TLS */
    struct http_request req = { 0 };
    req.method = HTTP_GET;
    req.url = path;
    req.host = host;
    req.protocol = "HTTP/1.1";
    req.response = http_response_cb;
    req.recv_buf = recv_buf;
    req.recv_buf_len = sizeof(recv_buf);

    ret = http_client_req(sock, &req, 10000, NULL);

    zsock_close(sock);
    return ret;
}
```

## HTTP Server

Using socket directly for simple server:

```c
#define HTTP_PORT 80

static const char response_200[] =
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html\r\n"
    "Connection: close\r\n"
    "\r\n"
    "<html><body><h1>Hello from Zephyr!</h1></body></html>";

static const char response_404[] =
    "HTTP/1.1 404 Not Found\r\n"
    "Content-Type: text/plain\r\n"
    "Connection: close\r\n"
    "\r\n"
    "Not Found";

void http_server(void)
{
    int server_sock, client_sock;
    struct sockaddr_in addr;
    char buf[512];

    server_sock = zsock_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(HTTP_PORT);

    zsock_bind(server_sock, (struct sockaddr *)&addr, sizeof(addr));
    zsock_listen(server_sock, 5);

    printk("HTTP server listening on port %d\n", HTTP_PORT);

    while (1) {
        client_sock = zsock_accept(server_sock, NULL, NULL);
        if (client_sock < 0) {
            continue;
        }

        /* Read request */
        int len = zsock_recv(client_sock, buf, sizeof(buf) - 1, 0);
        if (len > 0) {
            buf[len] = '\0';

            /* Parse request (simple) */
            if (strncmp(buf, "GET / ", 6) == 0 ||
                strncmp(buf, "GET /index.html ", 16) == 0) {
                zsock_send(client_sock, response_200,
                          sizeof(response_200) - 1, 0);
            } else {
                zsock_send(client_sock, response_404,
                          sizeof(response_404) - 1, 0);
            }
        }

        zsock_close(client_sock);
    }
}
```

### REST API Server

```c
#include <zephyr/data/json.h>

/* JSON encoding for sensor data */
struct sensor_data {
    int temperature;
    int humidity;
};

static const struct json_obj_descr sensor_descr[] = {
    JSON_OBJ_DESCR_PRIM(struct sensor_data, temperature, JSON_TOK_NUMBER),
    JSON_OBJ_DESCR_PRIM(struct sensor_data, humidity, JSON_TOK_NUMBER),
};

void handle_api_request(int sock, const char *method, const char *path,
                        const char *body)
{
    char response[256];
    char json_buf[128];

    if (strcmp(path, "/api/sensor") == 0 && strcmp(method, "GET") == 0) {
        struct sensor_data data = {
            .temperature = read_temperature(),
            .humidity = read_humidity()
        };

        json_obj_encode_buf(sensor_descr, ARRAY_SIZE(sensor_descr),
                           &data, json_buf, sizeof(json_buf));

        snprintf(response, sizeof(response),
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: application/json\r\n"
                "Content-Length: %zu\r\n"
                "\r\n%s",
                strlen(json_buf), json_buf);

    } else if (strcmp(path, "/api/led") == 0 && strcmp(method, "POST") == 0) {
        /* Parse JSON body */
        struct { int state; } led_cmd;
        static const struct json_obj_descr led_descr[] = {
            JSON_OBJ_DESCR_PRIM(struct { int state; }, state, JSON_TOK_NUMBER),
        };

        json_obj_parse(body, strlen(body), led_descr, 1, &led_cmd);
        gpio_pin_set_dt(&led, led_cmd.state);

        snprintf(response, sizeof(response),
                "HTTP/1.1 200 OK\r\n"
                "Content-Length: 0\r\n"
                "\r\n");
    } else {
        snprintf(response, sizeof(response),
                "HTTP/1.1 404 Not Found\r\n"
                "Content-Length: 0\r\n"
                "\r\n");
    }

    zsock_send(sock, response, strlen(response), 0);
}
```

## Best Practices

1. **Reuse connections** - Use HTTP/1.1 keep-alive when possible
2. **Handle timeouts** - Network operations can stall
3. **Validate TLS certificates** - For HTTPS security
4. **Parse responses carefully** - Check status codes
5. **Use chunked encoding** - For streaming large responses

## Next Steps

Learn about [TLS]({% link part6/13-tls.md %}) for secure communication.
