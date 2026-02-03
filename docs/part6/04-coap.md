---
layout: default
title: "CoAP"
parent: "Part 6: Advanced Topics"
nav_order: 4
---

# CoAP Protocol

CoAP (Constrained Application Protocol) is a specialized web protocol for constrained devices and networks.

## CoAP vs HTTP

| Feature | CoAP | HTTP |
|---------|------|------|
| Transport | UDP | TCP |
| Header | 4 bytes | Variable, large |
| Methods | GET, POST, PUT, DELETE | Same + more |
| Observe | Built-in | Requires WebSocket |
| Best for | Constrained devices | General web |

## Kconfig

```kconfig
CONFIG_NETWORKING=y
CONFIG_NET_UDP=y
CONFIG_COAP=y

# For DTLS
CONFIG_COAP_DTLS_SUPPORT=y
```

## CoAP Client

```c
#include <zephyr/net/coap.h>
#include <zephyr/net/socket.h>

#define COAP_SERVER "192.168.1.100"
#define COAP_PORT   5683

static int sock;
static uint16_t next_token = 1;

int coap_client_init(void)
{
    struct sockaddr_in addr;

    sock = zsock_socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock < 0) {
        return -errno;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(COAP_PORT);
    zsock_inet_pton(AF_INET, COAP_SERVER, &addr.sin_addr);

    return zsock_connect(sock, (struct sockaddr *)&addr, sizeof(addr));
}

int coap_get(const char *path)
{
    struct coap_packet request;
    uint8_t buf[256];
    int ret;

    /* Initialize packet */
    ret = coap_packet_init(&request, buf, sizeof(buf),
                           COAP_VERSION_1,
                           COAP_TYPE_CON,  /* Confirmable */
                           sizeof(next_token),
                           (uint8_t *)&next_token,
                           COAP_METHOD_GET,
                           coap_next_id());
    if (ret < 0) {
        return ret;
    }

    next_token++;

    /* Add URI path */
    ret = coap_packet_append_option(&request, COAP_OPTION_URI_PATH,
                                    path, strlen(path));
    if (ret < 0) {
        return ret;
    }

    /* Send request */
    ret = zsock_send(sock, request.data, request.offset, 0);

    return ret;
}

int coap_receive_response(void)
{
    struct coap_packet reply;
    uint8_t buf[256];
    int ret;

    ret = zsock_recv(sock, buf, sizeof(buf), 0);
    if (ret < 0) {
        return ret;
    }

    ret = coap_packet_parse(&reply, buf, ret, NULL, 0);
    if (ret < 0) {
        return ret;
    }

    uint8_t code = coap_header_get_code(&reply);
    printk("Response code: %d.%02d\n",
           COAP_RESPONSE_CODE_CLASS(code),
           COAP_RESPONSE_CODE_DETAIL(code));

    /* Get payload */
    const uint8_t *payload;
    uint16_t len;
    payload = coap_packet_get_payload(&reply, &len);
    if (payload && len > 0) {
        printk("Payload: %.*s\n", len, payload);
    }

    return 0;
}
```

## CoAP Server

```c
#include <zephyr/net/coap.h>

static int handle_get_temperature(struct coap_resource *resource,
                                  struct coap_packet *request,
                                  struct sockaddr *addr,
                                  socklen_t addr_len)
{
    struct coap_packet response;
    uint8_t buf[256];
    uint8_t token[8];
    uint16_t id;
    uint8_t tkl;
    int ret;

    /* Get token from request */
    tkl = coap_header_get_token(request, token);
    id = coap_header_get_id(request);

    /* Build response */
    ret = coap_packet_init(&response, buf, sizeof(buf),
                           COAP_VERSION_1,
                           COAP_TYPE_ACK,
                           tkl, token,
                           COAP_RESPONSE_CODE_CONTENT,
                           id);
    if (ret < 0) {
        return ret;
    }

    /* Add content format */
    ret = coap_append_option_int(&response, COAP_OPTION_CONTENT_FORMAT,
                                 COAP_CONTENT_FORMAT_TEXT_PLAIN);

    /* Add payload */
    ret = coap_packet_append_payload_marker(&response);
    char payload[32];
    int len = snprintf(payload, sizeof(payload), "23.5");
    ret = coap_packet_append_payload(&response, payload, len);

    /* Send response */
    return zsock_sendto(sock, response.data, response.offset, 0,
                        addr, addr_len);
}

static int handle_post_led(struct coap_resource *resource,
                           struct coap_packet *request,
                           struct sockaddr *addr,
                           socklen_t addr_len)
{
    const uint8_t *payload;
    uint16_t len;

    payload = coap_packet_get_payload(request, &len);
    if (payload) {
        if (len >= 2 && strncmp(payload, "on", 2) == 0) {
            gpio_pin_set_dt(&led, 1);
        } else if (len >= 3 && strncmp(payload, "off", 3) == 0) {
            gpio_pin_set_dt(&led, 0);
        }
    }

    /* Send acknowledgment */
    /* ... similar to GET response ... */

    return 0;
}

/* Resource definitions */
static struct coap_resource resources[] = {
    {
        .path = (const char * const[]){ "sensor", "temperature", NULL },
        .get = handle_get_temperature,
    },
    {
        .path = (const char * const[]){ "actuator", "led", NULL },
        .post = handle_post_led,
    },
    { .path = NULL }  /* End marker */
};

void coap_server_loop(void)
{
    struct sockaddr_in addr;
    int ret;

    sock = zsock_socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(5683);

    zsock_bind(sock, (struct sockaddr *)&addr, sizeof(addr));

    while (1) {
        struct sockaddr client_addr;
        socklen_t client_len = sizeof(client_addr);
        uint8_t buf[256];

        ret = zsock_recvfrom(sock, buf, sizeof(buf), 0,
                            &client_addr, &client_len);
        if (ret < 0) {
            continue;
        }

        struct coap_packet request;
        ret = coap_packet_parse(&request, buf, ret, NULL, 0);
        if (ret < 0) {
            continue;
        }

        /* Find matching resource */
        struct coap_resource *r = coap_resource_match(resources, &request);
        if (r) {
            uint8_t method = coap_header_get_code(&request);
            switch (method) {
            case COAP_METHOD_GET:
                if (r->get) r->get(r, &request, &client_addr, client_len);
                break;
            case COAP_METHOD_POST:
                if (r->post) r->post(r, &request, &client_addr, client_len);
                break;
            case COAP_METHOD_PUT:
                if (r->put) r->put(r, &request, &client_addr, client_len);
                break;
            case COAP_METHOD_DELETE:
                if (r->del) r->del(r, &request, &client_addr, client_len);
                break;
            }
        }
    }
}
```

