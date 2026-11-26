#include "senddata.h"

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>

#define TM_PORT 10011
#define TC_PORT 10022

static const uint8_t payload[] = {
    0x08, 0x01, 0xC0, 0x01, 0x00, 0x1D,
    0x21, 0x03, 0x19, 0x00, 0x00, 0x00,
    0x00, 0x05, 0x6A, 0x3D, 0x00, 0x01,
    0x00, 0x01, 0x00, 0x1A, 0x00, 0x0B,
    0x00, 0x12, 0x00, 0x24, 0x00, 0x69,
    0x00, 0x00, 0x00, 0x00, 0x75, 0x5A
};

static int tm_listen_fd = -1;
static int tc_listen_fd = -1;
static int tm_client_fd = -1;
static int tc_client_fd = -1;

static int create_listener(int port)
{
    int fd;
    int opt = 1;
    struct sockaddr_in addr;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("TASTE socket");
        return -1;
    }

    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons((uint16_t)port);

    if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("TASTE bind");
        close(fd);
        return -1;
    }

    if (listen(fd, 1) < 0) {
        perror("TASTE listen");
        close(fd);
        return -1;
    }

    int flags = fcntl(fd, F_GETFL, 0);
    if (flags >= 0) {
        fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    }

    printf("TASTE listening on port %d\n", port);
    return fd;
}

static void accept_client(int *listen_fd, int *client_fd, int port)
{
    if (*listen_fd < 0 || *client_fd >= 0) {
        return;
    }

    int fd = accept(*listen_fd, NULL, NULL);
    if (fd >= 0) {
        *client_fd = fd;
        printf("Yamcs connected on port %d\n", port);
    } else if (errno != EAGAIN && errno != EWOULDBLOCK && errno != EINTR) {
        perror("TASTE accept");
    }
}

void senddata_startup(void)
{
    tm_listen_fd = create_listener(TM_PORT);
    tc_listen_fd = create_listener(TC_PORT);
}

void senddata_PI_trigger(void)
{
    accept_client(&tm_listen_fd, &tm_client_fd, TM_PORT);
    accept_client(&tc_listen_fd, &tc_client_fd, TC_PORT);

    if (tm_client_fd < 0) {
        return;
    }

    size_t sent = 0;
    while (sent < sizeof(payload)) {
        ssize_t n = send(
            tm_client_fd,
            payload + sent,
            sizeof(payload) - sent,
            MSG_NOSIGNAL
            );

        if (n < 0 && errno == EINTR) {
            continue;
        }

        if (n <= 0) {
            perror("TASTE telemetry send");
            close(tm_client_fd);
            tm_client_fd = -1;
            return;
        }

        sent += (size_t)n;
    }

    printf("TASTE sent %zu telemetry bytes\n", sent);

    if (tc_client_fd >= 0) {
        uint8_t tc_buffer[1024];
        ssize_t n = recv(tc_client_fd, tc_buffer, sizeof(tc_buffer), MSG_DONTWAIT);

        if (n > 0) {
            printf("TASTE received %zd telecommand bytes:", n);
            for (ssize_t i = 0; i < n; i++) {
                printf(" %02X", tc_buffer[i]);
            }
            printf("\n");
        } else if (n == 0) {
            printf("Yamcs disconnected from telecommand port\n");
            close(tc_client_fd);
            tc_client_fd = -1;
        } else if (errno != EAGAIN && errno != EWOULDBLOCK && errno != EINTR) {
            perror("TASTE telecommand receive");
            close(tc_client_fd);
            tc_client_fd = -1;
        }
    }
}
