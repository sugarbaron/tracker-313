#include "general.h"
#include "exception.h"
#include "clockunit.h"
#include "settingsunit.h"
#include "smtphardworker.h"
#include "sqlhardworkerunit.h"
#if TURN_ON_UNIT_TESTS
#include "sqlhardworker_tester.h"
#endif//TURN_ON_UNIT_TESTS

void General::controllerInit()
{
  // Инициализируем подсистему времени
  _systemClock = ClockUnit::create();

  // Устанавливаем функцию-обработчик сообщений лога
  qInstallMessageHandler(logMessageOutput);
  qDebug()<<"created module: clock";
  qDebug()<<"created module: log";
  // создаём модуль извлечения параметров из настроечного файла
  _settings = SettingsUnit::create();
  qDebug()<<"created module: settings";
  
  // создаём модуль, выполняющий работу по взаимодействию с sql-сервером
  SqlHardWorkerUnit::create();
  qDebug()<<"created module: sql";
  _sqlHardWorker = SqlHardWorkerUnit::getInstance();
  #if TURN_ON_UNIT_TESTS
  _sqlDatabaseCreator = SqlHardWorkerUnit::getInstance();
  #endif//TURN_ON_UNIT_TESTS
  // синхронизируем системное время с sql-сервером
  _sqlHardWorker->synchronizeDateTime();
  // сообщаем стейтмашине инструмент для работы с sql-базой
  _guiSm->rememberSqlTool(_sqlHardWorker);
  qDebug()<<"statemachine receives sql module";
  
  // получаем реквизиты для подключения к ftp-серверу
  QString ftpIp = _settings->getFtpIp();
  quint32 ftpPort  = _settings->getFtpPort();
  QString ftpLogin = _settings->getFtpLogin();
  QString ftpPass  = _settings->getFtpPassword();
  // создаём модуль, выполняющий работу по взаимодействию с ftp-сервером
  _ftp = FtpHardWorker::getInstance();
  qDebug()<<"created module: ftp";
  //_ftp->setLocalTempPath(qApp->applicationDirPath()+ "/temp/");
  _ftp->rememberConnectionSettings(ftpIp, ftpLogin, ftpPass, ftpPort);
  _ftp->connectToFtpServer(ftpIp, ftpLogin, ftpPass, ftpPort);
  qDebug()<<"connected to ftp-server";

  #if TURN_ON_UNIT_TESTS
  SqlHardWorker_Tester::run();
  #endif//TURN_ON_UNIT_TESTS
}

void General::start()
{
  // Выделяем память под поле _guiSm
  _guiSm.reset(new GuiStateMachine);

  // Запускаем стейтмашину
  _guiSm->smStart();
}

void General::setRegistredUserId(quint32 registredUserId)
{
  _registredUserId = registredUserId;
}

quint32 General::getRegistredUserId()
{
  return _registredUserId;
}

Clock* General::getSystemClock()
{
  return _systemClock;
}

Settings* General::getSettings()
{
  return _settings;
}

SqlHardWorker* General::getSqlHardWorker()
{
  return _sqlHardWorker;
}

#if TURN_ON_UNIT_TESTS
SqlDatabaseCreator* General::getSqlDatabaseCreator()
{
  return _sqlDatabaseCreator;
}
#endif//TURN_ON_UNIT_TESTS

FtpHardWorker* General::getFtpHardWorker()
{
  return _ftp;
}

//секция_инициализации_статических_членов_класса_______________________________
///////////////////////////////////////////////////////////////////////////////
quint32 General::_registredUserId = 0;
Clock*  General::_systemClock;
Settings* General::_settings;
FtpHardWorker* General::_ftp;
SqlHardWorker* General::_sqlHardWorker;
#if TURN_ON_UNIT_TESTS
SqlDatabaseCreator* General::_sqlDatabaseCreator;
#endif//TURN_ON_UNIT_TESTS
QScopedPointer<GuiStateMachine> General::_guiSm;

