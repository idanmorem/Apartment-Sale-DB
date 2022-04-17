cmake_minimum_required(VERSION 3.17)
project(project C)

set(CMAKE_C_STANDARD 90)

add_executable(project main.c DataBase.c DataBase.h FilesOp.c FilesOp.h main_commands.c main_commands.h Reconstraction.c Reconstraction.h)