#-------------------------------------------------
#
# Project created by QtCreator 2014-06-11T01:33:42
#
#-------------------------------------------------

QT       += core gui widgets serialport opengl

TARGET = GRAM
TEMPLATE = app

DESTDIR = ../GRAM

SOURCES += main.cpp\
    mainwindow.cpp \
    triglwidget.cpp \
    modelmodifydialog.cpp \
    slicer.cpp \
    slicerdialog.cpp \
    progressdialog.cpp \
    typedialog.cpp

HEADERS  += \
    mainwindow.h \
    triglwidget.h \
    modelmodifydialog.h \
    slicerdialog.h \
    progressdialog.h \
    typedialog.h

FORMS    += \
    mainwindow.ui \
    modelmodifydialog.ui \
    slicerdialog.ui \
    progressdialog.ui \
    typedialog.ui

include(SerialWidget/SerialWidget.pri)
include(ConsoleWidget/ConsoleWidget.pri)
include(SerialComm/SerialComm.pri)
include(FirmwareWidget/FirmwareWidget.pri)
include(StlUtil/StlUtil.pri)

INCLUDEPATH += ../

TRANSLATIONS += zh_cn.ts

RESOURCES += \
    qrc.qrc

RC_FILE = version.rc
