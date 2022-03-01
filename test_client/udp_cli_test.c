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

void test_cli_open_sock(void)
{
    g_sock  = al_srv_open_sock(SOCK_UDP);
    TEST_ASSERT_NOT_EQUAL(-1, g_sock);
}

void test_cli_sendto()
{
    const char test_str[] = "Hello\n";
    struct sockaddr_in cliaddr;
    char buf[BUFFER_SIZE] = {0};
    ssize_t snd_len;
    cliaddr.sin_family = AF_INET;
    cliaddr.sin_addr.s_addr   = inet_addr("192.168.1.12");
    cliaddr.sin_port   = htons(35000);
    snd_len = al_sendto(g_sock, test_str, strlen(test_str), &cliaddr, 0);
    TEST_ASSERT_NOT_EQUAL(-1, snd_len);

}

void test_cli_recvfrom()
{
    // TODO: al_recvfrom should be tested here
    struct sockaddr cliaddr;
    char buf[BUFFER_SIZE] = {0};
    ssize_t rcv_len;
    rcv_len = al_recvfrom(g_sock, buf, BUFFER_SIZE, &cliaddr, 0);
    TEST_ASSERT_NOT_EQUAL(-1, rcv_len);
    fprintf(stderr,"RECV:%s\n", buf);
}


void test_cli_connect()
{
    // TODO: new connect function should be implemented for UDP
    g_sock = al_client_connect("192.168.1.12", 35000, SOCK_UDP);

    TEST_ASSERT_NOT_EQUAL(-1, g_sock);
}

int main(void)
{
    UNITY_BEGIN();

    // RUN_TEST(test_cli_open_sock);
    RUN_TEST(test_cli_connect);
    RUN_TEST(test_cli_sendto);
    RUN_TEST(test_cli_recvfrom);
    close(g_cli);
    close(g_sock);
    return UNITY_END();
}
