cmake_minimum_required(VERSION 3.27)
project(TestDemo)

set(CMAKE_CXX_STANDARD 17)

aux_source_directory(. SRC)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src SRC)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc)

add_executable(${PROJECT_NAME} ${SRC})
