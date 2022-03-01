// SPDX-License-Identifier: MIT
/** @file al.c
 *
 * @brief AL lightweight socket library source file.
 *
 * COPYRIGHT NOTICE: (c) 2022 Javad Rahimi.  All rights reserved.
 */
#include "al.h"

#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#include <linux/if_link.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <ifaddrs.h>

#define _DEBUG_ 1
#define dbg(fmt, ...)                            \
    do                                           \
    {                                            \
        if (_DEBUG_)                             \
            fprintf(stderr, fmt, ##__VA_ARGS__); \
    } while (0)
#define db_perror(err_msg)   \
    do                       \
    {                        \
        if (_DEBUG_)         \
            perror(err_msg); \
    } while (0)

static struct sockaddr_in g_addr;

static inline int32_t check_sock(const int32_t sock)
{
    if (-1 == sock)
    {
        db_perror("check_sock");
        al_close_sock(sock);
        return -1;
    }
    else
    {
        return 0;
    }
}
/**
 * @brief al_srv_open_sock:    opens a socket
 * @param sock_type The type of the socket (Current TCP and UDP are supported)
 *                  for TCP the SOCK_TCP and for UDP the SOCK_UDP should be passed.
 * @return On success: the socket, On failure: error code
 */
int32_t al_srv_open_sock(const sock_type_t sock_type)
{
    int sock = 0;
    int fret = -1;
    int optionValue = 1;

    switch (sock_type)
    {
    case SOCK_TCP:
        sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        break;
    case SOCK_UDP:
        sock = socket(AF_INET, SOCK_DGRAM, 0);

    default:
        break;
    }

    if (-1 != sock)
    {
        db_perror("Sock Open");
        fret = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optionValue, sizeof(optionValue));
        if (-1 == fret)
        {
            sock = -1;
        }
    }
    return sock;
}

/**
 * @brief al_srv_bind_sock:   binds a socket to a port [and ip address (optional)]
 *
 * @param sockfd socket file descriptor
 * @param bind_ip IP address to bind to
 * @param port_num port number to bind to
 * @return int32_t On success: 0, On failure: -1 and errno is set appropriately
 */
int32_t al_srv_bind_sock(int32_t sockfd, const char bind_ip[], const uint16_t port_num)
{
    int32_t fret = -1;

    bzero(&g_addr, sizeof(g_addr));
    g_addr.sin_family = AF_INET;

    if ((NULL != bind_ip) &&
        strlen(bind_ip) > 0)
    {
        g_addr.sin_addr.s_addr = inet_addr(bind_ip);
    }
    else
    {
        g_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    }
    g_addr.sin_port = htons(port_num);

    fret = bind(sockfd, (const struct sockaddr *)&g_addr, sizeof(g_addr));

    if (-1 == fret)
    {
        db_perror("Bind");
        close(sockfd);
    }

    return fret;
}

/**
 * @brief al_srv_listen_sock:   listens on a socket
 *
 * @param sockfd socket file descriptor
 * @param max_conn the maximum length to which the  queue  of
       pending  connections  for sockfd may grow.
 * @return int32_t On success: 0, On failure: -1 and errno is set appropriately
 */
int32_t al_srv_listen_sock(int32_t sockfd, const uint8_t max_conn)
{
    int32_t fret = -1;

    fret = listen(sockfd, max_conn);

    if (-1 == fret)
    {
        db_perror("Listen");
        close(sockfd);
    }

    return fret;
}

/**
 * @brief al_srv_accept_sock:   accepts a connection on a socket
 *
 * @param sockfd socket file descriptor
 * @param cli a pointer to a sockaddr structure.  This structure
       is filled in with the address of the peer socket, as known
        to the communications  layer.
 * @return int32_t On success: the peer client socket, On failure: -1 and errno is set appropriately
 */
int32_t al_srv_accept_sock(int32_t sockfd, struct sockaddr_in *cli)
{
    int cli_sck = -1;
    socklen_t cli_length = sizeof(*cli);

    cli_sck = accept(sockfd,
                     (struct sockaddr *)cli, &cli_length);

    if (-1 == cli_sck)
    {
        db_perror("Accept");
        close(sockfd);
    }

    return cli_sck;
}

/**
 * @brief al_write_sock:   write into a socket
 *
 * @param sockfd socket file descriptor
 * @param buf the buffer to be written into the socket
 * @param len the length of the buffer
 * @return ssize_t On success: the number of bytes written, On failure: -1 and errno is set appropriately
 */
ssize_t al_write_sock(int32_t sockfd, const void *buf, size_t len)
{
    ssize_t fret = -1;

    if (0 < sockfd)
    {
        fret = write(sockfd, buf, len);
        if (-1 == fret)
        {
            db_perror("al_write_sock");
        }
    }

    return fret;
}

