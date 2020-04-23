cmake_minimum_required(VERSION 3.8)

project(libcStackTest C)

add_executable(libcStackTest main.c ../stack.c)

add_custom_command(TARGET libcStackTest COMMAND ./libcStackTest POST_BUILD)