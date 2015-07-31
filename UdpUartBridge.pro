#-------------------------------------------------
#
# Project created by QtCreator 2014-12-24T19:04:37
#
#-------------------------------------------------

QT       += core
QT       += network

QT       -= gui

TARGET = UdpUartBridge
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    packetinterface.cpp \
    serialport.cpp \
    udpserver.cpp

HEADERS += \
    packetinterface.h \
    serialport.h \
    udpserver.h
