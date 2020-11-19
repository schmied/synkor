
TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

DEFINES += ASIO_STANDALONE
DEFINES += SODIUM_STATIC
DEFINES += _WIN32_WINNT=0x0601

QMAKE_CXXFLAGS += /std:c++17

SOURCES += \
    contrib/json11/json11.cpp \
	src/config.cpp \
	src/filesystem.cpp \
	src/log.cpp \
	src/main.cpp \
	src/server.cpp \
	src/base.cpp

HEADERS += \
    contrib/json11/json11.hpp \
	src/config.hpp \
	src/filesystem.hpp \
	src/global.hpp \
	src/log.hpp \
	src/server.hpp \
	src/base.hpp

win32: LIBS += -L$$PWD/build/windows/vs/sodium/ -llibsodium

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/build/windows/vs/sodium/libsodium.lib
else:win32-g++: PRE_TARGETDEPS += $$PWD/build/windows/vs/sodium/liblibsodium.a
