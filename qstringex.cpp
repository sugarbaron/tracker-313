#include "qstringex.h"
#include "exception.h"

QStringEx::QStringEx()
{}

QStringEx::QStringEx(const QString & other)
{
  *this = other;
}

quint32 QStringEx::toUIntEx(quint32 base)
{
  bool ok = false;
  quint32 ret = toUInt(&ok, base);
  if(!ok)
  {
    qCritical()<<"unconverted string = "<<*this;
    throw NeedFixCode("can't convert to uint");
  }
  return ret;
}

qint32 QStringEx::toIntEx(qint32 base)
{
  bool ok = false;
  qint32 ret = toInt(&ok, base);
  if(!ok)
  {
    qCritical()<<"unconverted string = "<<*this;
    throw NeedFixCode("can't convert to int");
  }
  return ret;
}

