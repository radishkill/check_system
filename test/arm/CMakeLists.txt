set (CMAKE_CXX_COMPILER ${CROSS_COMPILE}g++)

set(CMAKE_BUILD_TYPE Release)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/test/arm)


set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

FIND_PACKAGE(OpenMP REQUIRED)
if(OPENMP_FOUND)
message("ARM OPENMP FOUND")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()


add_definitions(
        # -o3 -g -w -Wall -mcpu=cortex-a17 -mfloat-abi=hard -mfpu=neon
        # -o3 -w -Wall
        )

include_directories(
  ${PROJECT_SOURCE_DIR}/src/gabor_im
  ${PROJECT_SOURCE_DIR}/src
  /opt/arm/include
)
link_directories (
  /opt/arm/lib
)

add_executable(OnlyAuth_arm
    ../src/onlyauth.cpp
    ${PROJECT_SOURCE_DIR}/src/authpic.cc
    ${PROJECT_SOURCE_DIR}/src/mutils.cc
    ${PROJECT_SOURCE_DIR}/src/lcd.cc
    ${PROJECT_SOURCE_DIR}/src/led.cc
    ${PROJECT_SOURCE_DIR}/src/laser.cc
    ${PROJECT_SOURCE_DIR}/src/usart.cc
    ${PROJECT_SOURCE_DIR}/src/eventmanager.cc
    ${PROJECT_SOURCE_DIR}/src/camera_manager.cc
    ${PROJECT_SOURCE_DIR}/src/key_file.cc
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
target_link_libraries(OnlyAuth_arm
  boost_program_options
  pthread
  tbb
  opencv_core
  opencv_imgproc
  opencv_features2d
  opencv_imgcodecs
  opencv_calib3d
  opencv_xfeatures2d
  opencv_highgui
  # /opt/arm/lib/libgomp.a
  # ${PROJECT_SOURCE_DIR}/lib/armhf/libCKCameraSDK.a
  gomp
  CKCameraSDK
)


add_executable(CalcTwoPic_arm
    ../src/calc_two_pic.cpp
    ${PROJECT_SOURCE_DIR}/src/authpic.cc
    ${PROJECT_SOURCE_DIR}/src/mutils.cc
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

target_link_libraries(CalcTwoPic_arm
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
	CKCameraSDK
)

add_executable(ScreenTest_arm
  ../src/screen_test.cpp
  )
target_link_libraries(ScreenTest_arm
  opencv_core
  opencv_imgcodecs
)

add_executable(LaserTest_arm
  ../src/laser_test.cpp
  ${PROJECT_SOURCE_DIR}/src/mutils.cc
  ${PROJECT_SOURCE_DIR}/src/laser.cc
  ${PROJECT_SOURCE_DIR}/src/usart.cc
)
target_link_libraries(LaserTest_arm
  opencv_core
)
add_executable(CameraTest_arm
  ../src/camera_test.cc
  ${PROJECT_SOURCE_DIR}/src/camera_manager.cc
  ${PROJECT_SOURCE_DIR}/src/lcd.cc
  ${PROJECT_SOURCE_DIR}/src/mutils.cc
)
target_link_libraries(CameraTest_arm
  pthread
  opencv_core
  opencv_imgcodecs
  boost_program_options
  CKCameraSDK
  # ${PROJECT_SOURCE_DIR}/lib/armhf/libCKCameraSDK.a
)

add_executable(GpioTest_arm
  ../src/gpio_test.cc
  ${PROJECT_SOURCE_DIR}/src/eventmanager.cc
  ${PROJECT_SOURCE_DIR}/src/led.cc
  ${PROJECT_SOURCE_DIR}/src/mutils.cc
  ${PROJECT_SOURCE_DIR}/src/lcd.cc
)
target_link_libraries(GpioTest_arm
opencv_core
pthread
boost_program_options
)

add_executable(SerialTrans_arm
  ../src/serialtrans.cpp
)