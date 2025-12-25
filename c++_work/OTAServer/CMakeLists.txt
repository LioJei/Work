set(CMAKE_TOOLCHAIN_FILE "D:/software/vcpkg-clion/scripts/buildsystems/vcpkg.cmake" CACHE STRING "" FORCE)
cmake_minimum_required(VERSION 4.0)
project(OTAServer)

set(CMAKE_CXX_STANDARD 17)

set(VCPKG_TARGET_TRIPLET "x64-mingw-dynamic")
find_package(OpenSSL CONFIG REQUIRED HINTS "D:/software/vcpkg-clion/installed/x64-mingw-dynamic/share/openssl")
find_package(cpprestsdk CONFIG REQUIRED)

# 添加源文件
aux_source_directory(. DIRS_SRCS)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src DIRS_SRCS)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_executable(${PROJECT_NAME} ${DIRS_SRCS})

target_link_libraries(${PROJECT_NAME} PRIVATE
        ${JSON_LIB}
        OpenSSL::SSL
        OpenSSL::Crypto
        cpprestsdk::cpprest
        cpprestsdk::cpprestsdk_zlib_internal
        cpprestsdk::cpprestsdk_brotli_internal
)
