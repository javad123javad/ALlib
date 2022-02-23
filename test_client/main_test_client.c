#include <unity/unity.h>
#include "../src/imp_daemon_server.h"
#include "../src/imp_daemon_proto.h"
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
    g_sock  = srv_open_sock();
    TEST_ASSERT_NOT_EQUAL(-1, g_sock);
}

void test_srv_bind_sock(void)
{
    TEST_ASSERT_NOT_EQUAL(-1, srv_bind_sock(g_sock, 1368));
}

void test_srv_listen_sock()
{
    TEST_ASSERT_NOT_EQUAL(-1, srv_listen_sock(g_sock,4));
}

void test_srv_accept()
{
    struct sockaddr_in client = {0};
    g_cli = srv_accept(g_sock, &client);
    TEST_ASSERT_NOT_EQUAL(-1, g_cli);
}

void test_cli_write()
{
    const char test_str[] = "Hello\n";

    TEST_ASSERT_EQUAL(strlen(test_str),
                      write_sock(cli_sock, test_str, strlen(test_str)));
}

void test_proto_unpack(const uint8_t *raw_buf, const ssize_t buf_len)
{
    imp_dp_t recv_pack = {0};
    uint8_t payload[128] = {0};
    recv_pack.payload = payload;
    TEST_ASSERT_NOT_EQUAL(-1, proto_unpack(raw_buf, buf_len, &recv_pack));
    TEST_ASSERT_EQUAL(recv_pack.STX, 0x02);
    TEST_ASSERT_EQUAL(recv_pack.ver, 0x01);
    TEST_ASSERT_EQUAL(recv_pack.req, 0xA1);
    fprintf(stderr,"Payload:%s", recv_pack.payload);

    TEST_ASSERT_EQUAL(recv_pack.ETX, 0x03);

}
void test_cli_read()
{
    char read_buff[128] = {0};
    ssize_t read_len;
    while(1)
    {
        write_sock(cli_sock,read_buff,1);
        read_len = read(cli_sock, read_buff, 1024 );

        fprintf(stderr,"Read:%s\n", read_buff);
        test_proto_unpack((const uint8_t*)read_buff, read_len );
//        if(strncmp(read_buff,"bye",3)==0)
//            break;
        sleep(1);

    }

    //    TEST_ASSERT_NOT_EQUAL(-1, read_len);
}


void test_cli_connect()
{
    cli_sock = cli_connect("127.0.0.1", 1368);

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
