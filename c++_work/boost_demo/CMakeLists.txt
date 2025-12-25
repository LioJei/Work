# cmake最低版本
cmake_minimum_required(VERSION 3.10)
# 工程名
project(BoostTest)

# c++标准
set(CMAKE_CXX_STANDARD 17)
# 开启Qt预处理的组件
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
#包含当前项目路径
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# 设置cmake模块查询目录
#set(QT_PATH D:\\software\\qt\\5.15.2\\mingw81_64)
set(QT_PATH /home/rpdzkj/software/qt5_15_15)
set(CMAKE_PREFIX_PATH ${QT_PATH})
# 指定 Boost 安装根目录（如果 CMake 未自动检测到）
set(BOOST_ROOT "/usr/local")  # 关键：手动指定 Boost 路径
# 查找 Boost 库（必须指定组件，例如 filesystem、system）
find_package(Boost 1.77.0 REQUIRED COMPONENTS
        filesystem
        system
        thread
        chrono
        timer
        unit_test_framework
)
#message(STATUS "Boost found version: ${Boost_VERSION}")
#message(STATUS "Boost include dirs: ${Boost_INCLUDE_DIRS}")
#message(STATUS "Boost libraries: ${Boost_LIBRARIES}")
# 查找QT模块
find_package(Qt5 COMPONENTS
        Core
        Gui
        Widgets
        TextToSpeech
        Quick
        #        OpenGL
        REQUIRED)
# 添加OpenXX模块
#find_package(OpenGL REQUIRED)
#find_package(OpenAL REQUIRED)
#find_package(OpenCV REQUIRED)
#find_package(GLUT REQUIRED)
#find_package(ALSA REQUIRED)
# 添加源文件
aux_source_directory(. DIRS_SRCS)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src DIRS_SRCS)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/inc)

set(RELEASE_HANDLE 1)
if (RELEASE_HANDLE)
    add_executable(${PROJECT_NAME}  ${DIRS_SRCS})
else()
    add_library(${PROJECT_NAME} SHARED ${DIRS_SRCS})
endif ()

#include_directories(${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS})
#target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} -lopenal -lalut)
target_link_libraries(${PROJECT_NAME}
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::TextToSpeech
        Qt5::Quick
        Boost::headers
        Boost::system
        Boost::filesystem
        Boost::thread
        Boost::chrono
        Boost::timer
        Boost::unit_test_framework
        #        Qt5::OpenGL
        #        opencv_core
        #        opencv_imgproc
        #        opencv_highgui
)

