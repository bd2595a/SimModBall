QT += core gui xml
QT += widgets
HEADERS += balls.h
SOURCES += balls.cpp

target.path = ball
sources.files = $$SOURCES $$HEADERS balls.pro
sources.path = .
INSTALLS += target sources
