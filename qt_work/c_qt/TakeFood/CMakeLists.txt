# cmake最低版本
cmake_minimum_required(VERSION 3.5)
# 工程名
project(TakeFood)

# c++标准
set(CMAKE_CXX_STANDARD 14)
# 开启Qt预处理的组件
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
#包含当前项目路径
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# 设置cmake模块查询目录
set(QT_PATH D:\\software\\qt\\5.14.1\\mingw73_64)
set(CMAKE_PREFIX_PATH ${QT_PATH})
# 查找QT模块
find_package(Qt5 COMPONENTS
        Core
        Gui
        Widgets
        Network
        REQUIRED)

# 添加源文件
aux_source_directory(. DIRS_SRCS)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src DIRS_SRCS)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/inc)

#编译选项句柄
set(WINDOWS_RELEASE 1)
if (WINDOWS_RELEASE)
    add_executable(${PROJECT_NAME} WIN32 ${DIRS_SRCS} version.rc)
else()
    add_executable(${PROJECT_NAME} ${DIRS_SRCS} version.rc)
endif ()

#链接库
target_link_libraries(${PROJECT_NAME}
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::Network
)

