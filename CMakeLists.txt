project(syslog_parser)
cmake_minimum_required(VERSION 3.10)

if (EXISTS conanbuildinfo.cmake)
    include(conanbuildinfo.cmake)
    conan_basic_setup()
endif()

SET(CMAKE_CXX_STANDARD 17)

SET( HEADERS include/grammar.hpp )

include_directories(include/)

add_subdirectory(tests)
enable_testing()
