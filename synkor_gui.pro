
QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = synkor_gui

TEMPLATE = app
CONFIG += c++17

DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += ASIO_STANDALONE
DEFINES += SODIUM_STATIC

QMAKE_CXXFLAGS += /std:c++17

SOURCES += \
    src/gui/main.cpp \
    src/gui/window.cpp \
    src/gui/tree_view.cpp \
    src/gui/list_view.cpp \
    src/gui/item_view.cpp \
    src/filesystem.cpp \
    contrib/json11/json11.cpp

HEADERS += \
    src/gui/window.hpp \
    src/gui/tree_view.hpp \
    src/gui/list_view.hpp \
    src/gui/item_view.hpp \
    src/global.hpp \
    src/filesystem.hpp \
    contrib/json11/json11.hpp

FORMS += \
    src/gui/window.ui

win32: LIBS += -L$$PWD/build/windows/vs/sodium/ -llibsodium

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/build/windows/vs/sodium/libsodium.lib
else:win32-g++: PRE_TARGETDEPS += $$PWD/build/windows/vs/sodium/liblibsodium.a