/**
 * @brief al_read_sock:   read from a socket
 *
 * @param sockfd socket file descriptor
 * @param buf the buffer to be read from the socket
 * @param len the length of the buffer
 * @return ssize_t On success: the number of bytes read, On failure: -1 and errno is set appropriately
 */
ssize_t al_read_sock(int32_t sockfd, void *buf, size_t len)
{
    ssize_t fret = -1;

    if ((0 < sockfd) ||
        (NULL != buf))
    {
        fret = read(sockfd, buf, len);
        if (-1 == fret)
        {
            db_perror("al_read_sock");
        }
    }

    return fret;
}

/**
 * @brief al_close_sock:   closes a socket
 *
 * @param sockfd socket file descriptor
 * @return int32_t On success: 0, On failure: -1 and errno is set appropriately
 */
int32_t al_close_sock(int32_t sockfd)
{
    int32_t fret = -1;

    fret = close(sockfd);

    if (-1 == fret)
    {
        db_perror("Close Sock");
    }

    return fret;
}

/**
 * @brief al_get_ip_addr:   gets the ip address of the local machine
 *
 * @param ip_addr a pointer to a buffer to store the ip addresses in iface:ip format
 * @param len the length of the buffer
 * @return int32_t On success: 0, On failure: -1 and errno is set appropriately
 */
int32_t al_get_ip_addr(char ip_addr[], const size_t len)
{
    int32_t fret = -1;
    struct ifaddrs *ifaddr, *ifa;
    int32_t family, s;
    char host[NI_MAXHOST];
    char buf[BUFFER_SIZE * 2] = {0};

    if (0 != getifaddrs(&ifaddr))
    {
        db_perror("Get Ip Addr");
        return fret;
    }

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
    {
        if (NULL == ifa->ifa_addr)
        {
            continue;
        }

        family = ifa->ifa_addr->sa_family;

        if (AF_INET == family)
        {
            s = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in),
                            host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
            if (0 != s)
            {
                db_perror("Get Name Info");
                continue;
            }

            sprintf(buf, "%s:%s", ifa->ifa_name, host);

            strncat(ip_addr, buf, strlen(buf));
            strcat(ip_addr, " ");
            bzero(buf, BUFFER_SIZE);
            fret = 0;
        }
    }

    freeifaddrs(ifaddr);

    return fret;
}

/**
 * @brief al_get_mac_addr:   gets the mac address of the local machine
 *
 * @param mac_addr a pointer to a buffer to store the mac address
 * @param len the length of the buffer
 * @return int32_t On success: 0, On failure: -1 and errno is set appropriately
 */
int32_t al_get_mac_addr(const char iface[], char mac_addr[], const size_t len)
{

    int32_t fret = -1;
    struct ifreq s;
    int fd;
    unsigned char buf[BUFFER_SIZE_SMALL] = {0};

    if (iface == NULL)
    {
        return fret;
    }

    strcpy(s.ifr_name, iface);
    fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (-1 == fd)
    {
        db_perror("Socket");
        return fret;
    }
    if (0 == ioctl(fd, SIOCGIFHWADDR, &s))
    {
        int i;
        for (i = 0; i < 6; ++i)
        {
            sprintf(buf, " %02x", (unsigned char)s.ifr_addr.sa_data[i]);
            strncat(mac_addr, buf, strlen(buf));
            bzero(buf, BUFFER_SIZE_SMALL);
        }
        fret = 0;
    }
    return fret;
}

/**
 * @brief al_get_if_stats :   gets the statistics of the specified interface
 *
 * @param iface    the interface name
 * @param stats    a pointer to a struct if_stats to store the statistics
 * @return int32_t On success: 0, On failure: -1 and errno is set appropriately
 */
int32_t al_get_if_stats(const char iface[], struct rtnl_link_stats *if_stats)
{
    struct ifaddrs *ifaddr, *ifa;
    int32_t family, s, n, ret = -1;

    if (getifaddrs(&ifaddr) == -1)
    {
        db_perror("getifaddrs");
        return ret;
    }

    for (ifa = ifaddr, n = 0; ifa != NULL; ifa = ifa->ifa_next, n++)
    {
        if ((ifa->ifa_addr == NULL) || (strcmp(ifa->ifa_name, iface) != 0))
            continue;

        family = ifa->ifa_addr->sa_family;

        if (family == AF_PACKET && ifa->ifa_data != NULL)
        {
            struct rtnl_link_stats *stats = ifa->ifa_data;
            memcpy(if_stats, stats, sizeof(struct rtnl_link_stats));
        }
    }
}

/**
 * @brief srv_register_new_socket : registers a new socket for the server
 *
 * @param client_socket_fd the socket file descriptor to be registered
 * @param cli_sock_fd the array of registered sockets to be updated
 * @param socks_num number of registered sockets
 * @return int32_t On Success: registered socket index, On Failure: -1
 */
