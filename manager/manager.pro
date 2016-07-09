#-------------------------------------------------
#
# Project created by QtCreator 2016-07-05T17:15:35
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QtZekyllManager
TEMPLATE = app
CONFIG += c++11

SOURCES += main.cpp\
        mainwindow.cpp \
    zkiresize.cpp \
    lzcsde_entry.cpp \
    lzcsde.cpp \
    math_functions.cpp \
    coding_functions.cpp \
    script_functions.cpp \
    messages.cpp \
    myqtabwidget.cpp \
    editor.cpp \
    closewithoutsavingdialog.cpp

HEADERS  += mainwindow.h \
    zkiresize.h \
    lzcsde_entry.h \
    lzcsde.h \
    math_functions.h \
    coding_functions.h \
    script_functions.h \
    messages.h \
    call_once.h \
    singleton.h \
    myqtabwidget.h \
    editor.h \
    closewithoutsavingdialog.h

FORMS    += mainwindow.ui \
    editor.ui \
    closewithoutsavingdialog.ui