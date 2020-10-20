set(APP_NAME check_system)
set(CROSS_COMPILE /home/vm/gcc-6.3.1/bin/arm-linux-gnueabihf-)

#set (CMAKE_CXX_COMPILER "arm-linux-gnueabihf-g++")
#set (CMAKE_CXX_COMPILER "g++")
set (CMAKE_CXX_COMPILER ${CROSS_COMPILE}g++)


set(CMAKE_BUILD_TYPE "Debug")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/)

add_compile_options("-std=c++14")
include_directories(
	${PROJECT_SOURCE_DIR}/src/include
	)

add_definitions(
        -o3 -g -w -Wall
        )

# 指定生成目标

add_executable(CheckSystem
    main.cc
    usart.cc
    )

target_link_libraries(CheckSystem
	~/gcc-6.3.1/arm-linux-gnueabihf/libc/usr/lib/libpthread.so
	#	${CMAKE_THREAD_LIBS_INIT}
	~/workspace/check_system/lib/armhf/libCKCameraSDK.a
)
