#-------------------------------------------------
#
# Project created by QtCreator 2017-05-02T19:00:28
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LZW_Compressor
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    clzwcompressor.cpp \
    clzwdecompressor.cpp \
    cbenchmark.cpp

HEADERS  += mainwindow.h \
    clzwcompressor.h \
    clzwdecompressor.h \
    cbenchmark.h

FORMS    += mainwindow.ui \
    cbenchmark.ui
