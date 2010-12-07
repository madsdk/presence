# -------------------------------------------------
# Project created by QtCreator 2009-12-18T13:26:15
# -------------------------------------------------
QT -= gui
TARGET = qPresenceCommon
TEMPLATE = lib
CONFIG += staticlib

unix:INCLUDEPATH += ../PackedData
win32:INCLUDEPATH += ..\PackedData

SOURCES += nodename.cpp \
    servicename.cpp \
    servicedata.cpp \
    servicedescription.cpp \
    presencepeer.cpp

HEADERS += nodename.h \
    servicename.h \
    servicedata.h \
    servicedescription.h \
    presencepeer.h
