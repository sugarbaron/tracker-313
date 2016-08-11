#include "project.h"
#include "sqlhardworker.h"
#include "general.h"

Project::Project():_id(0)
{}

Project::Project(quint32 id, QString name):_id(id),_name(name)
{}

bool Project::operator ==(quint32 id) const
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

quint32 Project::getId() const
{
  return _id;
}

QString Project::getName() const
{
  return _name;
}

void Project::setId(quint32 id)
{
  _id = id;
}

void Project::setName(QString name)
{
  _name = name;
}

bool Project::isValid() const
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

void Project::synhronizeWithDb(quint32 id)
{
  SqlHardWorker* sqlHardWorker = General::getSqlHardWorker();
  *this = sqlHardWorker->getProject(id);
}

bool Project::isEqual(const Project& other) const
{
  bool areEqual = true;
  QString otherOrderName = other.getName();
  if(_name != otherOrderName)
  {
    areEqual = false;
  }
  return areEqual;
}
