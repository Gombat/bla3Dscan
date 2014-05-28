#-------------------------------------------------
#
# Project created by QtCreator 2014-05-17T23:08:15
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = bla3Dscan_qt
TEMPLATE = app

INCLUDEPATH += /usr/include

LIBS += -L/usr/local/lib
LIBS += -lopencv_calib3d
LIBS += -lopencv_contrib
LIBS += -lopencv_core
LIBS += -lopencv_features2d
LIBS += -lopencv_flann
LIBS += -lopencv_gpu
LIBS += -lopencv_highgui
LIBS += -lopencv_imgproc
LIBS += -lopencv_legacy
LIBS += -lopencv_ml
LIBS += -lopencv_nonfree
LIBS += -lopencv_objdetect
LIBS += -lopencv_ocl
LIBS += -lopencv_photo
LIBS += -lopencv_stitching
LIBS += -lopencv_video
LIBS += -lopencv_videostab

LIBS += -lboost_filesystem
LIBS += -lboost_system
LIBS += -lboost_thread

SOURCES += main.cpp\
        mainwindow.cpp \
    arduino_serial.cpp \
    cam_viewer.cpp \
    cam_scene.cpp

HEADERS  += mainwindow.h \
    cam.h \
    arduino_serial.h \
    cam_viewer.h \
    cam_scene.h

FORMS    += mainwindow.ui
