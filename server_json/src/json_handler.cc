#include <QFileInfo>
#include <QDir>
#include <QDateTime>
#include <sstream>
#include <stdlib.h>
#include <time.h>
#include "json_handler.hh"

/////////////////////////////////////////////////////////////////////////////////////////////////////
//JsonHandler::JsonHandler
/////////////////////////////////////////////////////////////////////////////////////////////////////

JsonHandler::JsonHandler(QSettings*, QObject* parent) :
  HttpRequestHandler(parent)
{
  srand(time(NULL));
  sensors.push_back(sensor_receive_t("dupont circle", 0, 38.907200, -77.036900, 0, 0));
  sensors.push_back(sensor_receive_t("glover park", 1, 38.9209761, -77.0947097, 0, 0));
  sensors.push_back(sensor_receive_t("woodridge", 2, 38.9324288, -76.9777218, 0, 0));
  qDebug("JsonHandler: created");
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//JsonHandler::~JsonHandler
/////////////////////////////////////////////////////////////////////////////////////////////////////

JsonHandler::~JsonHandler()
{
  qDebug("JsonHandler: deleted");
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//JsonHandler::service
/////////////////////////////////////////////////////////////////////////////////////////////////////

void JsonHandler::service(HttpRequest&, HttpResponse& response)
{
  response.setHeader("Access-Control-Allow-Origin", "*");
  std::ostringstream strm;
  int radius;

  //To be able to figure out when new features are added, when old features are removed, 
  //and which features are just updated, Leaflet Realtime needs to identify each feature uniquely. 
  //This is done using a feature id. Usually, this can be done using one of the feature's properties.
  //By default, Leaflet Realtime will try to look for a called property id and use that.

  strm
    << "{"
    << "\"type\": \"FeatureCollection\","
    << "\"features\": [";

  for (int idx = 0; idx < sensors.size(); idx++)
  {
    sensor_receive_t sensor = sensors.at(idx);
    strm
      << "{\"geometry\": {\"type\": \"Point\", \"coordinates\": ["
      << sensor.lon
      << ","
      << sensor.lat
      << "]},"
      << "\"type\": \"Feature\", \"properties\": "
      << "{\"id\": \"";
    radius = rand() % 1000 + 1;
    strm << std::to_string(radius);
    strm
      << "\"}}";

    if (idx < sensors.size() - 1)
    {
      strm
        << ","; //array separator of "features" object
    }
  }

  strm
    << "]}"; //end array of "features" object, end JSON 
  QString body = strm.str().c_str();
  qDebug() << body;
  response.write(strm.str().c_str());
  const QString path("out.json");
  QFile qFile(path);
  if (qFile.open(QIODevice::WriteOnly))
  {
    QTextStream out(&qFile); out << body;
    qFile.close();
  }
}

