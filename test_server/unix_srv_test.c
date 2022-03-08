#include "al.h"
#include <unity/unity.h>
#include <string.h>
#include <unistd.h>

int g_sock;
static int g_cli;
#define AL_UNIX_FILE        "/tmp/alunix.sock"

void setUp()
{

}

void tearDown()
{

}

void test_al_srv_open_sock(void)
{
    g_sock  = al_srv_open_sock(SOCK_UNIX);
    TEST_ASSERT_GREATER_THAN_INT32(0, g_sock);
}

void test_al_unix_srv_bind(void)
{

    TEST_ASSERT_GREATER_OR_EQUAL_INT32(0, al_unix_srv_bind(g_sock, AL_UNIX_FILE));
    
}

void test_srv_listen_sock()
{
    TEST_ASSERT_GREATER_OR_EQUAL_INT32(0, al_srv_listen_sock(g_sock,4));
}

void test_srv_accept()
{
    struct sockaddr_in client = {0};
    g_cli = al_srv_accept_sock(g_sock, &client);
    TEST_ASSERT_GREATER_OR_EQUAL_INT32(0, g_cli);
}

void test_cli_write()
{
    const char test_str[] = "Hello\n";

    TEST_ASSERT_EQUAL(strlen(test_str),
                      al_write_sock(g_cli, test_str, strlen(test_str)));
}

void test_cli_read()
{
    char read_buff[128] = {0};
    ssize_t read_len;
    {
        read_len = al_read_sock(g_cli, read_buff, 10 );
        printf("Read:%s\n", read_buff);
    }
}

void serv_cb(cpayload * payload)
{
    
    fprintf(stderr,"Callback: %s\n", payload->payload);
    al_write_sock(payload->cli_sock, payload->payload, payload->payload_len);

}
void test_srv_accepts()
{
    int fret = 0;
    fret = al_srv_serve_reqs(g_sock,serv_cb);
    TEST_ASSERT_NOT_EQUAL(-1, fret);
}
int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_al_srv_open_sock);
    RUN_TEST(test_al_unix_srv_bind);
    RUN_TEST(test_srv_listen_sock);
    RUN_TEST(test_srv_accepts);
    // RUN_TEST(test_srv_accept);
    // RUN_TEST(test_cli_read);
    // RUN_TEST(test_cli_write);
    close(g_cli);
    close(g_sock);
    return UNITY_END();
}
