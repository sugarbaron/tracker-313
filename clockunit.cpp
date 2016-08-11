#include "clockunit.h"

/* создать экземпляр часов */
ClockUnit* ClockUnit::create()
{
  bool isNotAlreadyCreated = _instance.isNull();
  if(isNotAlreadyCreated)
  {
    _instance.reset( new ClockUnit );
    _instance->_time.reset( new QDateTime );
    _instance->_timeLocker.reset( new QMutex );
    _instance->_sleepLocker.reset( new QMutex );
    _instance->_ticktacker.reset( new QTimer );
    QDateTime* timeFieldPointer = _instance->_time.data();
    *timeFieldPointer = QDateTime::currentDateTime();
    _instance->start();
  }
  else
  {
    QString errorText = "this method must be invoked only once";
    qCritical()<<errorText;
    throw NeedFixCode(errorText);
  }
  return _instance.data();
}

ClockUnit::ClockUnit()
{}

ClockUnit::~ClockUnit()
{}

/* получить значение системного времени */
QDateTime ClockUnit::getTime() const
{
  _timeLocker->lock();
  QDateTime result = *( _time.data() );
  _timeLocker->unlock();
  return result;
}

/* установить значение системного времени */
void ClockUnit::setTime(const QDateTime& newValue)
{
  if( !(newValue.isValid()) )
  {
    QString errorText = "invalid agument";
    qCritical()<<errorText;
    throw NeedFixCode(errorText);
  }
  
  _timeLocker->lock();
  *( _time.data() ) = newValue;
  _timeLocker->unlock();
}


/* инкрементировать время и дату на TIMER_TICK миллисекунд */
void ClockUnit::incrementTime()
{
  QDateTime newValue = getTime();
  newValue = newValue.addMSecs(TIMER_TICK);
  setTime(newValue);
  return;
}

/* стартануть часы */
void ClockUnit::start()
{
  //[коннектим к тику таймера инкрементирование времени]
  QObject::connect( _ticktacker.data(), SIGNAL(timeout()), this, SLOT(incrementTime()) );
  //[стартуем таймер]
  _ticktacker->start(TIMER_TICK);
  return;
}

/* приостановка выполнения потока на заданное время */
void ClockUnit::msThreadSleep(quint32 ms) const
{
  _sleepLocker->lock();
  QTimer timer;
  QEventLoop loop;
  timer.setSingleShot(true);
  timer.setInterval(ms);
  connect(&timer,SIGNAL(timeout()),&loop,SLOT(quit()));
  timer.start();
  loop.exec();
  _sleepLocker->unlock();
  return;
}

//секция_создания_статических_членов_______________________________________
///////////////////////////////////////////////////////////////////////////
QScopedPointer<ClockUnit> ClockUnit::_instance;
