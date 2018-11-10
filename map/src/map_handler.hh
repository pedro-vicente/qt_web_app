#ifndef _MAP_HANDLER_H
#define _MAP_HANDLER_H
#include "httprequesthandler.h"
#include <QBasicTimer>

using namespace stefanfrings;

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
  void service_map(HttpRequest& request, HttpResponse& response);
  void service_circle(HttpRequest& request, HttpResponse& response, int radius);
  QString docroot;
  void timerEvent(QTimerEvent *event) override;
  QBasicTimer timer;
};

#endif // _MAP_HANDLER_H
