#include "worker.h"
#include "exception.h"
#include "general.h"

Worker::Worker() :_id(0),
                  _surname(""),
                  _name(""),
                  _patronymic(""),
                  _position(""),
                  _email(""),
                  _login(""),
                  _pass(""),
                  _idVice(0),
                  _isGod(false)
{}

bool Worker::operator ==(const Worker &w) const
{
  if(this->_id == w._id)
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool Worker::operator ==(const quint32 id) const
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

quint32 Worker::getId() const
{
  return _id;
}

QString Worker::getName() const
{
  return _name;
}

QString Worker::getSurname() const
{
  return _surname;
}

QString Worker::getPatronymic() const
{
  return _patronymic;
}

QString Worker::getFullName() const
{
  if(!this->isValid())
  {
    qCritical()<<"try get full name, but object worker not valid";
    throw NeedFixCode("need fix code");
  }
  QString fullName = _surname + " " + _name + " " + _patronymic;
  return fullName;
}

QString Worker::getShortName() const
{
  if(!this->isValid())
  {
    qCritical()<<"try get short name, but object worker not valid";
    throw NeedFixCode("need fix code");
  }
  QString shortName = _surname + " " + _name.at(0).toUpper() + "." + _patronymic.at(0).toUpper() + ".";
  return shortName;
}

QString Worker::getPosition() const
{
  return _position;
}

QString Worker::getEmail() const
{
  return _email;
}

QString Worker::getLogin() const
{
  return _login;
}

QString Worker::getPass() const
{
  return _pass;
}

quint32 Worker::getVice() const
{
  return _idVice;
}

bool Worker::isGod() const
{
  return _isGod;
}

void Worker::setId(quint32 id)
{
  _id = id;
}

void Worker::setName(QString name)
{
  _name = name;
}

void Worker::setSurname(QString surname)
{
  _surname = surname;
}

void Worker::setPatronymic(QString patronymic)
{
  _patronymic = patronymic;
}

void Worker::setPosition(QString position)
{
  _position = position;
}

void Worker::setEmail(QString email)
{
  _email = email;
}

void Worker::setLogin(QString login)
{
  _login = login;
}

void Worker::setPass(QString pass)
{
  _pass = pass;
}

void Worker::setVice(quint32 idVice)
{
  _idVice = idVice;
}

void Worker::setIsGod(bool isGod)
{
  _isGod = isGod;
}

bool Worker::isValid() const
{
  if((!_name.isEmpty())
     &&(!_surname.isEmpty())
     &&(!_patronymic.isEmpty())
     &&(!_position.isEmpty())
     &&(!_email.isEmpty())
     &&(!_login.isEmpty())
     &&(!_pass.isEmpty()))
  {
    return true;
  }
  else
  {
    return false;
  }
}

void Worker::synhronizeWithDb(quint32 id)
{
  SqlHardWorker* sqlHardWorker = General::getSqlHardWorker();
  *this = sqlHardWorker->getWorker(id);
}

bool Worker::isEqual(const Worker& other) const
{
  bool areEqual = true;
  
  QString otherWorkerEmail  = other.getEmail();
  QString otherWorkerLogin  = other.getLogin();
  QString otherWorkerPass   = other.getPass();
  quint32 otherWorkerIdVice = other.getVice();
  bool    otherWorkerIsGod  = other.isGod();
  QString otherWorkerName   = other.getName();
  QString otherWorkerSurname = other.getSurname();
  QString otherWorkerPatronymic = other.getPatronymic();
  QString otherWorkerPosition = other.getPosition();
  
  if( (_email  != otherWorkerEmail)||
      (_login  != otherWorkerLogin)||
      (_pass   != otherWorkerPass)||
      (_idVice != otherWorkerIdVice)||
      (_isGod  != otherWorkerIsGod)||
      (_name   != otherWorkerName)||
      (_surname != otherWorkerSurname)||
      (_patronymic != otherWorkerPatronymic)||
      (_position != otherWorkerPosition)
      )
  {
    areEqual = false;
  }
  return areEqual;
}


