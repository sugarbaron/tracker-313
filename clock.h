/*! \file clock.h
 *  \brief интерфейс системных часов
 *  \author sugarbaron ([sugarbaron_bicycles] e-mail: sugarbaron1@mail.ru)
 *  \date 23.07.2016
 */
#ifndef CLOCK
#define CLOCK

#include <QDateTime>

class Clock
{
  public: virtual QDateTime getTime() const = 0;
  public: virtual void setTime(const QDateTime& newValue) = 0;
  public: virtual void msThreadSleep(quint32 ms) const = 0;
};
#endif // CLOCK

