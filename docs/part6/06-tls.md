---
layout: default
title: "TLS"
parent: "Part 6: Advanced Topics"
nav_order: 6
---

# TLS (Transport Layer Security)

TLS provides encrypted communication for secure network connections.

## Kconfig

```kconfig
CONFIG_NETWORKING=y
CONFIG_NET_SOCKETS=y
CONFIG_NET_SOCKETS_SOCKOPT_TLS=y
CONFIG_TLS_CREDENTIALS=y

# Choose TLS library
CONFIG_MBEDTLS=y
CONFIG_MBEDTLS_BUILTIN=y

# Enable cipher suites
CONFIG_MBEDTLS_KEY_EXCHANGE_RSA_ENABLED=y
CONFIG_MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED=y
CONFIG_MBEDTLS_ECP_DP_SECP256R1_ENABLED=y
```

## TLS Credentials

```c
#include <zephyr/net/tls_credentials.h>

/* CA certificate (PEM format) */
static const char ca_cert[] =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIDrzCCApegAwIBAgIQCDvgVpBCRrGhdWrJWZHHSjANBgkqhkiG9w0BAQUFADBh\n"
    /* ... certificate data ... */
    "-----END CERTIFICATE-----\n";

/* Client certificate (for mutual TLS) */
static const char client_cert[] =
    "-----BEGIN CERTIFICATE-----\n"
    /* ... */
    "-----END CERTIFICATE-----\n";

/* Client private key */
static const char client_key[] =
    "-----BEGIN RSA PRIVATE KEY-----\n"
    /* ... */
    "-----END RSA PRIVATE KEY-----\n";

#define TLS_SEC_TAG 1

int setup_tls_credentials(void)
{
    int ret;

    /* Add CA certificate */
    ret = tls_credential_add(TLS_SEC_TAG,
                             TLS_CREDENTIAL_CA_CERTIFICATE,
                             ca_cert, sizeof(ca_cert));
    if (ret < 0) {
        printk("Failed to add CA cert: %d\n", ret);
        return ret;
    }

    /* Add client certificate (for mTLS) */
    ret = tls_credential_add(TLS_SEC_TAG,
                             TLS_CREDENTIAL_SERVER_CERTIFICATE,
                             client_cert, sizeof(client_cert));
    if (ret < 0) {
        return ret;
    }

    /* Add client private key */
    ret = tls_credential_add(TLS_SEC_TAG,
                             TLS_CREDENTIAL_PRIVATE_KEY,
                             client_key, sizeof(client_key));

    return ret;
}
```

## TLS Client

```c
#include <zephyr/net/socket.h>
#include <zephyr/net/tls_credentials.h>

int tls_connect(const char *host, uint16_t port)
{
    struct sockaddr_in addr;
    int sock;
    int ret;

    /* Create TLS socket */
    sock = zsock_socket(AF_INET, SOCK_STREAM, IPPROTO_TLS_1_2);
    if (sock < 0) {
        printk("Socket creation failed: %d\n", errno);
        return -errno;
    }

    /* Set TLS options */
    sec_tag_t sec_tag_list[] = { TLS_SEC_TAG };
    ret = zsock_setsockopt(sock, SOL_TLS, TLS_SEC_TAG_LIST,
                          sec_tag_list, sizeof(sec_tag_list));
    if (ret < 0) {
        zsock_close(sock);
        return -errno;
    }

    /* Set hostname for SNI and certificate validation */
    ret = zsock_setsockopt(sock, SOL_TLS, TLS_HOSTNAME,
                          host, strlen(host) + 1);

    /* Set peer verification */
    int verify = TLS_PEER_VERIFY_REQUIRED;
    zsock_setsockopt(sock, SOL_TLS, TLS_PEER_VERIFY,
                    &verify, sizeof(verify));

    /* Connect */
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    zsock_inet_pton(AF_INET, host, &addr.sin_addr);

    ret = zsock_connect(sock, (struct sockaddr *)&addr, sizeof(addr));
    if (ret < 0) {
        printk("TLS connect failed: %d\n", errno);
        zsock_close(sock);
        return -errno;
    }

    printk("TLS connection established\n");
    return sock;
}

void tls_send_receive(int sock)
{
    char tx[] = "GET / HTTP/1.1\r\nHost: example.com\r\n\r\n";
    char rx[1024];

    zsock_send(sock, tx, strlen(tx), 0);

    int len = zsock_recv(sock, rx, sizeof(rx) - 1, 0);
    if (len > 0) {
        rx[len] = '\0';
        printk("Received: %s\n", rx);
    }
}
```

