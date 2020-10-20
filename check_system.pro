TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += $$PWD/src/include
DEPENDPATH += $$PWD/src/include

SOURCES += src/main.cc \
    src/usart.cpp

LIBS += -L$$PWD/lib/ -lCKCameraSDK_x64 -lpthread

HEADERS += \
    src/usart.h
