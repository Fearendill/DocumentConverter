QT       += core gui widgets printsupport

greaterThan(QT_MAJOR_VERSION, 6): QT += widgets

CONFIG += c++17

TARGET = DocumentConverter
TEMPLATE = app

# Pour la conversion via QPrinter (PDF natif Qt)
# et potentiellement via LibreOffice en ligne de commande

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    converter/baseconverter.cpp \
    converter/txtconverter.cpp \
    converter/docxconverter.cpp \
    converter/odtconverter.cpp \
    utils/fileutils.cpp \
    utils/fileutils.cpp

HEADERS += \
    converter/converter.h \
    mainwindow.h \
    converter/converter.h \
    converter/baseconverter.h \
    converter/txtconverter.h \
    converter/docxconverter.h \
    converter/odtconverter.h \
    utils/fileutils.h \
    utils/fileutils.h

FORMS += \
    mainwindow.ui

# Détection de LibreOffice pour conversions avancées
unix {
    system(which libreoffice > /dev/null 2>&1) {
        DEFINES += HAS_LIBREOFFICE
    }
}

win32 {
    # Vérification Windows via registre ou chemin standard
    exists($$quote(C:/Program Files/LibreOffice/program/soffice.exe)) {
        DEFINES += HAS_LIBREOFFICE
        LIBREOFFICE_PATH = $$quote(C:/Program Files/LibreOffice/program/soffice.exe)
    }
}

# Ressources
RESOURCES += \
    resources.qrc
