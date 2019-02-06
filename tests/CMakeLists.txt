project(tests)
cmake_minimum_required(VERSION 3.10)

SET(CMAKE_CXX_STANDARD 17)

SET(CMAKE_BUILD_TYPE Debug)
ADD_DEFINITIONS(-O0)
ADD_DEFINITIONS(-fsanitize=address)
ADD_DEFINITIONS(-fno-omit-frame-pointer)

SET( HEADERS
    ../include/grammar.hpp
    ../include/message.hpp
    include/test.hpp
)

include_directories(include/)

SET( SOURCES sources/main.cpp )

ADD_EXECUTABLE(${PROJECT_NAME} ${HEADERS} ${SOURCES})

target_link_libraries(${PROJECT_NAME} -lasan -lpthread)

if(COMMAND conan_target_link_libraries)
    conan_target_link_libraries(${PROJECT_NAME})
endif()

enable_testing()

add_test(NAME basic COMMAND  ${PROJECT_NAME} --reporter junit --out ${CMAKE_SOURCE_DIR}/report.xml )
