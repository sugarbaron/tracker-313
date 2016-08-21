#include "msgloghandler.h"
#include "general.h"
#include <QApplication>
#include <QtCore>
#include <QMessageBox>


// Включен ли режим отладки
const int isDbgMode =  1;

void logMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
  QString dateTimeString;
  // Переменная для сохранения диагностики
  static QString logString;
  // Флаг, показывающий были ли сообщения с критическими ошибками
  static bool wasErrors = false;

  // Сделаем имя файла статиком, чтобы новый файл создавался только один раз при запуске приложения
  Clock* systemClock = General::getSystemClock();
  QDateTime currentTime = systemClock->getTime();
  QString currentTimeAsText = currentTime.toString("hh:mm:ss");
  QString currentDateTimeAsText = currentTime.toString("dd_MM_yyyy_hh_mm_ss");
  static QString logFileName = currentDateTimeAsText +"_dbg.log";
  dateTimeString = "[" + currentTimeAsText + "]";


  switch(type)
  {
    case QtDebugMsg:
      logString += QString("[debug] %1 [%2] %3").arg(dateTimeString).arg(context.function).arg(msg) + "\n";
      break;
    case QtInfoMsg:
      /*logString += QString("%1 [%2] [info] %3").arg(dateTimeString).arg(context.function).arg(msg) + "\n";*/
      break;
    case QtWarningMsg:
      logString += QString("[warn ] %1 [%2] %3").arg(dateTimeString).arg(context.function).arg(msg) + "\n";
      break;
    case QtCriticalMsg:
      logString += QString("[error] %1 [%2] %3").arg(dateTimeString).arg(context.function).arg(msg) + "\n";
      // Возводим флаг, что в протоколе есть ошибки
      wasErrors = true;
      break;
    case QtFatalMsg:
      logString += QString("[fatal] %1 [%2] %3").arg(dateTimeString).arg(context.function).arg(msg) + "\n";
      wasErrors = true;
      break;
  }
  // Если были сообщения об ошибках и не включен режим отладки, то сохраним лог в файл
  if((wasErrors)||(isDbgMode))
  {
    QDir logDir;
    QFile logFile;
    QTextStream out;
    // Создадим каталог для сохранения логов
    logDir.mkdir(qApp->applicationDirPath() + "\\logs");
    logDir.setPath(qApp->applicationDirPath() + "\\logs");
    logFile.setFileName(logDir.path() + "/" + logFileName);
    // Пытаемся открыть файл
    if(!logFile.open(QIODevice::Append|QIODevice::Text))
    {
      QMessageBox::critical(0, qApp->tr("ОШБИКА"),qApp->tr("Ошибка создания лог-файла"), QMessageBox::Ok);
      return;
    }
    out.setDevice(&logFile);
    // Пишем в файл
    out<<logString;
    // Очищаем то, что уже записали
    logString.clear();
    out.flush();
    logFile.flush();
    logFile.close();
  }
}
