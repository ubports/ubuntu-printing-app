TEMPLATE = lib
TARGET = Ubuntu_Printing_Appbackend
QT += qml quick
CONFIG += qt plugin

QT += printsupport

load(ubuntu-click)

TARGET = $$qtLibraryTarget($$TARGET)

# Input
SOURCES += \
    backend.cpp \
    printerinfo.cpp \
    printer.cpp \
    document.cpp \
    popplerimageprovider.cpp

HEADERS += \
    backend.h \
    printerinfo.h \
    printer.h \
    document.h \
    popplerimageprovider.h

OTHER_FILES = qmldir

!equals(_PRO_FILE_PWD_, $$OUT_PWD) {
    copy_qmldir.target = $$OUT_PWD/qmldir
    copy_qmldir.depends = $$_PRO_FILE_PWD_/qmldir
    copy_qmldir.commands = $(COPY_FILE) \"$$replace(copy_qmldir.depends, /, $$QMAKE_DIR_SEP)\" \"$$replace(copy_qmldir.target, /, $$QMAKE_DIR_SEP)\"
    QMAKE_EXTRA_TARGETS += copy_qmldir
    PRE_TARGETDEPS += $$copy_qmldir.target
}

qmldir.files = qmldir
installPath = $$[QT_INSTALL_QML]/Ubuntu_Printing_App
qmldir.path = $$installPath
target.path = $$installPath
INSTALLS += target qmldir

unix:!macx: INCLUDEPATH += /usr/include/poppler/qt5/
unix:!macx: LIBS += -lpoppler-qt5 -lcups
