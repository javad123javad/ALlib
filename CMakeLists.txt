cmake_minimum_required(VERSION 3.5)
project(AL VERSION 1.0.1 DESCRIPTION "AL socket library" LANGUAGES C)
include(GNUInstallDirs)
# Enable Testing
ENABLE_TESTING()
# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)
add_subdirectory( test_server )
# add_subdirectory(test_client)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic")

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS YES CACHE BOOL "Export all symbols")

add_library(al src/al.c)

set_target_properties(al PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    PUBLIC_HEADER include/al.h)
configure_file(allib.pc.in allib.pc @ONLY)

target_include_directories(al PRIVATE include)
target_include_directories(al PRIVATE src)

install(TARGETS al
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_BINARY_DIR}
)
install(FILES ${CMAKE_BINARY_DIR}/allib.pc DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)

#add_executable(AL src/main.c src/al.c src/al.h)
