# ALlib

ALlib is a lightweight socket library for embedded systems written in C. By using the multiplexing mechanism, it's possible to use callbacks in applications beside the traditional polling mechanism.
## Installation

ALlib is based on Cmake, so just building and installing ALlib is as simple as follows:
```bash
$ git clone git@github.com:javad123javad/ALlib.git
$ mkdir build
$ cd build
$ cmake ..
$ make
$ sudo make install
```
## Usage
Include `al.h` header file in you project and enjoy it ;)
## Examples
### Server Polling Example
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
    srv_socket = srv_open_sock();
    ret = srv_bind_sock(srv_socket, NULL,1368);
    if(ret < 0)
    {
        fprintf(stderr,"error in srv_bind_sock:%d\n", ret);
        exit(EXIT_FAILURE);
    }
    ret = srv_listen_sock(srv_socket, 4);
    if(ret < 0)
    {
        fprintf(stderr,"error in srv_listen_sock:%d\n", ret);

        exit(EXIT_FAILURE);
    }
    while (1) {
        client_socket = srv_accept(srv_socket, &client);
        write_sock(client_socket,"Hello\n",6);
        close(client_socket);
    }

    return 0;
} 
```
### Multithread Server Example
```c++
#include <al.h>
#include <iostream>
#include <thread>
using namespace std;

void server_cb(cpayload *payload)
{
        fprintf(stderr, "Client IP: %s\n", inet_ntoa(payload->cli_info.sin_addr));
        write_sock(payload->cli_sock, "Hello\n", 6);

}
int main()
{
    int ret = 0;
    int server_sock;
    server_sock = srv_open_sock();
    ret = srv_bind_sock(server_sock, nullptr, 1989);
    if(ret < 0)
    {
        return -1;
    }
    ret = srv_listen_sock(server_sock, 4);
    thread server_thr(srv_serve_reqs, server_sock, server_cb);
    server_thr.detach();
    while (1) {

    }
    return 0;
}
```
### Client Example
```c
#include <al.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int client_sock;
    char buf[1024] = {0};
    client_sock = cli_connect("127.0.0.1",1368);
    if(client_sock < 0)
    {
        exit(EXIT_FAILURE);
    }
    write_sock(client_sock,"Hello\n",6);
    read_sock(client_sock, buf, 1024);
    printf("Recv:%s\n", buf);
    return 0;
}
```


## Contributing
Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

Please make sure to update tests as appropriate.

## License
[MIT](https://choosealicense.com/licenses/mit/)