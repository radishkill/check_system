set(CROSS_COMPILE $ENV{HOME}/gcc-6.3.1/bin/arm-linux-gnueabihf-)

set (CMAKE_CXX_COMPILER ${CROSS_COMPILE}g++)

set(CMAKE_BUILD_TYPE "Debug")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/test/arm)

include_directories(
  ${PROJECT_SOURCE_DIR}/src/gabor_im
  ./include
  ${PROJECT_SOURCE_DIR}/src
  $ENV{HOME}/boost-gcc-6.3/include
  $ENV{HOME}/opencv/include
)
link_directories (
  $ENV{HOME}/gcc-6.3.1/arm-linux-gnueabihf/libc/usr/lib
  $ENV{HOME}/opencv/lib
)

add_executable(TestImage_arm
    ../src/main.cpp
    ${PROJECT_SOURCE_DIR}/src/utils.cc
    ${PROJECT_SOURCE_DIR}/src/gabor_im/bluesteinSetup.cpp 
    ${PROJECT_SOURCE_DIR}/src/gabor_im/cosd.cpp 
    ${PROJECT_SOURCE_DIR}/src/gabor_im/createNormalizedFrequencyVector.cpp 
    ${PROJECT_SOURCE_DIR}/src/gabor_im/fft1.cpp 
    ${PROJECT_SOURCE_DIR}/src/gabor_im/fft2.cpp 
    ${PROJECT_SOURCE_DIR}/src/gabor_im/gabor_im.cpp 
    ${PROJECT_SOURCE_DIR}/src/gabor_im/gabor_im_data.cpp 
    ${PROJECT_SOURCE_DIR}/src/gabor_im/gabor_im_emxAPI.cpp 
    ${PROJECT_SOURCE_DIR}/src/gabor_im/gabor_im_emxutil.cpp 
    ${PROJECT_SOURCE_DIR}/src/gabor_im/gabor_im_initialize.cpp 
    ${PROJECT_SOURCE_DIR}/src/gabor_im/gabor_im_rtwutil.cpp 
    ${PROJECT_SOURCE_DIR}/src/gabor_im/gabor_im_terminate.cpp 
    ${PROJECT_SOURCE_DIR}/src/gabor_im/imgaborfilt.cpp 
    ${PROJECT_SOURCE_DIR}/src/gabor_im/ipermute.cpp 
    ${PROJECT_SOURCE_DIR}/src/gabor_im/padarray.cpp 
    ${PROJECT_SOURCE_DIR}/src/gabor_im/power.cpp 
    ${PROJECT_SOURCE_DIR}/src/gabor_im/rtGetInf.cpp 
    ${PROJECT_SOURCE_DIR}/src/gabor_im/rtGetNaN.cpp 
    ${PROJECT_SOURCE_DIR}/src/gabor_im/rt_nonfinite.cpp 
    ${PROJECT_SOURCE_DIR}/src/gabor_im/sind.cpp
    )

target_link_libraries(TestImage_arm
	pthread
  #  opencv_highgui
  opencv_core
  opencv_imgproc
  opencv_features2d
  opencv_imgcodecs
  opencv_calib3d
  opencv_xfeatures2d
  opencv_highgui
  gomp
	~/workspace/check_system/lib/x64/libCKCameraSDK.a
)
