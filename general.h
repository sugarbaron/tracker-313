/*! \file General.h
 *  \brief Описание интерфейса класса General
 *  \author Бубенщиков А.И.
 *  \date 09.12.2015
 */

#ifndef General_H
#define General_H

#include <QObject>
#include <QApplication>
#include <QMainWindow>
#include "msgloghandler.h"
#include "mainwindow.h"
#include "guistatemachine.h"
#include "worker.h"
#include "task.h"
#include "clock.h"
#include "settings.h"
#include "ftphardworker.h"
#include "sqlhardworker.h"
#include "sqldatabasecreator.h"

#define TURN_ON_UNIT_TESTS 0

/*! \brief Класс-main (абстрактный)
 */
class General : public QObject
{
  Q_OBJECT
  private:
    static QScopedPointer<GuiStateMachine> _guiSm;
    static Settings* _settings;
    static FtpHardWorker* _ftp;
    static SqlHardWorker* _sqlHardWorker;
    static Clock* _systemClock;
    /// Id вошедшего в систему работника
    static quint32 _registredUserId;
    
    #if TURN_ON_UNIT_TESTS
    static SqlDatabaseCreator* _sqlDatabaseCreator;
    #endif//#if TURN_ON_UNIT_TESTS

    General();
    static qint32 stMachineInit();

  public:
    /// Код возвращаемый при аварийном завершении работы приложения на этапе инициализации
    static const qint32 critErrorExitCode = -1;

    /// Метод запуска контроллера
    static void start();

    /// Метод инициализации
    static void controllerInit();
    
    static Clock* getSystemClock();
    
    static Settings* getSettings();
    
    static SqlHardWorker* getSqlHardWorker();
    
    #if TURN_ON_UNIT_TESTS
    static SqlDatabaseCreator* getSqlDatabaseCreator();
    #endif//TURN_ON_UNIT_TESTS
    
    static FtpHardWorker* getFtpHardWorker();
    
    

    /// Возвращает id залогинившегося пользователя
    static quint32 getRegistredUserId();

    /// Устанавливает id залогинившегося пользователя
    static void setRegistredUserId(quint32 registredUserId);
};

#endif // General_H
