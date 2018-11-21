TARGET = server_json
TEMPLATE = app
QT = core network
CONFIG += console
HEADERS += src/json_handler.hh
SOURCES += src/main.cc src/json_handler.cc

include(../QtWebApp/logging/logging.pri)
include(../QtWebApp/httpserver/httpserver.pri)
include(../QtWebApp/templateengine/templateengine.pri)

