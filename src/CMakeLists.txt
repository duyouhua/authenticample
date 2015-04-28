cmake_minimum_required(VERSION 2.8)

project(openssl.test)

add_definitions(-Wall -O0 -g)

add_subdirectory(server)
add_subdirectory(client)
