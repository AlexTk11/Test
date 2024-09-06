cmake_minimum_required(VERSION 3.10)
project(ClientServerApp)

set(CMAKE_C_STANDARD 11)

add_executable(server Server.c)
add_executable(client Client.c)