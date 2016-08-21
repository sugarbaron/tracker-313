/*! \file sqlexchange.h
 *  \brief интерфейс модуля взаимодействия с sql-сервером
 *  \author sugarbaron ([bravery_and_stupidity] e-mail: sugarbaron1@mail.ru)
 *  \date 22.07.2016
 */
#ifndef SQLEXCHANGE
#define SQLEXCHANGE

#include <QString>
#include <QVariant>

/*! \typedef SqlRecord
 *  \brief   тип, описывающий запись sql-таблицы. 
 *           под записью понимается строка таблицы.
 */
typedef QList<QVariant> SqlRecord;

/*! \class SqlExchange
 *  \brief интерфейс модуля взаимодействия с sql-сервером
 */
class SqlExchange
{
  public: virtual void getReadyFor(const QString& queryText) = 0;
  public: virtual void loadPlaceholder(const QString& placeholder, const QVariant& value) = 0;
  public: virtual QList<SqlRecord> executeQuery() = 0;
  
  public: virtual void beginTransaction() = 0;
  public: virtual void commitTransaction() = 0;
  public: virtual void rollbackTransaction() = 0;
  
  public: virtual void disconnect() = 0;
};
#endif // SQLEXCHANGE

