#set(CROSS_COMPILE $ENV{HOME}/gcc-6.3.1/bin/arm-linux-gnueabihf-)

#set (CMAKE_CXX_COMPILER "arm-linux-gnueabihf-g++")
#set (CMAKE_CXX_COMPILER "g++")
set (CMAKE_CXX_COMPILER ${CROSS_COMPILE}g++)

set(CMAKE_BUILD_TYPE "Debug")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/)

add_compile_options("-std=c++14")
include_directories(
	${PROJECT_SOURCE_DIR}/src/include
  ${PROJECT_SOURCE_DIR}/src/gabor_im
  /opt/arm/include
	)
link_directories(
    /opt/arm/gcc-6.3.1/arm-linux-gnueabihf/libc/usr/lib
    /opt/arm/lib
    )

add_definitions(
        -o3 -g -w -Wall
        )

# 指定生成目标

add_executable(CheckSystem
    main.cc
    usart.cc
    global_arg.cc
    key_file.cc
    lcd.cc
    led.cc
    camera_manager.cc
    eventmanager.cc 
    laser.cc
    utils.cc
    state_machine.cc
    hostcontroller.cc
    authpic.cc
    gabor_im/bluesteinSetup.cpp 
    gabor_im/cosd.cpp 
    gabor_im/createNormalizedFrequencyVector.cpp 
    gabor_im/fft1.cpp 
    gabor_im/fft2.cpp 
    gabor_im/gabor_im.cpp 
    gabor_im/gabor_im_data.cpp 
    gabor_im/gabor_im_emxAPI.cpp 
    gabor_im/gabor_im_emxutil.cpp 
    gabor_im/gabor_im_initialize.cpp 
    gabor_im/gabor_im_rtwutil.cpp 
    gabor_im/gabor_im_terminate.cpp 
    gabor_im/imgaborfilt.cpp 
    gabor_im/ipermute.cpp 
    gabor_im/padarray.cpp 
    gabor_im/power.cpp 
    gabor_im/rtGetInf.cpp 
    gabor_im/rtGetNaN.cpp 
    gabor_im/rt_nonfinite.cpp 
    gabor_im/sind.cpp
    )

target_link_libraries(CheckSystem
  pthread
  boost_program_options
  opencv_core
  opencv_imgproc
  opencv_features2d
  opencv_imgcodecs
  opencv_calib3d
  opencv_xfeatures2d
  opencv_highgui
  gomp
	~/workspace/check_system/lib/armhf/libCKCameraSDK.a
)
