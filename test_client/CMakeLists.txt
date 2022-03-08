
cmake_minimum_required(VERSION 3.5)
ADD_EXECUTABLE( allib_udp_cli_test udp_cli_test.c
    ../src/al.c
    ../include/al.h
    )

ADD_EXECUTABLE( allib_tcp_cli_test tcp_cli_test.c
../src/al.c
../include/al.h
)

ADD_EXECUTABLE( allib_unix_cli_test unix_cli_test.c
../src/al.c
../include/al.h
)

ADD_TEST(Allib_udp_cli_test allib_udp_cli_test)
ADD_TEST(Allib_tcp_cli_test allib_tcp_cli_test)
ADD_TEST(Allib_unix_cli_test allib_tcp_cli_test)

target_link_libraries(allib_udp_cli_test unity)
target_link_libraries(allib_tcp_cli_test unity)
target_link_libraries(allib_unix_cli_test unity)
