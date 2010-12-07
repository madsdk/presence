#-------------------------------------------------
#
# Project created by QtCreator 2009-12-18T11:51:46
#
#-------------------------------------------------

QT       += network
QT       -= gui

TARGET = PackedData
TEMPLATE = lib
CONFIG += staticlib

SOURCES += byteswap.cpp \
           maxlengthcstring.cpp \
           uint16.cpp \
           uint32.cpp

HEADERS += byteswap.h \
           maxlengthcstring.h \
           serializable.h \
           uint16.h \
           uint32.h

