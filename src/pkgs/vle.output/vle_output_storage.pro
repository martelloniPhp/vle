include(../../../defaults.pri)

CONFIG += c++14
CONFIG += thread
CONFIG += plugin
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += object_parallel_to_source

TEMPLATE = lib

TARGET = storage

SOURCES = Storage.cpp

target.path = $$LIBSDIR/pkgs/vle.output/plugins/output

INSTALLS += target
