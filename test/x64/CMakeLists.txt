set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_BUILD_TYPE "Release")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/test/x64)

add_compile_options("-std=c++14")
include_directories(
  ${PROJECT_SOURCE_DIR}/src/gabor_im
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_SOURCE_DIR}/src/include
)

FIND_PACKAGE(OpenMP REQUIRED)
if(OPENMP_FOUND)
  message("x86_64 OPENMP FOUND")
  message("${OpenMP_C_FLAGS}")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

add_definitions(
  -g -w -Wall 
  )

add_executable(OnlyAuth_x64
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
target_link_libraries(OnlyAuth_x64
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
  gomp
	${PROJECT_SOURCE_DIR}/lib/x64/libCKCameraSDK.a
)

add_executable(CalcTwoPic_x64
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

target_link_libraries(CalcTwoPic_x64
  pthread
  tbb
  boost_program_options
  #  opencv_highgui
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

add_executable(ScreenTest_x64
  ../src/screen_test.cpp
  )
target_link_libraries(ScreenTest_x64
  opencv_core
  opencv_imgcodecs
)

add_executable(LaserTest_x64
  ../src/laser_test.cpp
  ${PROJECT_SOURCE_DIR}/src/mutils.cc
  ${PROJECT_SOURCE_DIR}/src/laser.cc
  ${PROJECT_SOURCE_DIR}/src/usart.cc
)
target_link_libraries(LaserTest_x64
  opencv_core
  )
add_executable(CameraTest_x64
  ../src/camera_test.cc
  ${PROJECT_SOURCE_DIR}/src/camera_manager.cc
  ${PROJECT_SOURCE_DIR}/src/lcd.cc
  ${PROJECT_SOURCE_DIR}/src/mutils.cc
)
target_link_libraries(CameraTest_x64
  opencv_core
  opencv_imgcodecs
  boost_program_options
  ${PROJECT_SOURCE_DIR}/lib/x64/libCKCameraSDK.a
)

add_executable(MainTest_x64
  ../src/main.cpp
  ${PROJECT_SOURCE_DIR}/src/eventmanager.cc
)
target_link_libraries(MainTest_x64
  
)

add_executable(GpioTest_x64
  ../src/gpio_test.cc
  ${PROJECT_SOURCE_DIR}/src/eventmanager.cc
  ${PROJECT_SOURCE_DIR}/src/led.cc
  ${PROJECT_SOURCE_DIR}/src/mutils.cc
  ${PROJECT_SOURCE_DIR}/src/lcd.cc
)
target_link_libraries(GpioTest_x64
opencv_core
boost_program_options
)
