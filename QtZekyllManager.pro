#-------------------------------------------------
#
# Project created by QtCreator 2016-07-05T17:15:35
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QtZekyllManager
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    zkiresize.cpp \
    lzcsde_entry.cpp \
    lzcsde.cpp

HEADERS  += mainwindow.h \
    zkiresize.h \
    lzcsde_entry.h \
    lzcsde.h

FORMS    += mainwindow.ui
