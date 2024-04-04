cmake_minimum_required(VERSION 3.20)

project(testPublic)

FILE(GLOB SRC "src/*.cpp"
              "${CMAKE_SOURCE_DIR}/public/src/*.cpp"
)

set(CMAKE_C_STANDARD 11)

add_compile_options(-Wall -g)

add_executable(${PROJECT_NAME} ${SRC})

target_include_directories(${PROJECT_NAME} PRIVATE
    ./include
    ${CMAKE_SOURCE_DIR}/public/include
)

target_link_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/lib)
target_link_libraries(${PROJECT_NAME} PRIVATE FreeFTPPublic)