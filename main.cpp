#include "mainwindow.h"
#include "general.h"


int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  General::start();
  qint32 ret = a.exec();
  return ret;
}
