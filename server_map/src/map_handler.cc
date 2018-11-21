#include <QFileInfo>
#include <QDir>
#include <QDateTime>
#include <sstream>
#include <stdlib.h>
#include <time.h>
#include "map_handler.hh"

/////////////////////////////////////////////////////////////////////////////////////////////////////
//MapHandler::MapHandler
/////////////////////////////////////////////////////////////////////////////////////////////////////

MapHandler::MapHandler(QSettings* settings, QObject* parent) :
  HttpRequestHandler(parent)
{
  encoding = settings->value("encoding", "UTF-8").toString();
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
//MapHandler::service
/////////////////////////////////////////////////////////////////////////////////////////////////////

void MapHandler::service(HttpRequest& request, HttpResponse& response)
{
  response.setHeader("Content-Type", qPrintable("text/html; charset=" + encoding));
  QByteArray path = request.getPath();
  if (QFileInfo(docroot + path).isDir())
  {
    path += "/index.leaflet.mapbox.html";
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
  service_realtime(response);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//MapHandler::service_map
/////////////////////////////////////////////////////////////////////////////////////////////////////

void MapHandler::service_map(HttpResponse& response)
{
  std::string mapbox_key = "MY_KEY";
  double lat = 38.9072;
  double lon = -77.0369;
  int zoom = 13;
  tile_provider_t tile = tile_provider_t::CARTODB;
  std::string str_z = std::to_string((int)zoom);
  std::string ll;
  ll = std::to_string((long double)lat);
  ll += ",";
  ll += std::to_string((long double)lon);
  QString body;
  std::ostringstream strm;
  if (tile == tile_provider_t::MAPBOX)
  {
    strm
      << "<script>";
    strm
      << "  var map = L.map('map').setView([" << ll << "], " << str_z << ");\n";
    strm
      << "  var gl = L.mapboxGL({\n"
      << "  accessToken:'" << mapbox_key << "',\n"
      << "  style: 'mapbox://styles/mapbox/streets-v9'\n"
      << "  }).addTo(map);\n";
    strm
      << "</script>";
  }
  if (tile == tile_provider_t::CARTODB)
  {
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
  }
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
    << "radius: "
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
  bool fit_bounds = false;
  std::string icon_url = "https://cdn.rawgit.com/pointhi/leaflet-color-markers/master/img/marker-icon-2x-violet.png";
  std::string shadow_url = "https://cdnjs.cloudflare.com/ajax/libs/leaflet/0.7.7/images/marker-shadow.png";
  QString body;
  std::ostringstream strm;
  strm
    << "<script>";
  strm
    << "var custom_icon = new L.Icon({"
    << "iconUrl: '"
    << icon_url
    << "',"
    << "shadowUrl: '"
    << shadow_url
    << "',"
    << "iconSize: [25, 41],"
    << "iconAnchor: [12, 41],"
    << "popupAnchor: [1, -34],"
    << "shadowSize: [41, 41]"
    << "});\n"
    << "var realtime = L.realtime('http://www.eden-earth.org:9002/', {"
    << "interval: 6 * 1000,"
    //pointToLayer, replace marker with circle
    << "pointToLayer: function (feature, latlng) {"
    << "  var c = feature.geometry.coordinates;"
    << "  var circle = L.circle([c[1], c[0]], {"
    << "    color: '#ff0000',"
    << "    stroke: false,"
    << "    radius: feature.properties.id"
    << "  });"
    << "  return circle;"
    << "},"
    //add marker on onEachFeature
    << "onEachFeature(feature, layer) {"
    << "  var c = feature.geometry.coordinates;"
    << "  var marker = L.marker([c[1], c[0]], {icon: custom_icon}).addTo(map);"
    << "  marker.bindPopup(feature.properties.id).openPopup();"
    << " }\n"
    << "}).addTo(map);\n"
    << "realtime.on('update', function() {";
  if (fit_bounds)
  {
    strm << "map.fitBounds(realtime.getBounds(), {maxZoom: 13});";
  }
  strm
    << "});"
    << "</script>";
  body += strm.str().c_str();
  qDebug() << body;
  response.write(strm.str().c_str());
}
