QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    debug/moc_HierarchyModel.cpp \
    debug/moc_mainwindow.cpp \
    debug/moc_recordwindow.cpp \
    debug/moc_widget.cpp \
    debug/qrc_Resources.cpp \
    release/moc_HierarchyModel.cpp \
    release/moc_mainwindow.cpp \
    release/moc_recordwindow.cpp \
    release/moc_widget.cpp \
    release/qrc_Resources.cpp \
    Component.cpp \
    HierarchyModel.cpp \
    HierarchyObject.cpp \
    Input.cpp \
    LineRenderer.cpp \
    main.cpp \
    mainwindow.cpp \
    PointCloudRenderer.cpp \
    recordwindow.cpp \
    Renderer.cpp \
    SkyboxRenderer.cpp \
    Trail.cpp \
    widget.cpp



HEADERS += \
    debug/moc_predefs.h \
    release/uic/ui_mainwindow.h \
    release/moc_predefs.h \
    commondefs.h \
    Component.h \
    futils.h \
    HierarchyModel.h \
    HierarchyObject.h \
    Input.h \
    LineRenderer.h \
    mainwindow.h \
    nanoflann.hpp \
    nesteddefs.h \
    pointcloud.h \
    PointCloudRenderer.h \
    recordwindow.h \
    Renderer.h \
    SkyboxRenderer.h \
    Trail.h \
    vertex.h \
    widget.h



# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

FORMS += \
    mainwindow.ui \
    recordwindow.ui \

RESOURCES += \
    Resources.qrc
