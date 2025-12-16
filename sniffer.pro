QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = NetworkSniffer
TEMPLATE = app

CONFIG += c++11

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    packetsniffer.cpp \
    packetmodel.cpp

HEADERS += \
    mainwindow.h \
    packetsniffer.h \
    packetmodel.h

FORMS += \
    mainwindow.ui

# Platform-specific configurations
unix:!android {
    # Linux specific
    LIBS += -lpcap
    DEFINES += LINUX_PLATFORM
}

android {
    # Android specific
    DEFINES += ANDROID_PLATFORM
    QT += androidextras
}

# Default rules for deployment
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
