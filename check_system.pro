TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += $$PWD/src/include
DEPENDPATH += $$PWD/src/include

SOURCES += src/main.cc \
    src/camera_manager.cc \
    src/laser.cc \
    src/usart.cc \
    src/utils.cc

LIBS += -L$$PWD/lib/x64 -Wl,-Bstatic -lCKCameraSDK -Wl,-Bdynamic -lpthread

HEADERS += \
    src/camera_manager.h \
    src/laser.h \
    src/screen_manager.h \
    src/usart.h \
    src/utils.h