static int32_t srv_register_new_socket(int client_socket_fd, int *cli_sock_fd, int socks_num)
{
    int32_t i = 0;
    for (i = 0; i < socks_num; ++i)
    {
        if (cli_sock_fd[i] == 0)
        {
            cli_sock_fd[i] = client_socket_fd;

            return i;
        }
    }
    return -1;
}

/**
 * @brief al_srv_serve_reqs:   serves requests from clients
 *
 * @param sockfd server socket file descriptor
 * @param serve_cb callback function to serve requests
 * @param payload payload to be passed to the callback function
 * @return int32_t this function never returns :)
 */
int32_t al_srv_serve_reqs(int32_t sockfd, void (*serve_cb)(cpayload *payload))
{
    int fret = -1;
    fd_set read_fd_set;

    char input_buffer[BUFFER_SIZE] = {0};
    int client_socks[MAX_CONNECTIONS] = {0};
    socklen_t sock_addr_size = sizeof(struct sockaddr);
    struct sockaddr_in client_socket_addr;

    while (1)
    {
        FD_ZERO(&read_fd_set);
        FD_SET(sockfd, &read_fd_set);
        int max_sock_fd = sockfd;

        int i = 0;
        for (i = 0; i < MAX_CONNECTIONS; ++i)
        {
            int client_socket_fd = client_socks[i];

            if (client_socket_fd > 0)
            {
                FD_SET(client_socket_fd, &read_fd_set);
            }
            if (client_socket_fd > max_sock_fd)
            {
                max_sock_fd = client_socket_fd;
            }
        }

        int events = select(max_sock_fd + 1, &read_fd_set, NULL, NULL, NULL);

        if (events == -1 && errno != EINTR)
        {
            dbg("[ERROR] An error occurred while monitoring sockets: %s\n", strerror(errno));
        }
        if (FD_ISSET(sockfd, &read_fd_set))
        {
            // Establish connection with client
            int client_socket_fd = accept(sockfd, (struct sockaddr *)(&client_socket_addr), &sock_addr_size);

            if (client_socket_fd == -1)
            {
                dbg("[WARN][TCP] Failed to accpet a socket from client: %s\n", strerror(errno));
                continue;
            }
            dbg("[INFO][TCP] Connection established with %s:%d\n",
                inet_ntoa(client_socket_addr.sin_addr), ntohs(client_socket_addr.sin_port));

            // Register file descriptors for sockets
            int client_socket_fd_index = srv_register_new_socket(client_socket_fd, client_socks, MAX_CONNECTIONS);
            if (client_socket_fd_index == -1)
            {
                dbg("[WARN][TCP] Failed to register the socket for client: %s:%d\n",
                    inet_ntoa(client_socket_addr.sin_addr), ntohs(client_socket_addr.sin_port));
            }
            else
            {
                dbg("[INFO][TCP] Socket #%d registered for the client: %s:%d\n",
                    client_socket_fd_index, inet_ntoa(client_socket_addr.sin_addr), ntohs(client_socket_addr.sin_port));
            }
        }

        for (i = 0; i < MAX_CONNECTIONS; ++i)
        {
            int client_socket_fd = client_socks[i];
            if (FD_ISSET(client_socket_fd, &read_fd_set))
            {
                ssize_t read_bytes = al_read_sock(client_socket_fd, input_buffer, BUFFER_SIZE);

                if (read_bytes < 0)
                {
                    dbg("[ERROR][TCP] An error occurred while sending message to the client %s:%d: %s\nThe connection is going to close.\n",
                        inet_ntoa(client_socket_addr.sin_addr), ntohs(client_socket_addr.sin_port), strerror(errno));
                    continue;
                }

                dbg("[INFO][TCP] Received a message from client %s:%d: %s\n",
                    inet_ntoa(client_socket_addr.sin_addr), ntohs(client_socket_addr.sin_port), input_buffer);

                if (read_bytes == 0 || strncmp("BYE", input_buffer, 3) == 0)
                {
                    // Complete receiving message from client
                    close(client_socket_fd);
                    client_socks[i] = 0;
                    dbg("[INFO][TCP] Client %s:%d disconnected --->%s\n",
                        inet_ntoa(client_socket_addr.sin_addr), ntohs(client_socket_addr.sin_port), input_buffer);

                    continue;
                }
                else
                {
                    cpayload *cli_payload = (cpayload *)malloc(sizeof(cpayload));
                    cli_payload->payload = input_buffer;
                    cli_payload->payload_len = read_bytes;
                    cli_payload->cli_sock = client_socket_fd;
                    cli_payload->cli_info = client_socket_addr;
                    serve_cb(cli_payload);
                    free(cli_payload);
                    bzero(input_buffer, BUFFER_SIZE);
                }
            }
        }
    }

    return fret;
}

