#-------------------------------------------------
#
# Project created by QtCreator 2020-08-20T18:00:37
#
#-------------------------------------------------

QT       += core gui network dbus network concurrent multimedia multimediawidgets x11extras core-private

linux* {
    includes.files += $$PWD/platforms/linux/*.h
}

greaterThan(QT_MAJOR_VERSION, 4) {
  QT += widgets widgets-private
  # Qt >= 5.8
  greaterThan(QT_MAJOR_VERSION, 5)|greaterThan(QT_MINOR_VERSION, 7): QT += gui-private
  else: QT += platformsupport-private
}

TARGET = tla
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

CONFIG += c++17 link_pkgconfig
PKGCONFIG = dtkwidget dtkcore gsettings-qt

SOURCES += \
        main.cpp \
    multicolors.cpp \
    dsplitedwindow.cpp \
    dsplitedbar.cpp \
    dopenapplication.cpp \
    diconlookup.cpp \
    editor.cpp \
    dopeniconbutton.cpp \
    imagepopup.cpp \
    mobilestorage.cpp \
    ioop.cpp

HEADERS += \
    multicolors.h \
    dsplitedwindow.h \
    dsplitedbar.h \
    dopenapplication.h \
    diconlookup.h \
    editor.h \
    dopeniconbutton.h \
    imagepopup.h \
    helperclass.h \
    mobilestorage.h \
    helpers.h \
    ioop.h

FORMS += \
        mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    private/startupnotifications/startupnotifications.pri

RESOURCES += \
    main.qrc

unix:!macx: LIBS += -ldl

unix:!macx: LIBS += -lKF5Wallet
