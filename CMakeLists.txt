cmake_minimum_required(VERSION 3.20)
project(FreeFTP)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
add_subdirectory(client)
add_subdirectory(server)