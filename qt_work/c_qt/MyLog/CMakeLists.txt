# cmake最低版本
cmake_minimum_required(VERSION 3.15)
# 工程名
project(Test)

# c++标准
set(CMAKE_CXX_STANDARD 14)
# 开启Qt预处理的组件
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
#包含当前项目路径
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# 设置cmake模块查询目录
set(CMAKE_PREFIX_PATH L:/workapp/QtCreator/5.15.2/mingw81_64)
#SET(CMAKE_PREFIX_PATH /opt/Qt/5.15.2/gcc_64/lib/cmake)
# 查找QT模块
find_package(Qt5 COMPONENTS
        Core
        Gui
        Widgets
        TextToSpeech
        REQUIRED)
# 添加源文件
aux_source_directory(. DIRS_SRCS)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/Interface DIRS_SRCS)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/MyLog DIRS_SRCS)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Interface
        ${CMAKE_CURRENT_SOURCE_DIR}/MyLog
        )

add_executable(${PROJECT_NAME} ${DIRS_SRCS})
# 添加模块
target_link_libraries(${PROJECT_NAME}
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::TextToSpeech
        )

