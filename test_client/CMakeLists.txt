
cmake_minimum_required(VERSION 3.5)
ADD_EXECUTABLE( allib_client_test main_test_client.c
    ../src/al.c
    ../include/al.h
    )
ADD_TEST(  ALlib_client_test allib_server_test    )
target_link_libraries(allib_client_test unity mnl)
