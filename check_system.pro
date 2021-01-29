TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt
INCLUDEPATH += $$PWD/src/include
INCLUDEPATH += $$PWD/src/gabor_im
DEPENDPATH += $$PWD/src/include

SOURCES += src/main.cc \
    src/usart.cc \
    src/camera_manager.cc \
    src/eventmanager.cc \
    src/global_arg.cc \
    src/hostcontroller.cc \
    src/key_file.cc \
    src/laser.cc \
    src/lcd.cc \
    src/led.cc \
    src/mutils.cc \
    src/state_machine.cc \
    src/gabor_im/bluesteinSetup.cpp \
    src/gabor_im/cosd.cpp \
    src/gabor_im/createNormalizedFrequencyVector.cpp \
    src/gabor_im/fft1.cpp \
    src/gabor_im/fft2.cpp \
    src/gabor_im/gabor_im.cpp \
    src/gabor_im/gabor_im_data.cpp \
    src/gabor_im/gabor_im_emxAPI.cpp \
    src/gabor_im/gabor_im_emxutil.cpp \
    src/gabor_im/gabor_im_initialize.cpp \
    src/gabor_im/gabor_im_rtwutil.cpp \
    src/gabor_im/gabor_im_terminate.cpp \
    src/gabor_im/imgaborfilt.cpp \
    src/gabor_im/ipermute.cpp \
    src/gabor_im/padarray.cpp \
    src/gabor_im/power.cpp \
    src/gabor_im/rtGetInf.cpp \
    src/gabor_im/rtGetNaN.cpp \
    src/gabor_im/rt_nonfinite.cpp \
    src/gabor_im/sind.cpp

LIBS += -L$$PWD/lib/x64 -Wl,-Bstatic -lCKCameraSDK -Wl,-Bdynamic -lpthread
LIBS += -lopencv_core -lopencv_features2d -lopencv_imgproc -lopencv_imgcodecs -lopencv_calib3d
LIBS += -lgomp -lopencv_xfeatures2d
HEADERS += \
    src/camera_manager.h \
    src/constant.h \
    src/eventmanager.h \
    src/global_arg.h \
    src/hostcontroller.h \
    src/key_file.h \
    src/laser.h \
    src/lcd.h \
    src/led.h \
    src/state_machine.h \
    src/usart.h \
    src/utils.h \
    src/gabor_im/bluesteinSetup.h \
    src/gabor_im/cosd.h \
    src/gabor_im/createNormalizedFrequencyVector.h \
    src/gabor_im/fft1.h \
    src/gabor_im/fft2.h \
    src/gabor_im/gabor_im.h \
    src/gabor_im/gabor_im_data.h \
    src/gabor_im/gabor_im_emxAPI.h \
    src/gabor_im/gabor_im_emxutil.h \
    src/gabor_im/gabor_im_initialize.h \
    src/gabor_im/gabor_im_rtwutil.h \
    src/gabor_im/gabor_im_terminate.h \
    src/gabor_im/imgaborfilt.h \
    src/gabor_im/ipermute.h \
    src/gabor_im/padarray.h \
    src/gabor_im/power.h \
    src/gabor_im/rtGetInf.h \
    src/gabor_im/rtGetNaN.h \
    src/gabor_im/rt_nonfinite.h \
    src/gabor_im/sind.h 

TARGET = ../CheckSystem
