#-------------------------------------------------
#
# Project created by QtCreator 2014-02-13T15:19:16
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DirectoryViewer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    mainhandler.cpp \
    types.cpp \
    osinterface.cpp \
    myexceptions.cpp \
    openedlisthandle.cpp \
    functions.cpp \
    mylineedit.cpp \
    mytreeview.cpp

HEADERS  += mainwindow.h \
    mainhandler.h \
    types.h \
    osinterface.h \
    myexceptions.h \
    openedlisthandle.h \
    functions.h \
    mylineedit.h \
    mytreeview.h \
    stylesheets.h

FORMS    += mainwindow.ui

QMAKE_CXXFLAGS += -std=c++11
