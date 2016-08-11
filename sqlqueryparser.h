/*! \file sqlqueryparser.h
 *  \brief статический модуль парсинга текста запроса
 *  \author sugarbaron ([bravery_and_stupidity] e-mail: sugarbaron1@mail.ru)
 *  \date 04.08.2016
 */
#ifndef SQLQUERYPARSER
#define SQLQUERYPARSER

#include <QString>

/*! \brief статический модуль парсинга текста запроса */
class SqlQueryParser
{
  public:  static quint32 calculateFields(const QString& queryText);
  private: static const QString extractFieldsSubstring(const QString& queryText);
};
#endif // SQLQUERYPARSER

