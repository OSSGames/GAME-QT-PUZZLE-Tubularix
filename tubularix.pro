QT += core \
	 gui network
TEMPLATE = app
MOC_DIR = compile
OBJECTS_DIR = compile
RCC_DIR = compile
UI_DIR = compile

# DESTDIR = bin
TARGET = tubularix
SOURCES += src/main.cpp \
    src/mainwindow.cpp \
    src/gameboard.cpp \
    src/piece.cpp \
    src/gametypedialog.cpp \
    src/gameoptionsdialog.cpp \
    src/keyselectionbutton.cpp \
    src/piececolorselection.cpp \
    src/networkserverdialog.cpp \
    src/networkclientdialog.cpp \
    src/gamecolors.cpp
HEADERS += src/mainwindow.h \
    src/gameboard.h \
    src/piece.h \
    src/gametypedialog.h \
    src/gameoptionsdialog.h \
    src/keyselectionbutton.h \
    src/piececolorselection.h \
    src/networkserverdialog.h \
    src/networkclientdialog.h \
    src/gamecolors.h
FORMS += ui/mainwindow.ui \
    ui/gametypedialog.ui \
    ui/gameoptionsdialog.ui \
    ui/networkserverdialog.ui \
    ui/networkclientdialog.ui
TRANSLATIONS += translations/tubularix_es.ts \
    translations/tubularix_gl.ts \
    translations/tubularix_cs.ts
RESOURCES += tubularix.qrc
RC_FILE = tubularix.rc

# INSTALL
isEmpty(PREFIX):PREFIX = /usr
target.path = $$PREFIX/bin/

# target.files = bin/tubularix
documents.path = $$PREFIX/share/doc/packages/tubularix/
documents.files = CHANGELOG \
    COPYING \
    LICENSE \
    README \
    README_ES
translations.path = $$PREFIX/share/games/tubularix/
translations.files = translations/*.qm
pixmap.path = $$PREFIX/share/pixmaps/
pixmap.files = icons/tubularix.png
desktop.path = $$PREFIX/share/applications/
desktop.files = icons/tubularix.desktop
INSTALLS += target \
    documents \
    translations \
    pixmap \
    desktop
