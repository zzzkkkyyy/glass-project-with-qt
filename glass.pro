#-------------------------------------------------
#
# Project created by QtCreator 2018-05-15T12:45:14
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = glass
TEMPLATE = app

QMAKE_LFLAGS += /STACK:10240000


SOURCES += main.cpp\
        mainwindow.cpp \
    hardware.cpp \
    about.cpp \
    process.cpp

HEADERS  += mainwindow.h \
    hardware.h \
    about.h \
    process.h

FORMS    += mainwindow.ui \
    hardware.ui \
    about.ui

RESOURCES += \
    image.qrc

INCLUDEPATH += E:\OpenCV_2.4.9\opencv\build\include

CONFIG(debug, debug|release): {
LIBS += -LE:\OpenCV_2.4.9\opencv\build\x64\vc12\lib\ \
-lopencv_core249d \
-lopencv_imgproc249d \
-lopencv_highgui249d \
-lopencv_ml249d \
-lopencv_video249d \
-lopencv_features2d249d \
-lopencv_calib3d249d \
-lopencv_objdetect249d \
-lopencv_contrib249d \
-lopencv_legacy249d \
-lopencv_flann249d
}
CONFIG(release, debug|release): {
LIBS += -LE:\OpenCV_2.4.9\opencv\build\x86\vc12\lib\ \
-lopencv_core249 \
-lopencv_imgproc249 \
-lopencv_highgui249 \
-lopencv_ml249 \
-lopencv_video249 \
-lopencv_features2d249 \
-lopencv_calib3d249 \
-lopencv_objdetect249 \
-lopencv_contrib249 \
-lopencv_legacy249 \
-lopencv_flann249
}
