#-------------------------------------------------
#
# Project created by QtCreator 2017-10-12T08:45:21
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = g_2
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    ve2.cpp \
    defaultsettings.cpp \
    graph.cpp \
    graphclass.cpp \
    entity.cpp \
    graphview.cpp \
    link.cpp \
    intersected.cpp \
    fun.cpp \
    brokenline.cpp

HEADERS += \
        mainwindow.h \
    precompiled.h \
    ve2.h \
    defaultsettings.h \
    singleton.h \
    graph.h \
    graphclass.h \
    entity.h \
    graphview.h \
    link.h \
    intersected.h \
    fun.h \
    brokenline.h

FORMS += \
        mainwindow.ui

# Use Precompiled headers (PCH)
PRECOMPILED_HEADER = precompiled.h
CONFIG   += c++14 precompile_header

QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.13
