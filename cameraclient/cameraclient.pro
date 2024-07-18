QT += core gui widgets network

CONFIG += c++17

SOURCES += \
    cameraclient.cpp \
    main.cpp \
    mainwindow.cpp \
    workerb.cpp \
    workerc.cpp

HEADERS += \
    cameraclient.h \
    mainwindow.h \
    packet.h \
    workerb.h \
    workerc.h

FORMS += \
    mainwindow.ui
