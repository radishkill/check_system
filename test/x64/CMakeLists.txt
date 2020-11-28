set (CMAKE_CXX_COMPILER "g++")

set(CMAKE_BUILD_TYPE "Release")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/test/x64)

include_directories(
  ${PROJECT_SOURCE_DIR}/src/gabor_im
  ./include
  ${PROJECT_SOURCE_DIR}/src
)
FIND_PACKAGE(OpenMP REQUIRED)
if(OPENMP_FOUND)
  message("OPENMP FOUND")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()


add_definitions(
        -o3 -g -w -Wall 
        )
        

add_executable(CalcTwoPic_x64
    ../src/calc_two_pic.cpp
    ${PROJECT_SOURCE_DIR}/src/authpic.cc
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

target_link_libraries(CalcTwoPic_x64
  pthread
  tbb
  #  opencv_highgui
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

add_executable(ScreenTest_x64
  ../src/screen_test.cpp
  )

add_executable(LaserTest_x64
  ../src/laser_test.cpp
  ${PROJECT_SOURCE_DIR}/src/utils.cc
  ${PROJECT_SOURCE_DIR}/src/laser.cc
  ${PROJECT_SOURCE_DIR}/src/usart.cc
)
