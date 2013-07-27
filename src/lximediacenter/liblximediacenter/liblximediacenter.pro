TEMPLATE = lib
CONFIG += dll
QT += network xml
LXIMEDIA_DIR = ../../..
DESTDIR = $${OUT_PWD}/$${LXIMEDIA_DIR}/bin
TARGET = LXiMediaCenter
include($${PWD}/$${LXIMEDIA_DIR}/include/config.pri)

INCLUDEPATH += $${PWD}/$${LXIMEDIA_DIR}/include/liblximediacenter
DEPENDPATH += $${PWD}/$${LXIMEDIA_DIR}/include/liblximediacenter

DEFINES += S_BUILD_LIBLXIMEDIACENTER

CONFIG += precompile_header
PRECOMPILED_HEADER = $${PWD}/$${LXIMEDIA_DIR}/include/LXiMediaCenter

include($${PWD}/$${LXIMEDIA_DIR}/include/liblxicore/linklxicore.pri)
include($${PWD}/$${LXIMEDIA_DIR}/include/liblxiserver/linklxiserver.pri)
include($${PWD}/$${LXIMEDIA_DIR}/include/liblxistream/linklxistream.pri)
include($${PWD}/$${LXIMEDIA_DIR}/include/liblxistreamgui/linklxistreamgui.pri)

# Files
HEADERS += $${PWD}/$${LXIMEDIA_DIR}/include/LXiMediaCenter \
 $${PWD}/$${LXIMEDIA_DIR}/include/liblximediacenter/export.h \
 $${PWD}/$${LXIMEDIA_DIR}/include/liblximediacenter/backendsandbox.h \
 $${PWD}/$${LXIMEDIA_DIR}/include/liblximediacenter/backendserver.h \
 $${PWD}/$${LXIMEDIA_DIR}/include/liblximediacenter/mediaprofiles.h \
 $${PWD}/$${LXIMEDIA_DIR}/include/liblximediacenter/mediaserver.h \
 $${PWD}/$${LXIMEDIA_DIR}/include/liblximediacenter/mediastream.h \
 $${PWD}/$${LXIMEDIA_DIR}/include/liblximediacenter/teletext.h

SOURCES += backendsandbox.cpp \
 backendserver.cpp \
 mediaprofiles.cpp \
 mediaserver.cpp \
 mediaserver.html.cpp \
 mediastream.cpp \
 teletext.cpp

HEADERS += lximediacenterprivate.h
SOURCES += lximediacenterprivate.cpp

unix {
  LIBS += -lz

  !macx {
    target.path = /usr/lib
    INSTALLS += target
  }
}

win32 {
  LIBS += -lws2_32
}

# Windows specific
win32 {
  OUT_DIR = $$replace(OUT_PWD,/,\\)\\$$replace(LXIMEDIA_DIR,/,\\)\\bin

  system(mkdir $${OUT_DIR} > NUL 2>&1)

  release {
    system(copy /Y $$(QTDIR)\\bin\\Qt5Gui.dll $${OUT_DIR} > NUL)
    system(copy /Y $$(QTDIR)\\bin\\Qt5Network.dll $${OUT_DIR} > NUL)
    system(copy /Y $$(QTDIR)\\bin\\Qt5Test.dll $${OUT_DIR} > NUL)
    system(copy /Y $$(QTDIR)\\bin\\libGLESv2.dll $${OUT_DIR} > NUL)
  }
  debug {
    system(copy /Y $$(QTDIR)\\bin\\Qt5Guid.dll $${OUT_DIR} > NUL)
    system(copy /Y $$(QTDIR)\\bin\\Qt5Networkd.dll $${OUT_DIR} > NUL)
    system(copy /Y $$(QTDIR)\\bin\\Qt5Testd.dll $${OUT_DIR} > NUL)
    system(copy /Y $$(QTDIR)\\bin\\libGLESv2d.dll $${OUT_DIR} > NUL)
  }

  system(copy /Y $$(QTDIR)\\bin\\D3DCompiler_43.dll $${OUT_DIR} > NUL)

  win32-msvc2005|win32-msvc2008|win32-msvc2010 {
    TEMPLATE = vclib
    GUID = f4e525ea-7365-11e0-aef3-cba632b16f97
    DEFINES += _CRT_SECURE_NO_WARNINGS
    PRECOMPILED_SOURCE = $${PRECOMPILED_HEADER}
  }
}
