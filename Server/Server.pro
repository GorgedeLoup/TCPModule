#-------------------------------------------------
#
# Project created by QtCreator 2015-08-01T23:23:52
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Server
TEMPLATE = app


SOURCES += main.cpp \
    server.cpp

HEADERS  += \
    treatmentplan.h \
    server.h

FORMS    += widget.ui
