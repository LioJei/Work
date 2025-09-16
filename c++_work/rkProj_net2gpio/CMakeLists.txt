cmake_minimum_required(VERSION 3.5.1)
project(rkProj)
set(CMAKE_CXX_STANDARD 11)

find_package(Threads)
aux_source_directory(. SRC)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src SRC)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc)

add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} Threads::Threads)
