#-------------------------------------------------
#
# Project created by QtCreator 2018-10-10T22:41:23
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = UpgradeFlash
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    upgrade_ctrl/crc16.cpp \
    upgrade_ctrl/hexparsing.cpp \
    upgrade_ctrl/message.cpp \
    upgrade_ctrl/upgradeproc.cpp \
    upgradework.cpp \
    qmsginfo.cpp \
    usbcan/canfunc_puchuang.cpp \
    usbcan/canfunc.cpp \
    usbcan/canfunc_chuangxin.cpp

HEADERS += \
        mainwindow.h \
    upgrade_ctrl/crc16.h \
    upgrade_ctrl/hexparsing.h \
    upgrade_ctrl/message.h \
    upgrade_ctrl/upgradeproc.h \
    upgradework.h \
    qmsginfo.h \
    usbcan/puchuangdianzi/CAN_TO_USB.h \
    usbcan/canfunc_puchuang.h \
    usbcan/canfunc.h \
    usbcan/chuangxinkeji/ControlCAN.h \
    usbcan/canfunc_chuangxin.h

FORMS += \
        mainwindow.ui

RC_ICONS = upgrade_flash.ico
