project(syslog_parser)
cmake_minimum_required(VERSION 3.10)

if (EXISTS ${CMAKE_SOURCE_DIR}/conanbuildinfo.cmake)
    include(${CMAKE_SOURCE_DIR}/conanbuildinfo.cmake)
    conan_basic_setup()
endif()

SET(CMAKE_CXX_STANDARD 17)

SET( HEADERS include/grammar.hpp )

include_directories(include/)

add_subdirectory(tests)
enable_testing()
