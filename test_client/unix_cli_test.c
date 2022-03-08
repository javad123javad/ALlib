#include "al.h"
#include <unity/unity.h>
#include <string.h>
#include <unistd.h>

#define AL_UNIX_FILE        "/tmp/alunix.sock"

static int g_sock;
static int g_cli;
static int32_t cli_sock;

void setUp()
{
}

void tearDown()
{
}

void test_srv_open_sock(void)
{
    g_sock = al_srv_open_sock(SOCK_UNIX);
    TEST_ASSERT_NOT_EQUAL(-1, g_sock);
}

void test_srv_bind_sock(void)
{
    TEST_ASSERT_NOT_EQUAL(-1, al_srv_bind_sock(g_sock, NULL, 1368));
}

void test_srv_listen_sock()
{
    TEST_ASSERT_NOT_EQUAL(-1, al_srv_listen_sock(g_sock, 4));
}

void test_srv_accept()
{
    struct sockaddr_in client = {0};
    g_cli = al_srv_accept_sock(g_sock, &client);
    TEST_ASSERT_NOT_EQUAL(-1, g_cli);
}

void test_cli_write()
{
    const char test_str[] = "Hello\n";

    TEST_ASSERT_EQUAL(strlen(test_str),
                      al_write_sock(cli_sock, test_str, strlen(test_str)));
}

void test_cli_read()
{
    char read_buff[128] = {0};
    ssize_t read_len;

    al_write_sock(cli_sock, read_buff, 1);
    read_len = al_read_sock(cli_sock, read_buff, 1024);

    TEST_ASSERT_GREATER_THAN_INT32(0, read_len);
    fprintf(stderr, "Read:%s\n", read_buff);
}

void test_cli_connect()
{
    cli_sock = al_client_connect(AL_UNIX_FILE, 0, SOCK_UNIX);

    TEST_ASSERT_NOT_EQUAL(-1, cli_sock);
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_cli_connect);
    RUN_TEST(test_cli_write);
    RUN_TEST(test_cli_read);
    close(g_cli);
    close(g_sock);
    return UNITY_END();
}
