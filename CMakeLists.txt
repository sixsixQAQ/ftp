cmake_minimum_required(VERSION 3.1)

project(ftp)

aux_source_directory(. SRC)

set(CMAKE_C_STANDARD 11)
add_compile_options(-Wall)

add_executable(main ${SRC})