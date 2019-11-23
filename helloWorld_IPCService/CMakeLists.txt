cmake_minimum_required(VERSION 3.1.1)
project(helloWorld_IPCService)



#设置C++编译器的编译选项
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -g -pthread -std=c++11")
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -g -pthread -std=c++11")

set(LINUX_STANDARD 1)
#SDK集成编译的环境的路径
set(SYSROOT_DIR "/home/ubuntu/Desktop/AS23/Crosstool/ql-ol-crosstool/sysroots/armv7a-vfp-neon-oe-linux-gnueabi")

#编译选项，头文件
if(LINUX_STANDARD)
#这个是桌面端
    message("linux compile")
    include_directories(/usr/include/)
    include_directories(/usr/include/glib-2.0/)
    include_directories(/usr/lib/x86_64-linux-gnu/glib-2.0/include/)
    include_directories(/usr/include/gio-unix-2.0/)
else()
#设置的交叉编译环境
    message("arm cross compile")
    include_directories(${SYSROOT_DIR}/usr/include/)
    include_directories(${SYSROOT_DIR}/usr/include/glib-2.0/)
    include_directories(${SYSROOT_DIR}/usr/include/confmmap/)
    include_directories(${SYSROOT_DIR}/usr/lib/x86_64-linux-gnu/glib-2.0/include/)
    include_directories(${SYSROOT_DIR}/usr/include/gio-unix-2.0/)
    include_directories(${SYSROOT_DIR}/usr/lib/glib-2.0/include/)
endif(LINUX_STANDARD)


#包含源文件,法1
#依次包含源文件
set(SOURCES "src/main.cpp"
             "src/test1.cpp"
             "src/threadpoolTest.cpp")


##包含源文件,法2
##cmake之前先进行make clean
##SOURCES 包含源文件, 形成的是一个源文件列表
#file(GLOB SOURCES "src/*.cpp")

#头文件
file(GLOB HEADQT "inc/QtCore/*.h")
#使用到的头文件
set(HEAD_FILES
        inc/test1.h
        inc/qtstl_global.h
        inc/qtstl.h
        inc/ThreadPool.h
        inc/threadpooltest.h)

#包含头文件
include_directories(./inc)
include_directories(./inc/QtCore)
include_directories(./inc/QtSerialPort)

#链接的库路径
if(LINUX_STANDARD)
    link_directories(${CMAKE_SOURCE_DIR}/lib)
    link_directories(${CMAKE_SOURCE_DIR}/lib2)
else()
    link_directories(${CMAKE_SOURCE_DIR}/lib-arm) #在arm上跑不起来
endif(LINUX_STANDARD)


#设置工程的组成架构，${HEAD_FILES} 这个字段有和没有是有区别的
add_executable(helloWorld_IPCService  ${HEAD_FILES} ${HEADQT} ${SOURCES} )


#链接的库,这边需要写到add_executable后面
target_link_libraries(helloWorld_IPCService Qt5Core Qt5SerialPort)







