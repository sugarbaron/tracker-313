#include "sqlqueryparser.h"
#include "exception.h"

quint32 SqlQueryParser::calculateFields(const QString& queryText)
{
  quint32 fielsQuantity = 0;
  bool isSelectQuery = queryText.startsWith("SELECT", Qt::CaseInsensitive);
  if( isSelectQuery )
  {
    QString fieldsSubstring = extractFieldsSubstring(queryText);
    QStringList fields = fieldsSubstring.split(",");
    fielsQuantity = fields.size();
  }
  return fielsQuantity;
}

const QString SqlQueryParser::extractFieldsSubstring(const QString& queryText)
{
  QStringList queryParts = queryText.split("FROM");
  quint32  partsQuantity = queryParts.size();
  if(partsQuantity == 0)
  {
    QString errorText = "invalid query. SELECT-query dosen't contain \"FROM\" statement:";
    qCritical()<<errorText;
    qCritical()<<queryText;
    throw NeedFixCode(errorText);
  }
  QString fieldsPart = queryParts.value(0);
  return fieldsPart;
}
