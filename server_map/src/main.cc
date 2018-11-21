#include <QCoreApplication>
#include <QDir>
#include "httplistener.h"
#include "map_handler.hh"

/////////////////////////////////////////////////////////////////////////////////////////////////////
//main
/////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
  QCoreApplication* app = new QCoreApplication(argc, argv);
  QString path = QCoreApplication::applicationDirPath();
  path += "/../etc";
  QString file_name_config;
  QFile file(path + "/" + "map_config.ini");
  if (file.exists())
  {
    file_name_config = QDir(file.fileName()).canonicalPath();
    qDebug("Using config file %s", qPrintable(file_name_config));
  }
  QSettings* settings = new QSettings(file_name_config, QSettings::IniFormat, app);
  settings->beginGroup("listener");
  MapHandler* handler = new MapHandler(settings, app);
  new HttpListener(settings, handler, app);
  return app->exec();
}