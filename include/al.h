// SPDX-License-Identifier: MIT
/** @file al.h
 *
 * @brief AL lightweight socket library header
 *
 * COPYRIGHT NOTICE: (c) 2022 Javad Rahimi.  All rights reserved.
 */

#ifndef AL_H_
#define AL_H_
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>

#define BUFFER_SIZE             1024
#define MAX_CONNECTIONS         32

typedef struct _client_payload
{
    int32_t cli_sock;
    ssize_t payload_len;
    char * payload;
    struct sockaddr_in cli_info;
}cpayload;

#ifdef __cplusplus
    extern "C" {
#endif

int32_t srv_open_sock(void);
int32_t srv_close_sock(int32_t sockfd);
int32_t srv_bind_sock(int32_t sockfd, const char bind_ip[], const uint16_t port_num);
int32_t srv_listen_sock(int32_t sockfd, const uint8_t max_conn);
int32_t srv_accept_sock(int32_t sockfd, struct sockaddr_in *cli);
int32_t srv_serve_reqs(int32_t sockfd, void (*serve_cb)(cpayload * payload));
ssize_t write_sock(int32_t cli_sockfd, const void * buf, size_t len);
ssize_t read_sock(int32_t cli_sockfd, void * buf, size_t len);
int32_t cli_connect(const char ip_addr[], const uint16_t port_num);

#ifdef __cplusplus
    }
#endif

#endif // AL_H_
