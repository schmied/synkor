TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

DEFINES += ASIO_STANDALONE
DEFINES += SODIUM_STATIC

QMAKE_CXXFLAGS += /std:c++17

SOURCES += \
    src/main.cpp \
    src/config.cpp \
    src/server.cpp \
    src/filesystem.cpp \
    contrib/json11/json11.cpp

HEADERS += \
    src/config.hpp \
    src/server.hpp \
    src/filesystem.hpp \
    src/global.hpp \
    contrib/json11/json11.hpp

win32: LIBS += -L$$PWD/build/windows/vs/sodium/ -llibsodium

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/build/windows/vs/sodium/libsodium.lib
else:win32-g++: PRE_TARGETDEPS += $$PWD/build/windows/vs/sodium/liblibsodium.a
