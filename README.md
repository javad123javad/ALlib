# ALlib

ALlib is a lightweight socket library for embedded systems written in C. By using the multiplexing mechanism, it's possible to use callbacks in applications beside the traditional polling mechanism.
## Compiling and Installation

ALlib is based on Cmake. It's possible to make `ALlib` both as static and shared library.
If you need test functions, uncomment below two lines in `CmakeLists.txt`
```cmake
#add_subdirectory( test_server )
#add_subdirectory(test_client)
```
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
* Server Examples
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
    int fret = 0, len;
    char msg[BUF_LEN] = {0}, buf[]="BYE\r";
    struct sockaddr_in clieaddr;
    clieaddr.sin_family = AF_INET;
    clieaddr.sin_port = htons(35000);
    clieaddr.sin_addr.s_addr = inet_addr("192.168.1.12");
    len = sizeof(clieaddr);
    g_sock = al_srv_open_sock(SOCK_UDP);
    fret = al_srv_bind_sock(g_sock, NULL, 35000);

    if( -1 != fret)
    {
       
        while (1)
        {
            fret = al_recvfrom(g_sock,msg, 1024, (struct sockaddr *)&clieaddr, 0);
            fprintf(stderr, "fret:%d, MSG:%s, IP:%s\n", fret, msg, inet_ntoa(clieaddr.sin_addr));

            al_sendto(g_sock, buf, strlen(buf), &clieaddr,0);
            fprintf(stderr, "fret:%d\n", fret);
            bzero(msg, strlen(msg));
        }
    }
}
```
### UNIX Domain Multiplexed Server Example
```c
#include <al.h>
#include <iostream>
#include <thread>
#define AL_UNIX_FILE        "/tmp/alunix.sock"

using namespace std;

void server_cb(cpayload *payload)
{
    fprintf(stderr,"Callback: %s\n", payload->payload);
    al_write_sock(payload->cli_sock, payload->payload, payload->payload_len);
}
int main()
{
    int fret = 0;
    int server_sock;
    server_sock = al_srv_open_sock(SOCK_UNIX);
    fret = al_unix_srv_bind(server_sock, AL_UNIX_FILE);
    if(fret < 0)
    {
        exit(-1);
    }
    fret = al_srv_listen_sock(server_sock,4);
    if(-1 == fret)
    {
        perror("Linsten");
        exit(-1);
    }
    fret = al_srv_serve_reqs(server_sock,serv_cb);
   
    return 0;
}
```
* Client Examples
### TCP Client Example
```c
#include <al.h>
#include <stdio.h>
#include <unistd.h>

#define BUF_LEN     1024
#define SERVER_PORT 1368

int main()
{
    int client_sock;
    char buf[BUF_LEN] = {0};
    client_sock = al_client_connect("127.0.0.1",SERVER_PORT);
    if(client_sock < 0)
    {
        exit(EXIT_FAILURE);
    }
    al_write_sock(client_sock,"Hello\n",6);
    al_read_sock(client_sock, buf, BUF_LEN);
    printf("Recv:%s\n", buf);
    return 0;
    }
}
```
### UDP Client Example
As the UDP is changing constantly, the examples also changes as well.
```c
#include <al.h>
#include <stdio.h>
#include <unistd.h>

#define BUF_LEN     1024
#define SERVER_PORT 1368

int main()
{
    int client_sock;
    char buf[BUF_LEN] = {0};
    const char test_str[] = "Hello\n";
    struct sockaddr_in cliaddr = {0};    
    cliaddr.sin_family = AF_INET;
    cliaddr.sin_addr.s_addr   = inet_addr("127.0.0.1");
    cliaddr.sin_port   = htons(SERVER_PORT);
    client_sock = al_srv_open_sock(SOCK_UDP);

    if(client_sock < 0)
    {
        exit(EXIT_FAILURE);
    }

    al_sendto(client_sock, test_str, strlen(test_str), &cliaddr, 0);
    al_recvfrom(client_sock, buf, BUF_LEN, &cliaddr, 0);
    printf("Recv:%s\n", buf);
    return 0;
}

```
### UNIX Domain Client Example
```c
#include <al.h>
#include <stdio.h>
#include <unistd.h>

#define BUF_LEN     1024
#define AL_UNIX_FILE        "/tmp/alunix.sock"

int main()
{
    int client_sock;
    char buf[BUF_LEN] = {0};
    cli_sock = al_client_connect(AL_UNIX_FILE, 0, SOCK_UNIX);
    if(client_sock < 0)
    {
        exit(EXIT_FAILURE);
    }
    al_write_sock(client_sock,"Hello\n",6);
    al_read_sock(client_sock, buf, BUF_LEN);
    printf("Recv:%s\n", buf);
    return 0;
    }
}
```
## Contributing
Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.


## License
[MIT](https://choosealicense.com/licenses/mit/)