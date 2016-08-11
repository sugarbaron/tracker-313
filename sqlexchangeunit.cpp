#include "sqlexchangeunit.h"
#include "sqlqueryparser.h"
#include "exception.h"
#include "clock.h"
#include "general.h"
#include <QSqlError>

SqlExchangeUnit* SqlExchangeUnit::create
(const QString& ip, const quint32 port, const QString& login, const QString& password)
{
  checkUnitAbsence();
  
  _thisUnit.reset( new SqlExchangeUnit );
  
  _thisUnit->initDatabase(ip, port, login, password);
  _thisUnit->connect();
  _thisUnit->initQuery();
  _thisUnit->_fieldsQuantity = 0;
  return _thisUnit.data();
}

void SqlExchangeUnit::checkUnitAbsence()
{
  bool isAlreadyCreated = !( _thisUnit.isNull() );
  if(isAlreadyCreated)
  {
    QString errorText = "this method must be invoked only once";
    qCritical()<<errorText;
    throw NeedFixCode(errorText);
  }
  return;
}

SqlExchangeUnit::SqlExchangeUnit()
{}

void SqlExchangeUnit::initDatabase
(const QString& ip, const quint32 port, const QString& login, const QString& password)
{
  QString driverType = "QMYSQL";
  _database = QSqlDatabase::addDatabase(driverType, "Tracker313SqlConnection");
  bool isOk = _database.isValid();
  if( !isOk )
  {
    QString databaseErrorText = _database.lastError().text();
    QString errorText = "invalid driver for QSqlDatabase. " + 
                        driverType + " is not available or couid not be loaded. "
                        "databaseErrorText is #" + databaseErrorText;
    qCritical()<<errorText;
    throw CriticalOperationException(errorText);
  }
  _database.setHostName(ip);
  _database.setPort(port);
  _database.setUserName(login);
  _database.setPassword(password);
  return;
}

void SqlExchangeUnit::connect()
{
  bool isOk =_database.open();
  if( !isOk )
  {
    qCritical()<<"db connect error: " + _database.lastError().text();
    qCritical()<<"connection settings:"
                 " hostname #"  + _database.hostName() +
                 " port #"      + QString::number( (int)_database.port() ) +
                 " user name #" + _database.userName() +
                 " pass #"      + _database.password();
    throw ConnectException("db connect error");
  }
  qDebug()<<"connected to sql server";
  return;
}

void SqlExchangeUnit::initQuery()
{
  _query = QSqlQuery(_database);
}

SqlExchangeUnit::~SqlExchangeUnit()
{}

void SqlExchangeUnit::prepareQuery(const QString& queryText)
{
  _fieldsQuantity = SqlQueryParser::calculateFields(queryText);
  bool isOk = _query.prepare(queryText);
  if( !isOk )
  {
    QString errorText = "_query.prepare() error";
    QString diagnostics = _query.lastError().text();
    qCritical()<<errorText;
    qCritical()<<diagnostics;
    
    throw SqlQueryException(errorText);
  }
  return;
}

void SqlExchangeUnit::bindValue(const QString& placeholder, const QVariant& value)
{
  _query.bindValue(placeholder, value);
  return;
}

QList<SqlRecord> SqlExchangeUnit::executeQuery()
{
  makeTriesOfQueryExecution();
  QList<SqlRecord> queryResult = getQueryResult();
  return queryResult;
}

void SqlExchangeUnit::makeTriesOfQueryExecution()
{
  bool isOk = false;
  for(quint32 i=0; i<TRIES_QUANTITY; i++)
  {
    isOk = _query.exec();
    if(isOk)
    {
      return;
    }
    else
    {
      handleNegativeTry();
    }
  }
  //[все попытки исчерпаны. запрос не выполнен]
  QString errorText = "sql query execution error";
  qCritical()<<errorText;
  throw SqlQueryException(errorText);
}

void SqlExchangeUnit::handleNegativeTry()
{
  qWarning()<<"negative try of query executing";
  qWarning()<<"reason: "<<_query.lastError().text();
  
  Clock* clock = General::getSystemClock();
  disconnect();
  clock->msThreadSleep(PARRY_PAUSE);
  connect();
  return;
}

QList<SqlRecord> SqlExchangeUnit::getQueryResult()
{
  QList<SqlRecord> queryResult;
  SqlRecord currentRecord;
  while( _query.next() )
  {
    checkRecord();
    currentRecord = extractCurrentRecord();
    queryResult.push_back(currentRecord);
  }
  return queryResult;
}

void SqlExchangeUnit::checkRecord() const
{
  QVariant field;
  for(quint32 i=0; i<_fieldsQuantity; i++)
  {
    field = _query.value(i);
    checkField(field);
  }
  return;
}

void SqlExchangeUnit::checkField(const QVariant& field) const
{
  bool isInvalid = true;
  isInvalid = !( field.isValid() );
  if(isInvalid)
  {
    QString errorText = "(strange_shit)invalid information from database";
    qCritical()<<errorText;
    throw CriticalOperationException(errorText);
  }
  return;
}

SqlRecord SqlExchangeUnit::extractCurrentRecord() const
{
  SqlRecord currentRecord;
  QVariant field;
  for(quint32 i=0; i<_fieldsQuantity; i++)
  {
    field = _query.value(i);
    currentRecord.push_back(field);
  }
  return currentRecord;
}

void SqlExchangeUnit::beginTransaction()
{
  bool isOk = _database.transaction();
  if( !isOk )
  {
    QString errorText = "begin transaction error";
    qCritical()<<errorText;
    qCritical()<<_database.lastError().text();
    throw SqlQueryException(errorText);
  }
  return;
}

void SqlExchangeUnit::commitTransaction()
{
  bool isOk = _database.commit();
  if( !isOk )
  {
    QString errorText = "commit transaction error";
    qCritical()<<errorText;
    qCritical()<<_database.lastError().text();
    throw SqlQueryException(errorText);
  }
  return;
}

void SqlExchangeUnit::rollbackTransaction()
{
  qDebug()<<"rollbacking transaction";
  bool isOk = _database.rollback();
  if( !isOk )
  {
    QString errorText = "commit transaction error";
    qCritical()<<errorText;
    qCritical()<<_database.lastError().text();
    throw SqlQueryException(errorText);
  }
  return;
}

void SqlExchangeUnit::disconnect()
{
  _database.close();
  qWarning()<<"disconnected from sql-server";
  return;
}

//секция_определения_статических_членов_класса_________________________________
///////////////////////////////////////////////////////////////////////////////
QScopedPointer<SqlExchangeUnit> SqlExchangeUnit::_thisUnit;
