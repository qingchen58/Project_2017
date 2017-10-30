#-------------------------------------------------
#
# Project created by QtCreator 2017-07-22T16:38:02
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = HD3023USBDB
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


#SOURCES += main.cpp\
#        mainwindow.cpp \
#    src/main.cpp

#HEADERS  += mainwindow.h \
#    hdebug.h

#include .pri
include($$PWD/src/hid/hid.pri)
include($$PWD/src/tasks/tasks.pri)
include($$PWD/src/utils/utils.pri)
#include(filemgr/filemgr.pri)
include($$PWD/src/cfgMgr/cfgMgr.pri)
include ($$PWD/src/ui/ui.pri)
#include ($$PWD/src/filemgr/filemgr.pri)

INCLUDEPATH += $$PWD/src/hid
INCLUDEPATH += $$PWD/src/tasks
INCLUDEPATH += $$PWD/src/utils
INCLUDEPATH += $$PWD/src/cfgMgr
INCLUDEPATH += $$PWD/src/ui
#INCLUDEPATH += $$PWD/src/filemgr

FORMS    += src/ui/mainwindow.ui

LIBS += -L$$PWD/src/hid/Lib/ -lhid
LIBS += -L$$PWD/src/hid/Lib/ -lSETUPAPI

INCLUDEPATH += $$PWD/src/hid/Inc
DEPENDPATH += $$PWD/src/hid/Lib

#src files
SOURCES += $$PWD/src/main.cpp

RESOURCES += \
    res/res.qrc



