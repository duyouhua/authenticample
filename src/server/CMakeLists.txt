cmake_minimum_required(VERSION 2.8)

project(openssl.test)

add_definitions(-DNS_ENABLE_SSL)
include_directories(../../mongoose)

add_executable(server main.cpp ../../mongoose/mongoose.c)
target_link_libraries(server ACE ssl)

