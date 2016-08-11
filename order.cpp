#include "order.h"
#include "sqlhardworker.h"
#include "general.h"

Order::Order():_id(0)
{}

Order::Order(quint32 id, QString name):_id(id),_name(name)
{}

bool Order::operator ==(quint32 id) const
{
  if(this->_id == id)
  {
    return true;
  }
  else
  {
    return false;
  }
}

quint32 Order::getId() const
{
  return _id;
}

QString Order::getName() const
{
  return _name;
}

void Order::setId(quint32 id)
{
  _id = id;
}

void Order::setName(QString name)
{
  _name = name;
}

bool Order::isValid() const
{
  if(!_name.isEmpty())
  {
    return true;
  }
  else
  {
    return false;
  }
}

void Order::synhronizeWithDb(quint32 id)
{
  SqlHardWorker* sqlHardWorker = General::getSqlHardWorker();
  *this = sqlHardWorker->getOrder(id);
}

bool Order::isEqual(const Order& other) const
{
  bool areEqual = true;
  
  QString otherOrderName = other.getName();
  
  if(_name != otherOrderName)
  {
    areEqual = false;
  }
  return areEqual;
}
