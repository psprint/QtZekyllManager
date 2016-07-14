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

SOURCES += main.cpp \
    closewithoutsavingdialog.cpp \
    coding_functions.cpp \
    lzcsde.cpp \
    lzcsde_entry.cpp \
    mainwindow.cpp \
    math_functions.cpp \
    messages.cpp \
    myqtabwidget.cpp \
    script_functions.cpp \
    zkiresize.cpp \
    zkrewrite.cpp \
    zmeditor.cpp \
    myqtablewidget.cpp \
    git.cpp \
    lgit.cpp \
    mycodeeditor.cpp

HEADERS  += mainwindow.h \
    call_once.h \
    closewithoutsavingdialog.h \
    coding_functions.h \
    lzcsde.h \
    lzcsde_entry.h \
    math_functions.h \
    messages.h \
    myqtabwidget.h \
    script_functions.h \
    singleton.h \
    zkiresize.h \
    zkrewrite.h \
    zmeditor.h \
    myqtablewidget.h \
    git.h \
    lgit.h \
    mycodeeditor.h

FORMS    += mainwindow.ui \
    zmeditor.ui \
    closewithoutsavingdialog.ui

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../libgit2/release/ -lgit2
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../libgit2/debug/ -lgit2
else:unix: LIBS += -L$$PWD/../libgit2/ -lgit2

INCLUDEPATH += $$PWD/../libgit2/include
DEPENDPATH += $$PWD/../libgit2/include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../libgit2/release/libgit2.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../libgit2/debug/libgit2.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../libgit2/release/git2.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../libgit2/debug/git2.lib
else:unix: PRE_TARGETDEPS += $$PWD/../libgit2/libgit2.a



MAC_SDK  = /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.11.sdk
if( !exists( $$MAC_SDK) ) {
  error("The selected Mac OSX SDK does not exist at $$MAC_SDK!")
}

# CoreFoundation
QMAKE_LFLAGS += -F$$MAC_SDK/System/Library/Frameworks/CoreFoundation.framework
LIBS += -framework CoreFoundation

# Security
QMAKE_LFLAGS += -F/System/Library/Frameworks/Security.framework
LIBS += -framework Security

# Iconv
macx: LIBS += -L$$PWD/../../../../../usr/lib/ -liconv
INCLUDEPATH += $$PWD/../../../../../Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.11.sdk/usr/include
DEPENDPATH += $$PWD/../../../../../Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.11.sdk/usr/include

# Curl
macx: LIBS += -L$$PWD/../../../../../usr/lib/ -lcurl

INCLUDEPATH += $$PWD/../../../../../Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.11.sdk/usr/include
DEPENDPATH += $$PWD/../../../../../Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.11.sdk/usr/include

# LibSSH2
macx: LIBS += -L$$PWD/../../../../../usr/local/Cellar/libssh2/1.7.0/lib/ -lssh2

INCLUDEPATH += $$PWD/../../../../../usr/local/Cellar/libssh2/1.7.0/include
DEPENDPATH += $$PWD/../../../../../usr/local/Cellar/libssh2/1.7.0/include

# Zlib
macx: LIBS += -L$$PWD/../../../../../usr/lib/ -lz

INCLUDEPATH += $$PWD/../../../../../Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.11.sdk/usr/include
DEPENDPATH += $$PWD/../../../../../Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.11.sdk/usr/include


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../qodeedit/release/ -lQodeEdit
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../qodeedit/debug/ -lQodeEdit
else:unix: LIBS += -L$$OUT_PWD/../qodeedit/ -lQodeEdit

INCLUDEPATH += $$PWD/../qodeedit/src
DEPENDPATH += $$PWD/../qodeedit/src

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../qodeedit/release/libQodeEdit.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../qodeedit/debug/libQodeEdit.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../qodeedit/release/QodeEdit.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../qodeedit/debug/QodeEdit.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../qodeedit/libQodeEdit.a
