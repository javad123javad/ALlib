// SPDX-License-Identifier: GPL-2.0-only
/** @file al.c
 *
 * @brief AL lightweight socket library source file.
 *
 * COPYRIGHT NOTICE: (c) 2022 Javad Rahimi.  All rights reserved.
 */
#include "al.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>

#define _DEBUG_ 0
#define dbg(fmt, ...) \
    do { if (_DEBUG_) fprintf(stderr, fmt, ##__VA_ARGS__); } while (0)


static struct sockaddr_in g_addr;

/**
 * @brief srv_open_sock:    opens a socket
 * @return On success: the socket, On failure: error code
 */
int32_t srv_open_sock()
{
    int sock = 0;
    int optionValue = 1;
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(-1 == sock)
    {
        perror("Sock Open");
    }
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optionValue, sizeof(optionValue));
    return sock;
}

int32_t srv_bind_sock(int32_t sockfd, const char bind_ip[], const uint16_t port_num)
{
    int32_t fret = -1;

    bzero(&g_addr, sizeof (g_addr));
    g_addr.sin_family = AF_INET;

    if(bind_ip != NULL &&
            strlen(bind_ip) > 0)
    {
        g_addr.sin_addr.s_addr = inet_addr(bind_ip);
    }
    else
    {
        g_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    }
    g_addr.sin_port = htons(port_num);

    fret = bind(sockfd, (struct sockaddr*)&g_addr, sizeof (g_addr));

    if(-1 == fret)
    {
        perror("Bind");
        close(sockfd);
    }

    return fret;

}

int32_t srv_listen_sock(int32_t sockfd, const uint8_t max_conn)
{
    int32_t fret = -1;

    fret = listen(sockfd, max_conn);

    if( -1 == fret)
    {
        perror("Listen");
        close(sockfd);
    }

    return fret;

}

int32_t srv_accept(int32_t sockfd, struct sockaddr_in *cli)
{
    int cli_sck = -1;
    socklen_t cli_length = sizeof(*cli);

    cli_sck = accept(sockfd,
                     (struct sockaddr*)cli, &cli_length);

    if(-1 == cli_sck)
    {
        perror("Accept");
        close(sockfd);
    }

    return cli_sck;
}

ssize_t write_sock(int32_t cli_sockfd, const void *buf, size_t len)
{
    ssize_t fret = -1;

    if( 0 < cli_sockfd)
    {
        fret = write(cli_sockfd, buf, len);
        if( -1 == fret)
        {
            perror("Write Sock");
        }
    }

    return fret;

}

ssize_t read_sock(int32_t cli_sockfd, void *buf, size_t len)
{
    ssize_t fret = -1;
    fret = read(cli_sockfd, buf, len);
    return fret;
}

int32_t cli_connect(const char ip_addr[],
                    const uint16_t port_num)
{
    int cli_sock = -1;
    int fret = -1;
    struct sockaddr_in client_addr;
    // struct ifreq ifr;
    // memset(&ifr, 0, sizeof(ifr));
    // snprintf(ifr.ifr_name, sizeof(ifr.ifr_name), "lo");
    
    cli_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if( -1 == cli_sock)
    {
        perror("Socket");
        return  cli_sock;
    }

//     if(setsockopt(cli_sock, SOL_SOCKET, SO_BINDTODEVICE, (void *)&ifr, sizeof(ifr)) < 0)
//     {
//         perror("setsockopt");
//         return -1;
//     }

    bzero(&client_addr, sizeof (client_addr));
    client_addr.sin_family = AF_INET;
    client_addr.sin_addr.s_addr=inet_addr(ip_addr );
    client_addr.sin_port = htons(port_num);

    fret = connect(cli_sock, (struct sockaddr*) &client_addr,
                   sizeof (client_addr));

    if(fret)
    {
        perror("[cli_connect]");
        close(cli_sock);
        return fret;
    }



    return cli_sock;
}

int32_t srv_serve_reqs(int32_t sockfd, void (*serve_cb)(cpayload * payload))
{
    int fret = -1;
    fd_set read_fd_set;

    char input_buffer[BUFFER_SIZE] = {0};
    int client_socks[MAX_CONNECTIONS] = {0};
    socklen_t sock_addr_size = sizeof(struct sockaddr);
    struct sockaddr_in client_socket_addr;

    while ( 1 )
    {
        FD_ZERO(&read_fd_set);
        FD_SET(sockfd, &read_fd_set);
        int max_sock_fd = sockfd;

        int i = 0;
        for ( i = 0; i < MAX_CONNECTIONS; ++ i )
        {
            int client_socket_fd = client_socks[i];

            if ( client_socket_fd > 0 )
            {
                FD_SET(client_socket_fd, &read_fd_set);
            }
            if ( client_socket_fd > max_sock_fd )
            {
                max_sock_fd = client_socket_fd;
            }
        }

        int events = select(max_sock_fd + 1, &read_fd_set, NULL, NULL, NULL);

        if ( events == -1 && errno != EINTR )
        {
            dbg("[ERROR] An error occurred while monitoring sockets: %s\n", strerror(errno));
        }
        if ( FD_ISSET(sockfd, &read_fd_set) )
        {
            // Establish connection with client
            int clientSocketFD = accept(sockfd, (struct sockaddr *)(&client_socket_addr), &sock_addr_size);

            if ( clientSocketFD == -1 )
            {
                dbg("[WARN][TCP] Failed to accpet a socket from client: %s\n", strerror(errno));
                continue;
            }
            dbg("[INFO][TCP] Connection established with %s:%d\n",
                    inet_ntoa(client_socket_addr.sin_addr), ntohs(client_socket_addr.sin_port));

            // Register file descriptors for sockets
            int clientSocketFDIndex = registerNewSocket(clientSocketFD, client_socks, MAX_CONNECTIONS);
            if ( clientSocketFDIndex == -1 )
            {
                dbg("[WARN][TCP] Failed to register the socket for client: %s:%d\n",
                        inet_ntoa(client_socket_addr.sin_addr), ntohs(client_socket_addr.sin_port));
            } else {
                dbg("[INFO][TCP] Socket #%d registered for the client: %s:%d\n",
                        clientSocketFDIndex, inet_ntoa(client_socket_addr.sin_addr), ntohs(client_socket_addr.sin_port));
            }
        }

        for ( i = 0; i < MAX_CONNECTIONS; ++ i )
        {
            int clientSocketFD = client_socks[i];
            if ( FD_ISSET(clientSocketFD, &read_fd_set) )
            {
                ssize_t readBytes = read_sock(clientSocketFD, input_buffer, BUFFER_SIZE);

                if ( readBytes < 0 )
                {
                    dbg("[ERROR][TCP] An error occurred while sending message to the client %s:%d: %s\nThe connection is going to close.\n",
                            inet_ntoa(client_socket_addr.sin_addr), ntohs(client_socket_addr.sin_port), strerror(errno));
                    continue;
                }

                dbg("[INFO][TCP] Received a message from client %s:%d: %s\n",
                        inet_ntoa(client_socket_addr.sin_addr), ntohs(client_socket_addr.sin_port), input_buffer);

                if ( readBytes == 0 || strncmp("BYE", input_buffer, 3) == 0 )
                {
                    // Complete receiving message from client
                    close(clientSocketFD);
                    client_socks[i] = 0;
                    dbg("[INFO][TCP] Client %s:%d disconnected.-->%s\n",
                            inet_ntoa(client_socket_addr.sin_addr), ntohs(client_socket_addr.sin_port),input_buffer);

                    continue;
                }
                else
                {
                    cpayload *cli_payload = (cpayload*)malloc(sizeof (cpayload));
                    cli_payload->payload = input_buffer;
                    cli_payload->payload_len = readBytes;
                    cli_payload->cli_sock = clientSocketFD;
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

int registerNewSocket(int clientSocketFD, int* clientSocketFileDescriptors, int n) {
    int i = 0;
    for ( i = 0; i < n; ++ i ) {
        if ( clientSocketFileDescriptors[i] == 0 ) {
            clientSocketFileDescriptors[i] = clientSocketFD;

            return i;
        }
    }
    return -1;
}

