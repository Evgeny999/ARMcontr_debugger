#-------------------------------------------------
#
# Project created by QtCreator 2019-03-05T16:46:18
#
#-------------------------------------------------

QT       += core gui network

QT += widgets

TARGET = ARM.DBG
TEMPLATE = app
#CONFIG+=console

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += main.cpp\
        mainwindow.cpp \
    setaddwindow.cpp \
    alotofwindow.cpp \
    fileopenerrorwindow.cpp \
    celllist1.cpp \
    rowsettings.cpp \
    cellvaluechangewindow.cpp \
    dbal.cpp \
    processwindow.cpp \
    registerswindow.cpp \
    arraysview.cpp \
    afewrowsettings.cpp \
    memorycontenttable.cpp \
    memorycontenttablecustom.cpp \
    Threads.cpp

HEADERS  += mainwindow.h \
    setaddwindow.h \
    alotofwindow.h \
    fileopenerrorwindow.h \
    ParamStruct.h \
    celllist1.h \
    rowsettings.h \
    cellvaluechangewindow.h \
    varlistelm.h \
    dbal.h \
    processwindow.h\
    registerswindow.h \
    arraysview.h \
    SpinBoxAction.h \
    ReWriteContMenuWidget.h \
    afewrowsettings.h \
    SaveBeforeCloseAction.h \
    memorycontenttable.h \
    memorycontenttablecustom.h \
    Threads.h

FORMS    += mainwindow.ui \
    fileopenerrorwindow.ui \
    celllist1.ui \
    rowsettings.ui \
    cellvaluechangewindow.ui \
    processwindow.ui \
    registerswindow.ui \
    setaddwindow.ui \
    arraysview.ui \
    afewrowsettings.ui \
    memorycontenttable.ui

LIBS+= ./dbal.dll
