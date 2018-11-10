#include <QFileInfo>
#include <QDir>
#include <QDateTime>
#include <sstream>
#include "map_handler.hh"

/////////////////////////////////////////////////////////////////////////////////////////////////////
//MapHandler::MapHandler
/////////////////////////////////////////////////////////////////////////////////////////////////////

MapHandler::MapHandler(QSettings* settings, QObject* parent) :
  HttpRequestHandler(parent)
{
  docroot = settings->value("path", ".").toString();
  if (!(docroot.startsWith(":/") || docroot.startsWith("qrc://")))
  {
#ifdef Q_OS_WIN32
    if (QDir::isRelativePath(docroot) && settings->format() != QSettings::NativeFormat)
#else
    if (QDir::isRelativePath(docroot))
#endif
    {
      QFileInfo configFile(settings->fileName());
      docroot = QFileInfo(configFile.absolutePath(), docroot).absoluteFilePath();
    }
  }
  timer.start(6000, this);
  qDebug("MapHandler: created");
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//MapHandler::~MapHandler
/////////////////////////////////////////////////////////////////////////////////////////////////////

MapHandler::~MapHandler()
{
  qDebug("MapHandler: deleted");
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//MapHandler::timerEvent
/////////////////////////////////////////////////////////////////////////////////////////////////////

void MapHandler::timerEvent(QTimerEvent *)
{

}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//MapHandler::service
/////////////////////////////////////////////////////////////////////////////////////////////////////

void MapHandler::service(HttpRequest& request, HttpResponse& response)
{
  QByteArray path = request.getPath();
  if (QFileInfo(docroot + path).isDir())
  {
    path += "/index.leaflet.html";
  }
  QFile file(docroot + path);
  qDebug("MapHandler::service: Open file %s", qPrintable(file.fileName()));
  if (file.open(QIODevice::ReadOnly))
  {
    qDebug("MapHandler::service: Opened file %s", qPrintable(file.fileName()));
    while (!file.atEnd() && !file.error())
    {
      response.write(file.read(65536));
    }
  }
  file.close();
  service_map(response);
  service_circle(response, 300);
  service_realtime(response);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//MapHandler::service_map
/////////////////////////////////////////////////////////////////////////////////////////////////////

void MapHandler::service_map(HttpResponse& response)
{
  QString body;
  std::ostringstream strm;
  strm
    << "<script>"
    << "var layer_base = L.tileLayer(\n"
    << "'http://cartodb-basemaps-{s}.global.ssl.fastly.net/light_all/{z}/{x}/{y}@2x.png',{\n"
    << "opacity: 1\n"
    << "});\n"
    << "var map = new L.Map('map', {\n"
    << "center: new L.LatLng(38.9072, -77.0369),\n"
    << "zoom: 13,\n"
    << "layers: [layer_base]\n"
    << "});\n"
    << "</script>";
  body += strm.str().c_str();
  qDebug() << body;
  response.write(strm.str().c_str());
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//MapHandler::service_circle
/////////////////////////////////////////////////////////////////////////////////////////////////////

void MapHandler::service_circle(HttpResponse& response, int radius)
{
  QString body;
  std::ostringstream strm;
  strm
    << "<script>"
    << "var circle = L.circle([38.9072, -77.0369], {"
    << "color: '#ff0000',"
    << "stroke: false,"
    << "radius : "
    << std::to_string(radius);
  strm
    << "}).addTo(map);"
    << "</script>";
  body += strm.str().c_str();
  qDebug() << body;
  response.write(strm.str().c_str());
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
//MapHandler::service_realtime
/////////////////////////////////////////////////////////////////////////////////////////////////////

void MapHandler::service_realtime(HttpResponse& response)
{
  QString body;
  std::ostringstream strm;
  strm
    << "<script>"
    << "var realtime = L.realtime('https://wanderdrone.appspot.com/', {"
    << "interval: 5 * 1000"
    << "}).addTo(map);"
    << "realtime.on('update', function() {"
    << "map.fitBounds(realtime.getBounds(), {maxZoom: 3});"
    << "});"
    << "</script>";
  body += strm.str().c_str();
  qDebug() << body;
  response.write(strm.str().c_str());
}

