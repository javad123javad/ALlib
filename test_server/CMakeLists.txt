
cmake_minimum_required(VERSION 3.5)
ADD_EXECUTABLE( allib_srv_test_tcp tcp_srv_test.c
    ../src/al.c
    ../include/al.h
    )

ADD_EXECUTABLE( allib_srv_test_udp udp_srv_test.c
    ../src/al.c
    ../include/al.h
    )

ADD_TEST(Allib_srv_test_tcp allib_srv_test_tcp)
ADD_TEST(Allib_srv_test_udp allib_srv_test_udp)

target_link_libraries(allib_srv_test_tcp unity )
target_link_libraries(allib_srv_test_udp unity )
