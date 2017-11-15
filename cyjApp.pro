#-------------------------------------------------
#
# Project created by QtCreator 2017-09-05T10:05:26
#
#-------------------------------------------------

QT       += core gui sql network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = cyjApp
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    surfacecommunication.cpp \
    autoalgorithm.cpp \
    pid.cpp \
    trackmemory.cpp \
    SICK.cpp

HEADERS  += mainwindow.h \
    surfacecommunication.h \
    autoalgorithm.h \
    pid.h \
    trackmemory.h \
    SICK.h

FORMS    += mainwindow.ui
