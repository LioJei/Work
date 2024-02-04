# cmake最低版本
cmake_minimum_required(VERSION 3.15)
# 工程名
project(PadQtDemo)

# c++标准
set(CMAKE_CXX_STANDARD 14)
# 开启Qt预处理的组件
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
#包含当前项目路径
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# 设置cmake模块查询目录
# set(QT_PATH D:\\14621\\software\\Qt5_14_1\\5.14.1\\mingw73_64)
set(QT_PATH /usr/lib/aarch64-linux-gnu/qt5)
set(CMAKE_PREFIX_PATH ${QT_PATH})
# 查找QT模块
find_package(Qt5 COMPONENTS
        Core
        Gui
        Widgets
        TextToSpeech
        Quick
        OpenGL
        REQUIRED)
# 添加OpenXX模块
find_package(OpenGL REQUIRED)
find_package(OpenAL REQUIRED)
find_package(OpenCV REQUIRED)
find_package(GLUT REQUIRED)
find_package(ALSA REQUIRED)
# 添加源文件
aux_source_directory(. DIRS_SRCS)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src DIRS_SRCS)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/inc)

link_directories(/usr/local/lib)

set(WINDOWS_RELEASE 0)
if (WINDOWS_RELEASE)
    add_executable(${PROJECT_NAME} WIN32 ${DIRS_SRCS})
else()
    add_executable(${PROJECT_NAME} ${DIRS_SRCS})
endif ()

include_directories(${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} -lopenal -lalut)
target_link_libraries(${PROJECT_NAME}
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::TextToSpeech
        Qt5::Quick
        Qt5::OpenGL
        opencv_core
        opencv_imgproc
        opencv_highgui
        )