## TLS Server

```c
/* Server certificate and key */
static const char server_cert[] = "-----BEGIN CERTIFICATE-----\n...";
static const char server_key[] = "-----BEGIN RSA PRIVATE KEY-----\n...";

#define SERVER_SEC_TAG 2

int tls_server_setup(void)
{
    tls_credential_add(SERVER_SEC_TAG,
                       TLS_CREDENTIAL_SERVER_CERTIFICATE,
                       server_cert, sizeof(server_cert));

    tls_credential_add(SERVER_SEC_TAG,
                       TLS_CREDENTIAL_PRIVATE_KEY,
                       server_key, sizeof(server_key));

    return 0;
}

void tls_server(void)
{
    int server_sock, client_sock;
    struct sockaddr_in addr;

    server_sock = zsock_socket(AF_INET, SOCK_STREAM, IPPROTO_TLS_1_2);

    /* Configure TLS */
    sec_tag_t sec_tags[] = { SERVER_SEC_TAG };
    zsock_setsockopt(server_sock, SOL_TLS, TLS_SEC_TAG_LIST,
                    sec_tags, sizeof(sec_tags));

    /* Optional: require client certificate */
    int verify = TLS_PEER_VERIFY_OPTIONAL;  /* or REQUIRED */
    zsock_setsockopt(server_sock, SOL_TLS, TLS_PEER_VERIFY,
                    &verify, sizeof(verify));

    /* Bind and listen */
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(443);

    zsock_bind(server_sock, (struct sockaddr *)&addr, sizeof(addr));
    zsock_listen(server_sock, 5);

    while (1) {
        client_sock = zsock_accept(server_sock, NULL, NULL);
        /* Handle TLS client */
        handle_client(client_sock);
        zsock_close(client_sock);
    }
}
```

## DTLS (for UDP)

```c
int dtls_client(const char *host, uint16_t port)
{
    struct sockaddr_in addr;
    int sock;

    /* Create DTLS socket */
    sock = zsock_socket(AF_INET, SOCK_DGRAM, IPPROTO_DTLS_1_2);

    /* Configure TLS credentials */
    sec_tag_t sec_tags[] = { TLS_SEC_TAG };
    zsock_setsockopt(sock, SOL_TLS, TLS_SEC_TAG_LIST,
                    sec_tags, sizeof(sec_tags));

    zsock_setsockopt(sock, SOL_TLS, TLS_HOSTNAME,
                    host, strlen(host) + 1);

    /* Connect performs DTLS handshake */
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    zsock_inet_pton(AF_INET, host, &addr.sin_addr);

    zsock_connect(sock, (struct sockaddr *)&addr, sizeof(addr));

    return sock;
}
```

## TLS Socket Options

```c
/* Peer verification */
TLS_PEER_VERIFY_NONE      /* No verification */
TLS_PEER_VERIFY_OPTIONAL  /* Verify if cert provided */
TLS_PEER_VERIFY_REQUIRED  /* Must verify */

/* Cipher suites (example) */
int ciphers[] = {
    0xC02F,  /* TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256 */
    0
};
zsock_setsockopt(sock, SOL_TLS, TLS_CIPHERSUITE_LIST, ciphers, sizeof(ciphers));

/* Session caching */
zsock_setsockopt(sock, SOL_TLS, TLS_SESSION_CACHE, &enabled, sizeof(enabled));
```

## Best Practices

1. **Always verify certificates** - Use `TLS_PEER_VERIFY_REQUIRED`
2. **Use strong cipher suites** - Prefer ECDHE, GCM modes
3. **Keep certificates updated** - Check expiration dates
4. **Protect private keys** - Store securely
5. **Enable SNI** - Set `TLS_HOSTNAME` for virtual hosts

## Next Steps

Learn about [BLE Overview]({% link part6/07-ble-overview.md %}) for Bluetooth Low Energy.
