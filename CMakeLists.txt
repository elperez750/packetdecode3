cmake_minimum_required(VERSION 4.3)
project(packetdecode3 C)

set(CMAKE_C_STANDARD 17)

add_library(packetdecode3 STATIC library.c)
