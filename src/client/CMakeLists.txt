cmake_minimum_required(VERSION 2.8)

project(openssl.test)

execute_process(COMMAND curl-config --cflags OUTPUT_VARIABLE CURL_CFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND curl-config --libs OUTPUT_VARIABLE CURL_LIBS OUTPUT_STRIP_TRAILING_WHITESPACE)

add_definitions(${CURL_CFLAGS})

add_executable(client main.cpp)
target_link_libraries(client ACE ${CURL_LIBS})
