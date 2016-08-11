/*! \file sqlhardworker_tester.h
 *  \brief тестирование sql-модуля
 *  \author sugarbaron ([bravery_and_stupidity] e-mail: sugarbaron1@mail.ru)
 *  \date 21.06.2016
 */
#ifndef SQLHARDWORKER_TESTER_H
#define SQLHARDWORKER_TESTER_H

#include "general.h"
#include "sqlhardworker.h"
#include "sqldatabasecreator.h"
#include "task.h"
#include "worker.h"
#include "report.h"
#include "order.h"
#include "project.h"

#if TURN_ON_UNIT_TESTS
class SqlHardWorker_Tester
{
  //секция_данных____________________________________________________________
  ///////////////////////////////////////////////////////////////////////////
  private: static SqlHardWorker* _sqlHardWorker;
  private: static SqlDatabaseCreator* _sqlDatabaseCreator;
  private: static Worker  _father;
  private: static Worker  _sysadmin;
  private: static Worker  _designer;
  private: static Worker  _programmer;
  private: static Worker  _nigga;
  private: static Order   _orderEnlargePenis;
  private: static Order   _orderEnlargeBoobs;
  private: static Order   _orderDrawAss;
  private: static Project _project1000000Dollars;
  private: static Project _projectWorldDomination;
  private: static Project _projectStarOfDeath;
  private: static Task    _requestedByFatherSimpleTask;
  private: static Task    _requestedByFatherCompleteTask;
  private: static Task    _requestedByFatherForProgrammerTask;
  private: static Task    _requestedByProgrammerTask;
  private: static Report  _simpleReport;
  private: static Report  _programmersReport;
  private: static Report  _niggasReport;
  
  //секция_методов___________________________________________________________
  ///////////////////////////////////////////////////////////////////////////
  public: static void run();

  private: static void initSqlHardWorker_Tester();
  
  private: static void fillDatabaseWithTestWorkers();
  private: static void initFather();
  private: static void initSysadmin();
  private: static void initDesigner();
  private: static void initProgrammer();
  private: static void initNigga();
  
  private: static void test_getWorkerByLogin();
  private: static void setIdsForAllWorkers();
  private: static void setIdForWorker(Worker& worker);
  private: static void setSlavesForMasters();
  private: static void test_getWorkerById();
  private: static void test_getWorkers();
  private: static void fillListWithAllWorkers(QList<Worker>& list);
  private: static QList<quint32> getIdsForWorkers(QList<Worker>& workers);
  private: static void test_getListOfResponsibleForWorker();
  
  private: static void fillDatabaseWithTestOrders();
  private: static void initOrderEnlargePenis();
  private: static void initOrderEnlargeBoobs();
  private: static void initOrderDrawAss();
  
  private: static void test_getAllOrders();
  private: static void fillListWithAllOrders(QList<Order>& list);
  private: static void setIdsForAllOrders();
  private: static void setIdForOrder(Order& order);
  private: static void test_getOrders();
  private: static void test_getOrder();
  
  private: static void fillDatabaseWithTestProjects();
  private: static void initProject1000000Dollars();
  private: static void initProjectWorldDomination();
  private: static void initProjectStarOfDeath();
  
  private: static void test_getAllProjects();
  private: static void fillListWithAllProjects(QList<Project>& list);
  private: static void setIdsForAllProjects();
  private: static void setIdForProject(Project& project);
  private: static void test_getProjects();
  private: static void test_getProject();
  
  private: static void fillDatabaseWithTestTasks();
  private: static void initRequestedByFatherSimpleTask();
  private: static void initRequestedByFatherCompleteTask();
  private: static void initRequestedByFatherForProgrammerTask();
  private: static void initRequestedByProgrammerTask();
  
  private: static void test_getTasksRequestedBy();
  private: static void setIdsForAllTasks();
  private: static void setIdForTask(Task& task);
  private: static Task getTaskByAuthorIdAndDescription(const quint32 authorId, const QString& description);
  private: static void test_getAllWorkerTasks();
  private: static void test_getTasksForResponsible();
  private: static void test_getCompleteTasksForWorker();
  private: static void test_getTask();
  
  private: static void fillDatabaseWithTestReports();
  private: static void initSimpleReport();
  private: static void initProgrammersReport();
  private: static void initNiggasReport();
  private: static void setReportsIdsForTasks();
  private: static void setIdsForAllReports();
  
  private: static void test_getReports();
  
  private: static void reinitRequestedByFatherSimpleTask();
  
  private: static void test_updateTask();
  
  private: static void reinitSimpleReport();
  
  private: static void test_updateReport();
  
  private: template <typename T> class ListsComparator
  {
    public:  static bool isSameContent(QList<T> list1, QList<T> list2);
    private: static quint32 detectElementIndexInList(const T& required, const QList<T>& list);
  };
  
  /* как тестировать????????????? *
   * void synchronizeDateTime();? *
   * void disconnect();?????????? */
};
#endif//TURN_ON_UNIT_TESTS
#endif // SQLHARDWORKER_TESTER_H
