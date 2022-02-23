
cmake_minimum_required(VERSION 3.5)
ADD_EXECUTABLE( allib_server_test main_test.c
    ../src/al.c
    ../include/al.h
    )

ADD_TEST(ALlib_server_test allib_server_test)
target_link_libraries(allib_server_test unity mnl)
