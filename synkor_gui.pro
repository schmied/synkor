#-------------------------------------------------
#
# Project created by QtCreator 2018-10-22T21:17:16
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = synkor_gui
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

CONFIG += c++17

DEFINES += ASIO_STANDALONE
DEFINES += SODIUM_STATIC

QMAKE_CXXFLAGS += /std:c++17

SOURCES += \
    src/gui/main.cpp \
    src/gui/window.cpp \
    src/gui/tree_view.cpp \
    src/gui/list_view.cpp \
    src/gui/base_view.cpp

HEADERS += \
    src/gui/window.hpp \
    src/gui/tree_view.hpp \
    src/gui/base_view.hpp \
    src/gui/list_view.hpp

FORMS += \
    src/gui/window.ui

win32: LIBS += -L$$PWD/build/windows/vs/sodium/ -llibsodium

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/build/windows/vs/sodium/libsodium.lib
else:win32-g++: PRE_TARGETDEPS += $$PWD/build/windows/vs/sodium/liblibsodium.a
