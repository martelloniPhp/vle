include(../../../defaults.pri)

CONFIG += c++14
CONFIG += thread
CONFIG -= app_bundle
CONFIG += qt

QT += core
QT += gui
QT += widgets
QT += help
QT += xml

TEMPLATE = app

TARGET = gvle

SOURCES = main.cpp

mac {
  QMAKE_CXXFLAGS += -iwithsysroot /usr/include/libxml2
  LIBS += -lxml2
  LIBS += -L../../../src -llibvle
  LIBS += -L../../../src/vle/gvle -llibgvle
}
win32 {
}
unix {
  PKGCONFIG += libxml-2.0
  CONFIG += link_pkgconfig
  LIBS += -L../../../src -lvle-2.0
  LIBS += -L../../../src/vle/gvle -lgvle-2.0
}

target.path = $$BINDIR

INSTALLS += target
