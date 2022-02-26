# ALlib

ALlib is a lightweight socket library for embedded systems written in C. By using the multiplexing mechanism, it's possible to use callbacks in applications beside the traditional polling mechanism.
## Compiling and Installation

ALlib is based on Cmake. It's possible to make `ALlib` both as static and shared library.
### Static library
```bash
$ git clone git@github.com:javad123javad/ALlib.git
$ mkdir build
$ cd build
$ cmake -DBUILD_SHARED_LIBS=ON ..
$ make
$ sudo make install
```

### Shared library
To build the `ALlib` as shared library, the `-DBUILD_SHARED_LIBS=ON` should passed to the cmake:
```bash
$ git clone git@github.com:javad123javad/ALlib.git
$ mkdir build
$ cd build
$ cmake ..
$ make
$ sudo make install
```
## Usage
Include `al.h` header file in you project and link it with AL library. A simple `cmake` configuration is depicted below:
* For static version of the library:
```cmake
cmake_minimum_required(VERSION 3.5)

project(AL_test LANGUAGES C)

add_executable(AL_test main.c)
target_link_libraries(AL_test libal.a)
```
* For the shared library:
```bash
cmake_minimum_required(VERSION 3.5)

project(AL_test LANGUAGES C)

add_executable(AL_test main.c)
target_link_libraries(AL_test al)
```
## Examples
### TCP Server Polling Example
```c
#include <al.h>
#include <stdio.h>
#include <unistd.h>
int main()
{
    int srv_socket;
    int client_socket;
    struct sockaddr_in client = {0};
    int ret = 0;
    srv_socket = al_srv_open_sock();
    ret = al_srv_bind_sock(srv_socket, NULL,1368);
    if(ret < 0)
    {
        fprintf(stderr,"error in srv_bind_sock:%d\n", ret);
        exit(EXIT_FAILURE);
    }
    ret = al_srv_listen_sock(srv_socket, 4);
    if(ret < 0)
    {
        fprintf(stderr,"error in srv_listen_sock:%d\n", ret);

        exit(EXIT_FAILURE);
    }
    while (1) {
        client_socket = al_srv_accept_sock(srv_socket, &client);
        al_write_sock(client_socket,"Hello\n",6);
        al_close_sock(client_socket);
    }
    return 0;
} 
```
### Multithread TCP Server Example
```c
#include <al.h>
#include <iostream>
#include <thread>
using namespace std;

void server_cb(cpayload *payload)
{
        fprintf(stderr, "Client IP: %s\n", inet_ntoa(payload->cli_info.sin_addr));
        al_write_sock(payload->cli_sock, "Hello\n", 6);

}
int main()
{
    int ret = 0;
    int server_sock;
    server_sock = al_srv_open_sock();
    ret = al_srv_bind_sock(server_sock, nullptr, 1989);
    if(ret < 0)
    {
        return -1;
    }
    ret = al_srv_listen_sock(server_sock, 4);
    thread server_thr(al_srv_serve_reqs, server_sock, server_cb);
    server_thr.detach();
    while (1) {

    }
    return 0;
}
```
### UDP Server Example
```c
#include <al.h>
#include <stdio.h>
#include <unistd.h>

#define BUF_LEN     1024

int main(void)
{
    int32_t fret = 0, len;
    char msg[BUF_LEN] = {0};
    struct cli_addr;

    int udp_sock  = al_srv_open_sock(SOCK_UDP);
    fret = al_srv_bind_sock(udp_sock, NULL, 35000);
    while (1)
    {
        fret = recvfrom(udp_sock, msg, BUF_LEN, 0, (struct sockaddr *)&cli_addr, &len);
        fprintf(stdout, "MSG:%s\n", msg);
        bzero(msg, strlen(msg));
    }
}
```
### Client Example
```c
#include <al.h>
#include <stdio.h>
#include <unistd.h>

int client_sock;
    char buf[1024] = {0};
    client_sock = al_client_connect("127.0.0.1",1368);
    if(client_sock < 0)
    {
        exit(EXIT_FAILURE);
    }
    al_write_sock(client_sock,"Hello\n",6);
    al_read_sock(client_sock, buf, 1024);
    printf("Recv:%s\n", buf);
    return 0;
}
```

## Contributing
Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.


## License
[MIT](https://choosealicense.com/licenses/mit/)