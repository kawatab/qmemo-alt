######################################################################
# Automatically generated by qmake (3.0) Sun Apr 14 11:40:58 2019
######################################################################

TEMPLATE = app
TARGET = qmemo
INCLUDEPATH += .

QT += widgets core

CONFIG += debug_and_release
           
# Input
HEADERS += src/datahandler.hpp \
           src/fileinfomodel.hpp \
           src/fileinfoproxy.hpp \
           src/gui/editpane.hpp \
           src/gui/listpane.hpp \
           src/gui/mainwindow.hpp \
           src/gui/previewdelegate.hpp

SOURCES += src/main.cpp \
           src/datahandler.cpp \
           src/fileinfomodel.cpp \
           src/fileinfoproxy.cpp \
           src/gui/editpane.cpp \
           src/gui/listpane.cpp \
           src/gui/mainwindow.cpp \
           src/gui/previewdelegate.cpp

RESOURCES += i18n.qrc
           
TRANSLATIONS = i18n/qmemo_ja.ts \
               i18n/qmemo_hu.ts

lupdate_only{
SOURCES = src/gui/editpane.cpp \
          src/gui/listpane.cpp \
          src/gui/mainwindow.cpp
}

CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT

Release:DESTDIR = release
Release:OBJECTS_DIR = release/.obj
Release:MOC_DIR = release/.moc
Release:RCC_DIR = release/.rcc
Release:UI_DIR = release/.ui

Debug:DESTDIR = debug
Debug:OBJECTS_DIR = debug/.obj
Debug:MOC_DIR = debug/.moc
Debug:RCC_DIR = debug/.rcc
Debug:UI_DIR = debug/.ui
