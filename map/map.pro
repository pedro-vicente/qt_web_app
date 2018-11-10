TARGET = Map
TEMPLATE = app
QT = core network
CONFIG += console
HEADERS += src/map_handler.hh
SOURCES += src/main.cc src/map_handler.cc

include(../QtWebApp/logging/logging.pri)
include(../QtWebApp/httpserver/httpserver.pri)
include(../QtWebApp/templateengine/templateengine.pri)

