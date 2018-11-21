#ifndef _MAP_HANDLER_H
#define _MAP_HANDLER_H
#include "httprequesthandler.h"

using namespace stefanfrings;

enum class tile_provider_t
{
  CARTODB, MAPBOX
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
//MapHandler
/////////////////////////////////////////////////////////////////////////////////////////////////////

class MapHandler : public HttpRequestHandler
{
  Q_OBJECT
    Q_DISABLE_COPY(MapHandler)
public:
  MapHandler(QSettings* settings, QObject* parent = NULL);
  ~MapHandler();
  void service(HttpRequest& request, HttpResponse& response);

private:
  void service_map(HttpResponse& response);
  void service_circle(HttpResponse& response, int radius);
  void service_realtime(HttpResponse& response);
  QString docroot;
  QString encoding;
};

#endif // _MAP_HANDLER_H
