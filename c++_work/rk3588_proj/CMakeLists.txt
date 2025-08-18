cmake_minimum_required(VERSION 3.5)
project(rk3588_proj)

set(CMAKE_CXX_STANDARD 11)

# 添加源文件
aux_source_directory(. DIRS_SRCS)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src DIRS_SRCS)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/inc)

# 库包配置
find_package(Threads REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(DRM REQUIRED libdrm)
pkg_check_modules(AVCODEC REQUIRED libavcodec)
pkg_check_modules(AVFORMAT REQUIRED libavformat)
pkg_check_modules(AVUTIL REQUIRED libavutil)
#pkg_check_modules(AVFILTER REQUIRED libavfilter)
pkg_check_modules(SWSCALE REQUIRED libswscale)

# 添加链接库
#set(MPP_INCLUDE_DIR "/usr/local/include/rockchip")
#set(MPP_LIBRARY "/usr/local/lib/librockchip_mpp.so")
link_directories(/usr/local/lib)

set(EXECUTABLE 1)
if (EXECUTABLE)
    add_executable(${PROJECT_NAME}  ${DIRS_SRCS})
    set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
else()
    add_library(${PROJECT_NAME}  SHARED ${DIRS_SRCS})
    set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
endif ()

target_include_directories(${PROJECT_NAME} PRIVATE
        ${DRM_INCLUDE_DIRS}
        ${AVCODEC_INCLUDE_DIRS}
        ${AVFORMAT_INCLUDE_DIRS}
        ${AVUTIL_INCLUDE_DIRS}
        ${AVFILTER_INCLUDE_DIRS}
        ${SWSCALE_INCLUDE_DIRS}
        ${FFMPEG_INC_DIR}
#        ${MPP_INCLUDE_DIR}
)
target_link_libraries(${PROJECT_NAME}
#        ${MPP_LIBRARY}
        avcodec avformat avutil swresample
        Threads::Threads
        ${DRM_LIBRARIES}
        ${AVCODEC_LIBRARIES}
        ${AVFORMAT_LIBRARIES}
        ${AVUTIL_LIBRARIES}
        ${AVFILTER_LIBRARIES}
        ${SWSCALE_LIBRARIES}
        ${DRM_LIBRARIES}
)
