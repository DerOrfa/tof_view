#-------------------------------------------------
#
# Project created by QtCreator 2014-09-02T16:01:19
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = tof_view
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11

SOURCES += main.cpp widget.cpp pmd.cpp \
    recog.cpp

HEADERS  += widget.hpp pmd.hpp \
    recog.hpp

FORMS    += widget.ui

unix: LIBS += -L/tmp/lib/ -lsgl -lqt4_glue -L/opt/PMDSDK/bin/ -lpmdaccess2

INCLUDEPATH += /tmp/include/libsgl /opt/PMDSDK/include
DEPENDPATH += /tmp/include/libsgl /opt/PMDSDK/include

OTHER_FILES += \
    tof_view.supp

