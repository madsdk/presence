# -------------------------------------------------
# Project created by QtCreator 2009-10-26T17:58:21
# -------------------------------------------------
QT += network
QT -= gui
TARGET = presence
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app

unix:LIBS += ../PackedData/*.o ../qPresenceCommon/*.o
unix:INCLUDEPATH += ../PackedData ../qPresenceCommon

win32:LIBS += -L..\PackedData -lPackedData -L..\qPresenceCommon -lqPresenceCommon
win32:INCLUDEPATH += ..\PackedData ..\qPresenceCommon

SOURCES += main.cpp \
    announcementpacket.cpp \
    announcer.cpp \
    controlconnectionmanager.cpp \
    controlconnection.cpp \
    presenceconfig.cpp

HEADERS += announcementpacket.h \
    announcer.h \
    controlconnectionmanager.h \
    controlconnection.h \
    presenceconfig.h
