QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
CONFIG += build_all

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    circularbuffer.cpp \
    main.cpp \
    mainwindow.cpp \
    panel.cpp \
    plot.cpp \
    thread/MyThread.cpp \
    thread/WorkThread.cpp \
    tools/dma_from_device.c \
    tools/dma_to_device.c \
    tools/dma_tool.c \
    tools/dma_utils.c

HEADERS += \
    circularbuffer.h \
    mainwindow.h \
    panel.h \
    plot.h \
    settings.h \
    thread/MyThread.h \
    thread/WorkThread.h \
    tools/dma_tool.h \
    tools/dma_utils.h

QWT_ROOT = /home/toec/qwt/qwt-6.1.5

INCLUDEPATH += $${QWT_ROOT}/src
INCLUDEPATH += $${QWT_ROOT}/src/tools
INCLUDEPATH += $${QWT_ROOT}/src/thread
LIBS += -L"$${QWT_ROOT}/lib" -lqwt

QT += printsupport
QT += concurrent
QT += opengl

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
