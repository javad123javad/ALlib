#include <al.h>
#include <unity/unity.h>
#include <string.h>
#include <unistd.h>

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
    g_sock  = al_srv_open_sock(SOCK_UDP);
    TEST_ASSERT_NOT_EQUAL(-1, g_sock);
}

void test_srv_bind_sock(void)
{
    TEST_ASSERT_NOT_EQUAL(-1, al_srv_bind_sock(g_sock, NULL, 1368));
}

void test_srv_listen_sock()
{
    TEST_ASSERT_NOT_EQUAL(-1, al_srv_listen_sock(g_sock,4));
}

void test_srv_accept()
{
    struct sockaddr_in client = {0};
    g_cli = al_srv_accept_sock(g_sock, &client);
    TEST_ASSERT_NOT_EQUAL(-1, g_cli);
}

void test_cli_sendto()
{
    const char test_str[] = "Hello\n";
    //TODO: al_sendto should be tested.
    // TEST_ASSERT_EQUAL(strlen(test_str),
    //                   al_s(cli_sock, test_str, strlen(test_str))); // al_sendto
}

void test_cli_recvfrom()
{
    // TODO: al_recvfrom should be tested here
}


void test_cli_connect()
{
    // TODO: new connect function should be implemented for UDP
    // cli_sock = al_cli_connect("127.0.0.1", 1368);

    // TEST_ASSERT_NOT_EQUAL(-1, cli_sock);
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_cli_connect);
    RUN_TEST(test_cli_sendto);
    RUN_TEST(test_cli_recvfrom);
    close(g_cli);
    close(g_sock);
    return UNITY_END();
}
