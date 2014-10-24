#-------------------------------------------------
#
# Project created by QtCreator 2014-06-15T23:53:59
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Poisson
TEMPLATE = app


DEPENDPATH += . /usr/local/include/opencv/ /usr/local/include/opencv2/
INCLUDEPATH += . /usr/local/include/opencv/ /usr/local/include/opencv2/
LIBS += -L/usr/local/lib/ \
        -lopencv_core \
        -lopencv_highgui \
        -lopencv_imgproc \

SOURCES += main.cpp\
        mainwindow.cpp \
    poissonimageediting.cpp \
    pendialog.cpp

HEADERS  += mainwindow.h \
    poissonimageediting.h \
    pendialog.h

FORMS    += mainwindow.ui \
    pendialog.ui
