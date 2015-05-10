#-------------------------------------------------
#
# Project created by QtCreator 2015-05-10T07:14:06
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FileArchiveWindow
TEMPLATE = app


SOURCES += main.cpp\
        file_archive_window.cpp \
    file_archive.c \
    utils.c \
    text_dialog.cpp

HEADERS  += file_archive_window.h \
    file_archive.h \
    types.h \
    utils.h \
    text_dialog.h

FORMS    += file_archive_window.ui
