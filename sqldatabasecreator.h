/*! \file   sqldatabasecreator.h
 *  \brief  интерфейс модуля, создания тестовой базы данных и инструментов для её заполнениия
 *  \author sugarbaron ([bravery_and_stupidity] e-mail: sugarbaron1@mail.ru)
 *  \date 05.08.2016
 */
#ifndef SQLDATABASECREATOR
#define SQLDATABASECREATOR

#include "worker.h"
#include "project.h"
#include "order.h"

/*! \brief  интерфейс модуля, создания тестовой базы данных и инструментов для её заполнениия */
class SqlDatabaseCreator
{
  public:  virtual void dropDatabase() const = 0;
  
  public:  virtual void createDatabase() const = 0;
  public:  virtual void createTables() const = 0;
  
  public:  virtual void addWorker(Worker& newWorker) const = 0;
  public:  virtual void addSlave(const quint32 master, const quint32 slave) const = 0;
  public:  virtual void addOrder(const Order& order) const = 0;
  public:  virtual void addProject(const Project& project) const = 0;
  public:  virtual void addVice(const quint32 worker, const quint32 vice) const = 0;
};
#endif // SQLDATABASECREATOR

