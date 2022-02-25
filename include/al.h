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
#include <linux/if_link.h>
#include <stdlib.h>


#define BUFFER_SIZE             1024
#define BUFFER_SIZE_SMALL       16
#define MAX_CONNECTIONS         32
#define IF_NAMESIZE             128
typedef struct _client_payload
{
    int32_t cli_sock;
    ssize_t payload_len;
    char * payload;
    struct sockaddr_in cli_info;
}cpayload;

typedef enum _sock_type_
{
    SOCK_TCP = 0x00,
    SOCK_UDP = 0x01
}sock_type_t;



#ifdef __cplusplus
    extern "C" {
#endif

int32_t al_srv_open_sock(const sock_type_t sock_type);
int32_t al_srv_bind_sock(int32_t sockfd, const char bind_ip[], const uint16_t port_num);
int32_t al_srv_listen_sock(int32_t sockfd, const uint8_t max_conn);
int32_t al_srv_accept_sock(int32_t sockfd, struct sockaddr_in *cli);
int32_t al_srv_serve_reqs(int32_t sockfd, void (*serve_cb)(cpayload * payload));

int32_t al_close_sock(int32_t sockfd);
int32_t al_client_connect(const char ip_addr[], const uint16_t port_num);
int32_t al_get_ip_addr(char ip_addr[], const size_t len);
int32_t al_get_mac_addr(const char iface[], char mac_addr[], const size_t len);
int32_t al_get_if_stats(const char iface[], struct rtnl_link_stats * stats);

ssize_t al_write_sock(int32_t cli_sockfd, const void * buf, size_t len);
ssize_t al_read_sock(int32_t cli_sockfd, void * buf, size_t len);

#ifdef __cplusplus
    }
#endif

#endif // AL_H_
