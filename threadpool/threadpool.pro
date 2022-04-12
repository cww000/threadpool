TEMPLATE = app
CONFIG += console c++20
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        main.cpp \
        taskqueue.cpp \
        threadpool.cpp

#QMAKE_CXXFLAGS = -std=c++20 -fmodules-ts

#system(cd $$OUT_PWD && g++  -std=c++20 -fmodules-ts -x c++-header /usr/include/c++/11.1.0/iostream /usr/include/c++/11.1.0/string)

HEADERS += \
    taskqueue.h \
    threadpool.h


unix|win32: LIBS += -lpthread
