TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt
INCLUDEPATH += $$PWD/src/include
DEPENDPATH += $$PWD/src/include

SOURCES += src/main.cc \
    src/camera_manager.cc \
    src/global_arg.cc \
    src/key_file.cc \
    src/laser.cc \
    src/lcd.cc \
    src/usart.cc \
    src/utils.cc \
    src/state_machine.cc

LIBS += -L$$PWD/lib/x64 -Wl,-Bstatic -lCKCameraSDK -Wl,-Bdynamic -lpthread

HEADERS += \
    src/camera_manager.h \
    src/global_arg.h \
    src/key_file.h \
    src/laser.h \
    src/lcd.h \
    src/screen_manager.h \
    src/state_machine.h \
    src/usart.h \
    src/utils.h

TARGET = ../check_system/CheckSystem
