cmake_minimum_required(VERSION 3.5)
project(cv_demo)

set(CMAKE_CXX_STANDARD 11)
set(OpenCV_DIR "/usr/local/lib/cmake/opencv4")
#list(APPEND CMAKE_PREFIX_PATH "/usr/local")
find_package(OpenCV REQUIRED)

aux_source_directory(. SRC)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src SRC)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc)

add_executable(${PROJECT_NAME} ${SRC})
target_include_directories(${PROJECT_NAME} PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})