## CoAP Observe

```c
/* Client: Subscribe to resource */
int coap_observe_register(const char *path)
{
    struct coap_packet request;
    uint8_t buf[256];

    coap_packet_init(&request, buf, sizeof(buf),
                     COAP_VERSION_1, COAP_TYPE_CON,
                     sizeof(next_token), (uint8_t *)&next_token,
                     COAP_METHOD_GET, coap_next_id());

    /* Add observe option (0 = register) */
    coap_append_option_int(&request, COAP_OPTION_OBSERVE, 0);

    coap_packet_append_option(&request, COAP_OPTION_URI_PATH,
                              path, strlen(path));

    return zsock_send(sock, request.data, request.offset, 0);
}

/* Server: Handle observe registration */
static int observe_notify(struct coap_resource *resource,
                          struct sockaddr *addr,
                          socklen_t addr_len,
                          const char *payload)
{
    struct coap_packet response;
    uint8_t buf[256];

    coap_packet_init(&response, buf, sizeof(buf),
                     COAP_VERSION_1, COAP_TYPE_CON,
                     0, NULL,  /* Token stored in observer list */
                     COAP_RESPONSE_CODE_CONTENT,
                     coap_next_id());

    coap_append_option_int(&response, COAP_OPTION_OBSERVE,
                           observe_counter++);

    coap_packet_append_payload_marker(&response);
    coap_packet_append_payload(&response, payload, strlen(payload));

    return zsock_sendto(sock, response.data, response.offset, 0,
                        addr, addr_len);
}
```

## Block Transfer

For large payloads:

```c
/* Client: Request with block option */
int coap_get_block(const char *path, int block_num)
{
    struct coap_packet request;
    uint8_t buf[256];

    coap_packet_init(&request, buf, sizeof(buf),
                     COAP_VERSION_1, COAP_TYPE_CON,
                     sizeof(next_token), (uint8_t *)&next_token,
                     COAP_METHOD_GET, coap_next_id());

    coap_packet_append_option(&request, COAP_OPTION_URI_PATH,
                              path, strlen(path));

    /* Block2 option: request block_num with 64-byte size */
    uint32_t block = (block_num << 4) | 2;  /* SZX=2 (64 bytes) */
    coap_append_option_int(&request, COAP_OPTION_BLOCK2, block);

    return zsock_send(sock, request.data, request.offset, 0);
}
```

## API Reference

```c
/* Packet initialization */
int coap_packet_init(struct coap_packet *cpkt, uint8_t *data, uint16_t max_len,
                     uint8_t ver, uint8_t type, uint8_t token_len,
                     const uint8_t *token, uint8_t code, uint16_t id);

int coap_packet_parse(struct coap_packet *cpkt, uint8_t *data, uint16_t len,
                      struct coap_option *options, uint8_t opt_num);

/* Options */
int coap_packet_append_option(struct coap_packet *cpkt, uint16_t code,
                              const uint8_t *value, uint16_t len);
int coap_append_option_int(struct coap_packet *cpkt, uint16_t code,
                           unsigned int val);

/* Payload */
int coap_packet_append_payload_marker(struct coap_packet *cpkt);
int coap_packet_append_payload(struct coap_packet *cpkt, const uint8_t *payload,
                               uint16_t len);
const uint8_t *coap_packet_get_payload(const struct coap_packet *cpkt,
                                       uint16_t *len);

/* Header access */
uint8_t coap_header_get_code(const struct coap_packet *cpkt);
uint16_t coap_header_get_id(const struct coap_packet *cpkt);
uint8_t coap_header_get_token(const struct coap_packet *cpkt, uint8_t *token);
```

## Best Practices

1. **Use confirmable messages** - For reliable delivery
2. **Implement observe** - For real-time updates
3. **Use block transfer** - For large payloads
4. **Handle retransmissions** - CoAP over UDP is unreliable
5. **Use appropriate content format** - JSON, CBOR for structured data

## Next Steps

Learn about [HTTP]({% link part6/05-http.md %}) for web communication.