/**
 * @brief static function for udp client to server connection
 * 
 * @param srv_ip server ip address
 * @param srv_port server listening port
 * @return int32_t On success: client socket, On failure: -1 and errno is set appropriately
 */
static int32_t udp_cli_connect(const char * srv_ip, const int32_t srv_port)
{
    int32_t fret = 0;
    int32_t client_sock;
    struct sockaddr_in cliaddr = {0};    
    cliaddr.sin_family = AF_INET;
    cliaddr.sin_addr.s_addr   = inet_addr(srv_ip);
    cliaddr.sin_port   = htons(srv_port);
    
    client_sock = al_srv_open_sock(SOCK_UDP);
    if(client_sock < 0)
    {
        perror("udp_cli_connect");
        fret = -1;
    }
    else
    {
        fret = client_sock;
    }

    return fret;
}

/**
 * @brief static function for tcp client to server connection
 * 
 * @param srv_ip server ip address
 * @param srv_port server listening port
 * @return int32_t On success: client socket, On failure: -1 and errno is set appropriately
 */
static int32_t tcp_cli_connect(const char * srv_ip, const int32_t srv_port)
{
    int32_t cli_sock = -1;
    int32_t fret = -1;
    struct sockaddr_in client_addr;

    cli_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (-1 == cli_sock)
    {
        db_perror("Socket");
        return cli_sock;
    }

    bzero(&client_addr, sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    client_addr.sin_addr.s_addr = inet_addr(srv_ip);
    client_addr.sin_port = htons(srv_port);

    fret = connect(cli_sock, (struct sockaddr *)&client_addr,
                   sizeof(client_addr));

    if (fret)
    {
        db_perror("[al_client_connect]");
        close(cli_sock);
        return fret;
    }

    return cli_sock;
}

/**
 * @brief al_client_connect:   connect to a server
 *
 * @param ip_addr the ip address of the server
 * @param port_num the port number of the server
 * @param conn_type Type of the connection. see sock_type_t for the supported protocols.
 * @return int32_t On success: client socket, On failure: -1 and errno is set appropriately
 */
int32_t al_client_connect(const char ip_addr[],
                          const uint16_t port_num, sock_type_t conn_type)
{
    int32_t cli_sock = -1;
    switch (conn_type)
    {
    case SOCK_TCP:
        cli_sock = tcp_cli_connect(ip_addr, port_num);
        break;
    case SOCK_UDP:
        cli_sock = udp_cli_connect(ip_addr,port_num);
        break;
    default:
        cli_sock = -1;
        break;
    }

    return -1;
    
}

/* UDP functions */

/**
 * @brief al_recvfrom   receiving udp data from the socket
 *
 * @param sockfd socket file descriptor
 * @param buf the buffer to be read from the socket
 * @param len the length of the buffer
 * @param peer_sockaddr the peer address structure. Set NULL, if not required.
 * @param flags the flags to pass; accroding to recvfrom(2)
 * @return ssize_t  The number of bytes received, or -1 if an error occurred.
 *                  In the event of an error, errno is set to indicate the error.
 */
ssize_t al_recvfrom(int32_t sockfd, void *buf, size_t len, struct sockaddr *peer_sockaddr, const int32_t flags)
{
    int fret = -1;
    socklen_t sock_len = sizeof(*peer_sockaddr);
    if (-1 == check_sock(sockfd))
    {
        return -1;
    }

    if (NULL == peer_sockaddr)
    {
        return -1;
    }

    fret = recvfrom(sockfd, buf, len, flags, peer_sockaddr, &sock_len);
    if (-1 == fret)
    {
        db_perror("[al_recvfrom_");
    }
    return fret;
}

/**
 * @brief
 *
 @param sockfd socket file descriptor
 * @param buf the buffer to be read from the socket
 * @param buf_len the length of the buffer
 * @param peer the peer address structure
 * @param flags the flags to pass; accroding to sendto(2)
 * @return  On success, these calls return the number of bytes sent.
 *          On error, -1 is returned, and errno is set appropriately.
 */
ssize_t al_sendto(int32_t sockfd, const void *buf, const size_t buf_len, struct sockaddr *peer, const int32_t flags)
{
    int fret = 0;
    if (-1 != check_sock(sockfd))
    {

        if ((NULL != buf) && (NULL != peer) && (0 < buf_len))
        {
            socklen_t len = sizeof(*peer);
            fret = sendto(sockfd, (const char *)buf, buf_len,
                          flags, peer, len);
            if (-1 == fret)
            {
                perror("sendto");
            }
        }
        else
        {
            fret = -1;
        }
    }
    else
    {
        fret = -1;
    }

    return fret;
}