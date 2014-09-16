#-------------------------------------------------
#
# Project created by QtCreator 2014-05-17T23:08:15
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = bla3Dscan_qt
TEMPLATE = app

win32 {
    INCLUDEPATH += C:/Libs/boost/boost_1_56_0
    LIBS += -LC:/Libs/boost/boost_1_56_0/lib32-msvc-10.0

    INCLUDEPATH += C:/Libs/opencv/opencv2.4.9/install/include
    LIBS += -LC:/Libs/opencv/opencv2.4.9/install/x86/x86/vc10/lib

    CONFIG(debug) {
        LIBS += -lopencv_calib3d249d
        LIBS += -lopencv_core249d
        LIBS += -lopencv_features2d249d
        LIBS += -lopencv_highgui249d
        LIBS += -lopencv_imgproc249d
        LIBS += -lopencv_nonfree249d
        LIBS += -lopencv_photo249d
        LIBS += -lopencv_stitching249d
        LIBS += -lopencv_video249d
        LIBS += -lopencv_videostab249d
    
        LIBS += -lboost_filesystem-vc100-mt-gd-1_56
        LIBS += -lboost_system-vc100-mt-gd-1_56
        LIBS += -lboost_thread-vc100-mt-gd-1_56
    }
    CONFIG(release) {
        LIBS += -lopencv_calib3d249
        LIBS += -lopencv_core249
        LIBS += -lopencv_features2d249
        LIBS += -lopencv_highgui249
        LIBS += -lopencv_imgproc249
        LIBS += -lopencv_nonfree249
        LIBS += -lopencv_photo249
        LIBS += -lopencv_stitching249
        LIBS += -lopencv_video249
        LIBS += -lopencv_videostab249
    
        LIBS += -lboost_filesystem-vc100-mt-1_56
        LIBS += -lboost_system-vc100-mt-1_56
        LIBS += -lboost_thread-vc100-mt-1_56
    }

    LIBS += -lws2_32
}
else {
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
}

SOURCES += main.cpp\
        mainwindow.cpp \
    cam_viewer.cpp \
    cam_scene.cpp \
    calibrationdialog.cpp \
    utils.cpp \
    reconstruct.cpp

HEADERS  += mainwindow.h \
    cam.h \
    cam_viewer.h \
    cam_scene.h \
    calibrationdialog.h \
    utils.h \
    reconstruct.h

FORMS    += mainwindow.ui \
    calibrationdialog.ui
