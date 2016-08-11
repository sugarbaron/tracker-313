/*! \file sqlexchangeunit.h
 *  \brief класс реализации модуля взаимодействия с sql-сервером
 *  \author sugarbaron ([bravery_and_stupidity] e-mail: sugarbaron1@mail.ru)
 *  \date 22.07.2016
 */
#ifndef SQLEXCHANGEUNIT
#define SQLEXCHANGEUNIT

#include "sqlexchange.h"
#include <QScopedPointer>
#include <QSqlQuery>

/*! \class SqlExchangeUnit
 *  \brief класс реализации модуля взаимодействия с sql-сервером.
 *         дополнительно несёт функции обёртки для QSqlQuery и QSqlDatabase
 */
class SqlExchangeUnit : public SqlExchange
{
  //секция_данных______________________________________________________________
  /////////////////////////////////////////////////////////////////////////////
  private: static const quint32 TRIES_QUANTITY = 4;
  private: static const quint32 PARRY_PAUSE = 200;
  
  private: static QScopedPointer<SqlExchangeUnit> _thisUnit;
  
  private: QSqlDatabase _database;
  private: QSqlQuery    _query;
  private: quint32 _fieldsQuantity;
  
  //секция_методов_____________________________________________________________
  /////////////////////////////////////////////////////////////////////////////
  public:  static SqlExchangeUnit* create(const QString& ip, const quint32 port, const QString& login, const QString& password); 
  private: static void checkUnitAbsence();
  private: SqlExchangeUnit();
  private: void initDatabase(const QString& ip, const quint32 port, const QString& login, const QString& password);
  private: void connect();
  private: void initQuery();
  public: virtual ~SqlExchangeUnit();
  
  public:  void prepareQuery(const QString& queryText);
  
  public:  void bindValue(const QString& placeholder, const QVariant& value);
  
  public:  QList<SqlRecord> executeQuery();
  private: QList<QVariant> executeQueryWithParry() const;
  private: void makeTriesOfQueryExecution();
  private: void handleNegativeTry();
  private: QList<SqlRecord> getQueryResult();
  private: void checkRecord() const;
  private: void checkField(const QVariant& field) const;
  private: SqlRecord extractCurrentRecord() const;
  
  public:  void beginTransaction();
  public:  void commitTransaction();
  public:  void rollbackTransaction();
  
  public: void disconnect();
};
#endif // SQLEXCHANGEUNIT

