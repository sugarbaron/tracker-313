/*! \file sqlhardworker_testersqlhardworker_tester.h
 *  \brief реализация методов тестирующих модуль SqlHardWorker
 *  \author sugarbaron ([sugarbaron_bicycles] e-mail: sugarbaron1@mail.ru)
 *  \date 23.06.2016
 */

#include "sqlhardworker_tester.h"
#include "exception.h"

#if TURN_ON_UNIT_TESTS

//секция_реализации_методов_класса_____________________________________________
///////////////////////////////////////////////////////////////////////////////
void SqlHardWorker_Tester::run()
{
  initSqlHardWorker_Tester();
  
  _sqlDatabaseCreator->dropDatabase();
  _sqlDatabaseCreator->createDatabase();
  _sqlDatabaseCreator->createTables();
  
  fillDatabaseWithTestWorkers();
  setSlavesForMasters();
  
  qDebug()<<" ";
  test_getWorkerByLogin();
  test_getWorkerById();
  test_getWorkers();
  test_getListOfResponsibleForWorker();
  
  fillDatabaseWithTestOrders();
  
  qDebug()<<" ";
  test_getAllOrders();
  setIdsForAllOrders();
  test_getOrders();
  test_getOrder();
  
  fillDatabaseWithTestProjects();
  
  qDebug()<<" ";
  test_getAllProjects();
  setIdsForAllProjects();
  test_getProjects();
  test_getProject();
  
  fillDatabaseWithTestTasks();
  
  qDebug()<<" ";
  test_getTasksRequestedBy();
  test_getTasksForResponsible();
  test_getCompleteTasksForWorker();
  test_getAllWorkerTasks();
  test_getTask();
  test_sqlRollbackForAddTask();
  test_ftpRollbackForAddTask();
  
  fillDatabaseWithTestReports();
  setReportsIdsForTasks();
  
  qDebug()<<" ";
  test_getReports();
  test_sqlRollbackForAddReport();
  test_ftpRollbackForAddReport();
  
  reinitRequestedByFatherSimpleTask();
  
  qDebug()<<" ";
  test_updateTask();
  test_updateSqlTaskData();
  test_sqlRollbackForUpdateTask();
  test_ftpRollbackForUpdateTask();
  test_rollbackForUpdateSqlTaskData();
  
  reinitSimpleReport();
  
  qDebug()<<" ";
  test_updateReport();
  test_ftpRollbackForUpdateReport();
  
  qDebug()<<"very good: all tests passed";
  return;
}

void SqlHardWorker_Tester::initSqlHardWorker_Tester()
{
  _sqlHardWorker = General::getSqlHardWorker();
  _sqlDatabaseCreator = General::getSqlDatabaseCreator();
  qDebug()<<"initialization of #SqlHardWorker_Tester is commplete";
  return;
}



void SqlHardWorker_Tester::fillDatabaseWithTestWorkers()
{
  initFather();
  initSysadmin();
  initDesigner();
  initProgrammer();
  initNigga();
  //[важно: addWorker() инициализирует айди воркера, полученного в аргументе]
  _sqlDatabaseCreator->addWorker(_father);
  _sqlDatabaseCreator->addWorker(_sysadmin);
  _sqlDatabaseCreator->addWorker(_designer);
  _sqlDatabaseCreator->addWorker(_programmer);
  _sqlDatabaseCreator->addWorker(_nigga);
  qDebug()<<" ";
  qDebug()<<"#database is filled with test workers";
  return;
}

void SqlHardWorker_Tester::initFather()
{
  _father.setName("big");
  _father.setSurname("mr.");
  _father.setPatronymic("ass");
  _father.setPosition("father");
  _father.setEmail("mr_big_ass@gmail.com");
  _father.setLogin("mr.bigass");
  _father.setPass("bigass");
  _father.setIsGod(false);
  _father.setVice(0);
  return;
}

void SqlHardWorker_Tester::initSysadmin()
{
  _sysadmin.setName("brown");
  _sysadmin.setSurname("mr.");
  _sysadmin.setPatronymic("beard");
  _sysadmin.setPosition("sysadmin");
  _sysadmin.setEmail("mr_brown_beard@gmail.com");
  _sysadmin.setLogin("mr.brownbeard");
  _sysadmin.setPass("brownbeard");
  _sysadmin.setIsGod(false);
  _sysadmin.setVice(0);
  return;
}

void SqlHardWorker_Tester::initDesigner()
{
  _designer.setName("red");
  _designer.setSurname("mr.");
  _designer.setPatronymic("pencil");
  _designer.setPosition("designer");
  _designer.setEmail("mr_red_pencil@gmail.com");
  _designer.setLogin("mr.redpencil");
  _designer.setPass("redpencil");
  _designer.setIsGod(false);
  _designer.setVice(0);
  return;
}

void SqlHardWorker_Tester::initProgrammer()
{
  _programmer.setName("curve");
  _programmer.setSurname("mr.");
  _programmer.setPatronymic("crutch");
  _programmer.setPosition("programmer");
  _programmer.setEmail("mr_curve_crutch@gmail.com");
  _programmer.setLogin("mr.curvecrutch");
  _programmer.setPass("curvecrutch");
  _programmer.setIsGod(false);
  _programmer.setVice(0);
  return;
}

void SqlHardWorker_Tester::initNigga()
{
  _nigga.setName("goldhands");
  _nigga.setSurname("mr.");
  _nigga.setPatronymic("fromass");
  _nigga.setPosition("nigga");
  _nigga.setEmail("mr_goldhands_fromass@gmail.com");
  _nigga.setLogin("mr.goldhandsfromass");
  _nigga.setPass("goldhandsfromass");
  _nigga.setIsGod(false);
  _nigga.setVice(0);
  return;
}

void SqlHardWorker_Tester::setSlavesForMasters()
{
  quint32 fatherId = _father.getId();
  quint32 sysadminId = _sysadmin.getId();
  quint32 designerId = _designer.getId();
  quint32 programmerId = _programmer.getId();
  quint32 niggaId = _nigga.getId();
  _sqlDatabaseCreator->addSlave(fatherId, sysadminId);
  _sqlDatabaseCreator->addSlave(fatherId, designerId);
  _sqlDatabaseCreator->addSlave(fatherId, programmerId);
  _sqlDatabaseCreator->addSlave(programmerId, niggaId);
  qDebug()<<"masters and slaves are setted";
  return;
}

void SqlHardWorker_Tester::test_getWorkerByLogin()
{
  QString fatherLogin = _father.getLogin();
  
  Worker received = _sqlHardWorker->getWorker(fatherLogin);
  
  bool areEqual = received.isEqual(_father);
  if( !areEqual )
  {
    throw TestFailed("test failed: #getWorker(login)");
  }
  qDebug()<<"test passed: #getWorker(login)";
  return;
}

void SqlHardWorker_Tester::test_getWorkerById()
{
  quint32 fatherId = _father.getId();
  
  Worker received = _sqlHardWorker->getWorker(fatherId);
  
  bool areEqual = received.isEqual(_father);
  if( !areEqual )
  {
    throw TestFailed("test failed: #getWorker(id)");
  }
  qDebug()<<"test passed: #getWorker(id)";
  return;
}

void SqlHardWorker_Tester::test_getWorkers()
{
  QList<Worker>  expectedWorkers;
  QList<quint32> workersIds;
  fillListWithAllWorkers(expectedWorkers);
  workersIds = getIdsForWorkers(expectedWorkers);
  
  QList<Worker>  receivedWorkers;
  receivedWorkers = _sqlHardWorker->getWorkers(workersIds);
  
  bool areEqual = ListsComparator<Worker>::isSameContent(expectedWorkers, receivedWorkers);
  if( !areEqual )
  {
    throw TestFailed("test failed: #getWorkers(idList)");
  }
  qDebug()<<"test passed: #getWorkers(idList)";
  return;
}

void SqlHardWorker_Tester::fillListWithAllWorkers(QList<Worker>& list)
{
  list.push_back(_father);
  list.push_back(_sysadmin);
  list.push_back(_designer);
  list.push_back(_programmer);
  list.push_back(_nigga);
  return;
}

QList<quint32> SqlHardWorker_Tester::getIdsForWorkers(QList<Worker>& workers)
{
  QList<quint32> ids;
  quint32 currentId = 0;
  foreach(Worker nigga, workers)
  {
    currentId = nigga.getId();
    ids.push_back(currentId);
  }
  return ids;
}

void SqlHardWorker_Tester::test_getListOfResponsibleForWorker()
{
  quint32 fatherId = _father.getId();
  QList<Worker> expectedSlaves;
  expectedSlaves.push_back(_sysadmin);
  expectedSlaves.push_back(_designer);
  expectedSlaves.push_back(_programmer);
  
  QList<Worker> receivedSlaves;
  receivedSlaves = _sqlHardWorker->getListOfResponsibleForWorker(fatherId);
  
  bool areEqual = false;
  areEqual = ListsComparator<Worker>::isSameContent(expectedSlaves, receivedSlaves);
  if( !areEqual )
  {
    throw TestFailed("test failed: #getListOfResponsibleForWorker()");
  }
  qDebug()<<"test passed: #getListOfResponsibleForWorker()";
  return;
}

void SqlHardWorker_Tester::fillDatabaseWithTestOrders()
{
  initOrderEnlargePenis();
  initOrderEnlargeBoobs();
  initOrderDrawAss();
  _sqlDatabaseCreator->addOrder(_orderEnlargePenis);
  _sqlDatabaseCreator->addOrder(_orderEnlargeBoobs);
  _sqlDatabaseCreator->addOrder(_orderDrawAss);
  qDebug()<<" ";
  qDebug()<<"database is filled with test orders";
  return;
}

void SqlHardWorker_Tester::initOrderEnlargePenis()
{
  _orderEnlargePenis.setName("enlarge_penis");
  return;
}

void SqlHardWorker_Tester::initOrderEnlargeBoobs()
{
  _orderEnlargeBoobs.setName("enlarge_boobs");
  return;
}

void SqlHardWorker_Tester::initOrderDrawAss()
{
  _orderDrawAss.setName("draw_ass");
  return;
}

void SqlHardWorker_Tester::test_getAllOrders()
{
  QList<Order> expectedOrders;
  fillListWithAllOrders(expectedOrders);
  
  QList<Order> receivedOrders = _sqlHardWorker->getAllOrders();
  
  bool areEqual = ListsComparator<Order>::isSameContent(expectedOrders, receivedOrders);
  if( !areEqual )
  {
    throw TestFailed("test failed: #getAllOrders()");
  }
  qDebug()<<"test passed: #getAllOrders()";
  return;
}

void SqlHardWorker_Tester::fillListWithAllOrders(QList<Order>& list)
{
  list.push_back(_orderEnlargePenis);
  list.push_back(_orderEnlargeBoobs);
  list.push_back(_orderDrawAss);
  return;
}

void SqlHardWorker_Tester::setIdsForAllOrders()
{
  setIdForOrder(_orderEnlargePenis);
  setIdForOrder(_orderEnlargeBoobs);
  setIdForOrder(_orderDrawAss);
  qDebug()<<"ids are setted for all orders";
  return;
}

void SqlHardWorker_Tester::setIdForOrder(Order& order)
{
  QList<Order> allOrders;
  QString currentOrderName  = "";
  QString requiredOrderName = order.getName();
  allOrders = _sqlHardWorker->getAllOrders();
  foreach(Order currentOrder, allOrders)
  {
    currentOrderName = currentOrder.getName();
    if(currentOrderName == requiredOrderName)
    {
      order = currentOrder;
      return;
    }
  }
  throw NeedFixCode("order #" + requiredOrderName + " is not found in database");
}

void SqlHardWorker_Tester::test_getOrders()
{
  QList<Order> expectedOrders;
  expectedOrders.push_back(_orderEnlargePenis);
  expectedOrders.push_back(_orderDrawAss);
  quint32 orderEnlargePenisId = _orderEnlargePenis.getId();
  quint32 orderDrawAssId = _orderDrawAss.getId();
  QList<quint32> expectedOrdersIds;
  expectedOrdersIds.push_back(orderEnlargePenisId);
  expectedOrdersIds.push_back(orderDrawAssId);
  
  QList<Order> receivedOrders = _sqlHardWorker->getOrders(expectedOrdersIds);
  
  bool areEqual = ListsComparator<Order>::isSameContent(expectedOrders, receivedOrders);
  if( !areEqual )
  {
    throw TestFailed("test failed: #getOrders(idList)");
  }
  qDebug()<<"test passed: #getOrders(idList)";
  return;
}

void SqlHardWorker_Tester::test_getOrder()
{
  quint32 orderEnlargePenisId = _orderEnlargePenis.getId();
  
  Order received = _sqlHardWorker->getOrder(orderEnlargePenisId);
  
  bool areEqual = received.isEqual(_orderEnlargePenis);
  if( !areEqual )
  {
    throw TestFailed("test failed: #getOrder(id)");
  }
  qDebug()<<"test passed: #getOrder(id)";
  return;
}

void SqlHardWorker_Tester::fillDatabaseWithTestProjects()
{
  initProject1000000Dollars();
  initProjectWorldDomination();
  initProjectStarOfDeath();
  _sqlDatabaseCreator->addProject(_project1000000Dollars);
  _sqlDatabaseCreator->addProject(_projectWorldDomination);
  _sqlDatabaseCreator->addProject(_projectStarOfDeath);
  qDebug()<<" ";
  qDebug()<<"database is filled with test projects";
  return;
}

void SqlHardWorker_Tester::initProject1000000Dollars()
{
  _project1000000Dollars.setName("how_to_get_1000000_dollars");
  return;
}

void SqlHardWorker_Tester::initProjectWorldDomination()
{
  _projectWorldDomination.setName("world_domination");
  return;
}

void SqlHardWorker_Tester::initProjectStarOfDeath()
{
  _projectStarOfDeath.setName("star_of_death");
  return;
}

void SqlHardWorker_Tester::test_getAllProjects()
{
  QList<Project> expectedProjects;
  fillListWithAllProjects(expectedProjects);
  
  QList<Project> receivedProjects = _sqlHardWorker->getAllProjects();
  
  bool areEqual = ListsComparator<Project>::isSameContent(expectedProjects, receivedProjects);
  if( !areEqual )
  {
    throw TestFailed("test failed: #getAllProjects()");
  }
  qDebug()<<"test passed: #getAllProjects()";
  return;
}

void SqlHardWorker_Tester::fillListWithAllProjects(QList<Project>& list)
{
  list.push_back(_project1000000Dollars);
  list.push_back(_projectWorldDomination);
  list.push_back(_projectStarOfDeath);
  return;
}

void SqlHardWorker_Tester::setIdsForAllProjects()
{
  setIdForProject(_project1000000Dollars);
  setIdForProject(_projectWorldDomination);
  setIdForProject(_projectStarOfDeath);
  qDebug()<<"ids are setted for all projects";
  return;
}

void SqlHardWorker_Tester::setIdForProject(Project& project)
{
  QList<Project> allProjects;
  QString currentProjectName = "";
  QString requiredProjectName = project.getName();
  allProjects = _sqlHardWorker->getAllProjects();
  foreach(Project currentProject, allProjects)
  {
    currentProjectName = currentProject.getName();
    if(currentProjectName == requiredProjectName)
    {
      project = currentProject;
      return;
    }
  }
  throw NeedFixCode("project #" + requiredProjectName + " is not found in database");
}

void SqlHardWorker_Tester::test_getProjects()
{
  QList<Project> expected;
  expected.push_back(_project1000000Dollars);
  expected.push_back(_projectStarOfDeath);
  quint32 project1000000DollarsId = _project1000000Dollars.getId();
  quint32 projectStarOfDeathId = _projectStarOfDeath.getId();
  QList<quint32> expectedProjectsIds;
  expectedProjectsIds.push_back(project1000000DollarsId);
  expectedProjectsIds.push_back(projectStarOfDeathId);
  
  QList<Project> received = _sqlHardWorker->getProjects(expectedProjectsIds);
  
  bool areEqual = ListsComparator<Project>::isSameContent(expected, received);
  if( !areEqual )
  {
    throw TestFailed("test failed: #getProjects(idList)");
  }
  qDebug()<<"test passed: #getProjects(idList)";
  return;
}

void SqlHardWorker_Tester::test_getProject()
{
  quint32 project1000000DollarsId = _project1000000Dollars.getId();
  
  Project received = _sqlHardWorker->getProject(project1000000DollarsId);
  
  bool areEqual = received.isEqual(_project1000000Dollars);
  if( !areEqual )
  {
    throw TestFailed("test failed: #getProject(id)");
  }
  qDebug()<<"test passed: #getProject(id)";
  return;
}

void SqlHardWorker_Tester::fillDatabaseWithTestTasks()
{
  initRequestedByFatherSimpleTask();
  initRequestedByFatherCompleteTask();
  initRequestedByFatherForProgrammerTask();
  initRequestedByProgrammerTask();
  try
  {
    //[важно: addTask() инициализирует айди поручения, полученного в аргументе]
    _sqlHardWorker->addTask(_requestedByFatherSimpleTask);
    _sqlHardWorker->addTask(_requestedByFatherCompleteTask);
    _sqlHardWorker->addTask(_requestedByFatherForProgrammerTask);
    _sqlHardWorker->addTask(_requestedByProgrammerTask);
  }
  catch(Exception& exception)
  {
    QString errorMessage = "test failed: #addTask(). diag #" + exception.getInfo();
    throw TestFailed(errorMessage);
  }
  qDebug()<<" ";
  qDebug()<<"database is filled with test tasks";
  return;
}

void SqlHardWorker_Tester::initRequestedByFatherSimpleTask()
{
  QString description  = "description: simple task. ' ; DROP DATABASE TrackerDB;";
  QString comment = "comment: simple task. ' ; DROP DATABASE TrackerDB;";
  quint32 parentTaskId = 0;
  quint32 status = Task::newTask;
  quint32 authorId = _father.getId();
  quint32 orderId  = _orderEnlargePenis.getId();
  quint32 projectId  = _project1000000Dollars.getId();
  quint32 importance = 10;
  quint32 designerId = _designer.getId();
  quint32 programmerId  = _programmer.getId();
  quint32 responsibleId = _sysadmin.getId();
  QDate creationDate = QDate(2013, 5, 17);
  QDate deadlineDate = QDate(2017, 6, 7);
  QDate warningDate  = QDate(2017, 5, 27);
  bool isOverdue = false;
  QList<quint32> accomplices;
  accomplices.push_back(programmerId);
  accomplices.push_back(designerId);
  QString file1;
  QString file2;
  QString file3;
  file1 = "C:/0_sugarbaron/0_programmism/qt_projects/Tracker313/ftp_shared_folder/test_task_file_1.txt";
  file2 = "C:/0_sugarbaron/0_programmism/qt_projects/Tracker313/ftp_shared_folder/evil_file'.txt";
  file3 = "C:/0_sugarbaron/0_programmism/qt_projects/Tracker313/ftp_shared_folder/evil_file;DROP DATABASE TrackerDB;.txt";
  QStringList files;
  files.push_back(file1);
  files.push_back(file2);
  files.push_back(file3);
  
  _requestedByFatherSimpleTask.setDescription(description);
  _requestedByFatherSimpleTask.setCreationDate(creationDate);
  _requestedByFatherSimpleTask.setComment(comment);
  _requestedByFatherSimpleTask.setDeadlineDate(deadlineDate);
  _requestedByFatherSimpleTask.setStatus(status);
  _requestedByFatherSimpleTask.setImportance(importance);
  _requestedByFatherSimpleTask.setParent(parentTaskId);
  _requestedByFatherSimpleTask.setIsOverdue(isOverdue);
  _requestedByFatherSimpleTask.setAuthor(authorId);
  _requestedByFatherSimpleTask.setResponsible(responsibleId);
  _requestedByFatherSimpleTask.setReportId(0);
  _requestedByFatherSimpleTask.setOrder(orderId);
  _requestedByFatherSimpleTask.setProject(projectId);
  _requestedByFatherSimpleTask.setWarningDate(warningDate);
  _requestedByFatherSimpleTask.setAccomplices(accomplices);
  _requestedByFatherSimpleTask.setFilesTask(files);
  return;
}

void SqlHardWorker_Tester::initRequestedByFatherCompleteTask()
{
  QString description  = "description: complete task";
  QString comment = "comment: complete task";
  quint32 status = Task::complete;
  quint32 authorId = _father.getId();
  quint32 importance = 1;
  quint32 parentTaskId  = 0;
  quint32 responsibleId = _designer.getId();
  QDate creationDate = QDate(2015, 5, 5);
  QDate deadlineDate = QDate(2015, 7, 7);
  QDate warningDate = QDate(2015, 7, 1);
  bool isOverdue = false; 
  
  _requestedByFatherCompleteTask.setDescription(description);
  _requestedByFatherCompleteTask.setCreationDate(creationDate);
  _requestedByFatherCompleteTask.setComment(comment);
  _requestedByFatherCompleteTask.setDeadlineDate(deadlineDate);
  _requestedByFatherCompleteTask.setStatus(status);
  _requestedByFatherCompleteTask.setImportance(importance);
  _requestedByFatherCompleteTask.setParent(parentTaskId);
  _requestedByFatherCompleteTask.setIsOverdue(isOverdue);
  _requestedByFatherCompleteTask.setAuthor(authorId);
  _requestedByFatherCompleteTask.setResponsible(responsibleId);
  _requestedByFatherCompleteTask.setReportId(0);
  _requestedByFatherCompleteTask.setOrder(0);
  _requestedByFatherCompleteTask.setProject(0);
  _requestedByFatherCompleteTask.setWarningDate(warningDate);
  return;
}

void SqlHardWorker_Tester::initRequestedByFatherForProgrammerTask()
{
  QString description  = "description: requested by father for programmer task";
  QString comment = "comment: requested by father for programmer task";
  quint32 status = Task::inProgress;
  quint32 authorId = _father.getId();
  quint32 importance = 5; 
  quint32 parentTaskId  = 0;
  quint32 responsibleId = _programmer.getId();
  QDate creationDate = QDate(2015, 7, 7);
  QDate deadlineDate = QDate(2055, 8, 8);
  bool isOverdue = false;
  
  _requestedByFatherForProgrammerTask.setDescription(description);
  _requestedByFatherForProgrammerTask.setCreationDate(creationDate);
  _requestedByFatherForProgrammerTask.setComment(comment);
  _requestedByFatherForProgrammerTask.setDeadlineDate(deadlineDate);
  _requestedByFatherForProgrammerTask.setStatus(status);
  _requestedByFatherForProgrammerTask.setImportance(importance);
  _requestedByFatherForProgrammerTask.setParent(parentTaskId);
  _requestedByFatherForProgrammerTask.setIsOverdue(isOverdue);
  _requestedByFatherForProgrammerTask.setAuthor(authorId);
  _requestedByFatherForProgrammerTask.setResponsible(responsibleId);
  _requestedByFatherForProgrammerTask.setReportId(0);
  _requestedByFatherForProgrammerTask.setOrder(0);
  _requestedByFatherForProgrammerTask.setProject(0);
  return;
}

void SqlHardWorker_Tester::initRequestedByProgrammerTask()
{
  QString description  = "description: requested by programmer for nigga task";
  QString comment = "comment: requested by programmer for nigga task";
  quint32 status = Task::inProgress;
  quint32 authorId = _programmer.getId();
  quint32 importance = 5;
  quint32 parentTaskId  = 0;
  quint32 responsibleId = _nigga.getId();
  QDate creationDate = QDate(2015, 8, 8);
  QDate deadlineDate = QDate(2055, 9, 9);
  QDate warningDate  = QDate(2055, 9, 1);
  bool isOverdue = false; 
  
  _requestedByProgrammerTask.setDescription(description);
  _requestedByProgrammerTask.setCreationDate(creationDate);
  _requestedByProgrammerTask.setComment(comment);
  _requestedByProgrammerTask.setDeadlineDate(deadlineDate);
  _requestedByProgrammerTask.setStatus(status);
  _requestedByProgrammerTask.setImportance(importance);
  _requestedByProgrammerTask.setParent(parentTaskId);
  _requestedByProgrammerTask.setIsOverdue(isOverdue);
  _requestedByProgrammerTask.setAuthor(authorId);
  _requestedByProgrammerTask.setResponsible(responsibleId);
  _requestedByProgrammerTask.setReportId(0);
  _requestedByProgrammerTask.setOrder(0);
  _requestedByProgrammerTask.setProject(0);
  _requestedByProgrammerTask.setWarningDate(warningDate);
  return;
}

void SqlHardWorker_Tester::test_getTasksRequestedBy()
{
  QList<Task> expectedTasks;
  expectedTasks.push_back(_requestedByFatherSimpleTask);
  expectedTasks.push_back(_requestedByFatherCompleteTask);
  expectedTasks.push_back(_requestedByFatherForProgrammerTask);
  
  quint32 fatherId = _father.getId();
  QList<Task> receivedTasks = _sqlHardWorker->getTasksRequestedBy(fatherId);
  
  bool areEqual = ListsComparator<Task>::isSameContent(expectedTasks, receivedTasks);
  if( !areEqual )
  {
    throw TestFailed("test failed: #getTasksRequestedBy()");
  }
  qDebug()<<"test passed: #getTasksRequestedBy()";
  return;
}

void SqlHardWorker_Tester::test_getAllWorkerTasks()
{
  QList<Task> expected;
  expected.push_back(_requestedByFatherSimpleTask);
  expected.push_back(_requestedByFatherForProgrammerTask);
  expected.push_back(_requestedByProgrammerTask);
  quint32 programmerId = _programmer.getId();
  
  QList<Task> received = _sqlHardWorker->getAllWorkerTasks(programmerId);
  
  bool areEqual = ListsComparator<Task>::isSameContent(expected, received);
  if( !areEqual )
  {
    throw TestFailed("test failed: #getAllWorkerTasks()");
  }
  qDebug()<<"test passed: #getAllWorkerTasks()";
  return;
}

void SqlHardWorker_Tester::test_getTasksForResponsible()
{
  QList<Task> expected;
  expected.push_back(_requestedByFatherSimpleTask);
  expected.push_back(_requestedByFatherForProgrammerTask);
  quint32 programmerId = _programmer.getId();
  
  QList<Task> received = _sqlHardWorker->getTasksForResponsible(programmerId);
  
  bool areEqual = ListsComparator<Task>::isSameContent(expected, received);
  if( !areEqual )
  {
    throw TestFailed("test failed: #getTasksForResponsible()");
  }
  qDebug()<<"test passed: #getTasksForResponsible()";
  return;
}

void SqlHardWorker_Tester::test_getCompleteTasksForWorker()
{
  QList<Task> expected;
  expected.push_back(_requestedByFatherCompleteTask);
  quint32 designerId = _designer.getId();
  
  QList<Task> received = _sqlHardWorker->getCompleteTasksForWorker(designerId);
  
  bool areEqual = ListsComparator<Task>::isSameContent(expected, received);
  if( !areEqual )
  {
    throw TestFailed("test failed: #getCompleteTasksForWorker()");
  }
  qDebug()<<"test passed: #getCompleteTasksForWorker()";
  return;
}

void SqlHardWorker_Tester::test_getTask()
{
  quint32 requestedByFatherSimpleTaskId = _requestedByFatherSimpleTask.getIdTask();
  
  Task received = _sqlHardWorker->getTask(requestedByFatherSimpleTaskId);
  
  bool areEqual = received.isEqual(_requestedByFatherSimpleTask);
  if( !areEqual )
  {
    throw TestFailed("test failed: #addTask() #getTask()");
  }
  qDebug()<<"test passed: #addTask() #getTask()";
  return;
}

void SqlHardWorker_Tester::test_sqlRollbackForAddTask()
{
  Task wrongTask;
  //[важно: provokeSqlRollbackForAddTask() проинициализирует айди у wrongTask]
  provokeSqlRollbackForAddTask(wrongTask);
  checkRollbackForAddTask(wrongTask);
  qDebug()<<"test passed: rollback while adding sql task data works correctly";
  return;
}

void SqlHardWorker_Tester::provokeSqlRollbackForAddTask(Task& wrongTask)
{
  const quint32 WRONG_ORDER_ID = 100;
  wrongTask = _requestedByFatherSimpleTask;
  wrongTask.setOrder(WRONG_ORDER_ID);
  
  try
  {
    //[важно: addTask() проинициализирует айди у wrongTask]
    _sqlHardWorker->addTask(wrongTask);
  }
  catch(SqlQueryException)
  {
    return;
  }
  QString errorText = "test failed: cant provoke rollback";
  qCritical()<<errorText;
  throw TestFailed(errorText);
  return;
}

void SqlHardWorker_Tester::test_ftpRollbackForAddTask()
{
  Task wrongTask;
  //[важно: provokeFtpRollbackForAddTask() проинициализирует айди у wrongTask]
  provokeFtpRollbackForAddTask(wrongTask);
  checkRollbackForAddTask(wrongTask);
  qDebug()<<"test passed: rollback while adding ftp task data works correctly";
  return;
}

void SqlHardWorker_Tester::provokeFtpRollbackForAddTask(Task& wrongTask)
{
  const QString WRONG_FILE = "C:/wrong_path/wrong_file.err";
  QStringList wrongFiles;
  wrongFiles.append(WRONG_FILE);
  wrongTask = _requestedByFatherSimpleTask;
  wrongTask.setFilesTask(wrongFiles);
  
  try
  {
    //[важно: addTask() проинициализирует айди у wrongTask]
    _sqlHardWorker->addTask(wrongTask);
  }
  catch(FtpException)
  {
    return;
  }
  QString errorText = "test failed: cant provoke rollback";
  qCritical()<<errorText;
  throw TestFailed(errorText);
}

void SqlHardWorker_Tester::checkRollbackForAddTask(const Task& wrongTask)
{
  quint32 wrongTaskId = wrongTask.getIdTask();
  try
  {
    _sqlHardWorker->getTask(wrongTaskId);
  }
  catch(ExecutionAborted)
  {
    return;
  }
  QString errorText = "test failed: there was no rollback";
  qCritical()<<errorText;
  throw TestFailed(errorText);
}

void SqlHardWorker_Tester::fillDatabaseWithTestReports()
{
  initSimpleReport();
  initProgrammersReport();
  initNiggasReport();
  quint32 requestedByFatherSimpleTaskId = _requestedByFatherSimpleTask.getIdTask();
  quint32 requestedByFatherForProgrammerTaskId = _requestedByFatherForProgrammerTask.getIdTask();
  quint32 requestedByProgrammerTaskId = _requestedByProgrammerTask.getIdTask();
  try
  {
    //[важно: addReport() инициализирует айди отчёта, полученного в аргументе]
    _sqlHardWorker->addReport(_simpleReport, requestedByFatherSimpleTaskId);
    _sqlHardWorker->addReport(_programmersReport, requestedByFatherForProgrammerTaskId);
    _sqlHardWorker->addReport(_niggasReport, requestedByProgrammerTaskId);
  }
  catch(Exception& exception)
  {
    QString errorMessage = "test failed: #addReport(). diag #" + exception.getInfo();
    throw TestFailed(errorMessage);
  }
  qDebug()<<" ";
  qDebug()<<"database is filled with test reports";
  return;
}

void SqlHardWorker_Tester::initSimpleReport()
{
  QString text = "text for simple report. ' ; DROP DATABASE TrackerDB;";
  QDate   creationDate = QDate(2013, 6, 5);
  QString file1;
  QString file2;
  QString file3;
  file1 = "C:/0_sugarbaron/0_programmism/qt_projects/Tracker313/ftp_shared_folder/test_report_file_1.txt";
  file2 = "C:/0_sugarbaron/0_programmism/qt_projects/Tracker313/ftp_shared_folder/evil_file'.txt";
  file3 = "C:/0_sugarbaron/0_programmism/qt_projects/Tracker313/ftp_shared_folder/evil_file;DROP DATABASE TrackerDB;.txt";
  QStringList files;
  files.push_back(file1);
  files.push_back(file2);
  files.push_back(file3);
  
  _simpleReport.setText(text);
  _simpleReport.setCreationDate(creationDate);
  _simpleReport.setFiles(files);
  return;
}

void SqlHardWorker_Tester::initProgrammersReport()
{
  QDate creationDate(2015, 12, 15);
  _programmersReport.setText("text for programmers report.");
  _programmersReport.setCreationDate(creationDate);
  return;
}

void SqlHardWorker_Tester::initNiggasReport()
{
  QDate creationDate(2015, 11, 20);
  _niggasReport.setText("text for niggas report.");
  _niggasReport.setCreationDate(creationDate);
  return;
}

void SqlHardWorker_Tester::setReportsIdsForTasks()
{
  quint32 requestedByFatherSimpleTaskId = _requestedByFatherSimpleTask.getIdTask();
  quint32 requestedByFatherForProgrammerTaskId = _requestedByFatherForProgrammerTask.getIdTask();
  quint32 requestedByProgrammerTask = _requestedByProgrammerTask.getIdTask();
  _requestedByFatherSimpleTask.synhronizeWithDb(requestedByFatherSimpleTaskId);
  _requestedByFatherForProgrammerTask.synhronizeWithDb(requestedByFatherForProgrammerTaskId);
  _requestedByProgrammerTask.synhronizeWithDb(requestedByProgrammerTask);
  qDebug()<<"reports ids are setted for all tasks";
  return;
}

void SqlHardWorker_Tester::test_getReports()
{
  QList<Report> expectedReports;
  expectedReports.push_back(_simpleReport);
  expectedReports.push_back(_programmersReport);
  expectedReports.push_back(_niggasReport);
  quint32 simpleReportId = _simpleReport.getId();
  quint32 programmersReportId = _programmersReport.getId();
  quint32 niggasReportId = _niggasReport.getId();
  QList<quint32> expectedReportsIds;
  expectedReportsIds.push_back(simpleReportId);
  expectedReportsIds.push_back(programmersReportId);
  expectedReportsIds.push_back(niggasReportId);
  
  QList<Report> receivedReports = _sqlHardWorker->getReports(expectedReportsIds);
  
  bool areEqual = ListsComparator<Report>::isSameContent(expectedReports, receivedReports);
  if( !areEqual )
  {
    throw TestFailed("test failed: #addReport() #getReports()");
  }
  qDebug()<<"test passed: #addReport() #getReports()";
  return;
}

void SqlHardWorker_Tester::test_sqlRollbackForAddReport()
{
  Report wrongReport;
  //[важно: provokeSqlRollbackForAddReport() проинициализирует айди у wrongReport]
  provokeSqlRollbackForAddReport(wrongReport);
  checkRollbackForAddReport(wrongReport);
  qDebug()<<"test passed: rollback while adding sql report data works correctly";
  return;
}

void SqlHardWorker_Tester::provokeSqlRollbackForAddReport(Report& wrongReport)
{
  const quint32 WRONG_TASK_ID = 100;
  wrongReport = _simpleReport;
  
  try
  {
    //[важно: addReport() проинициализирует айди у wrongReport]
    _sqlHardWorker->addReport(wrongReport, WRONG_TASK_ID);
  }
  catch(SqlQueryException)
  {
    return;
  }
  QString errorText = "test failed: cant provoke rollback";
  qCritical()<<errorText;
  throw TestFailed(errorText);
}

void SqlHardWorker_Tester::test_ftpRollbackForAddReport()
{
  Report wrongReport;
  //[важно: provokeFtpRollbackForAddReport() проинициализирует айди у wrongReport]
  provokeFtpRollbackForAddReport(wrongReport);
  checkRollbackForAddReport(wrongReport);
  qDebug()<<"test passed: rollback while adding ftp report data works correctly";
  return;
}

void SqlHardWorker_Tester::provokeFtpRollbackForAddReport(Report& wrongReport)
{
  const QString WRONG_FILE = "C:/wrong_path/wrong_file.err";
  QStringList wrongFiles;
  wrongFiles.append(WRONG_FILE);
  wrongReport = _simpleReport;
  wrongReport.setFiles(wrongFiles);
  quint32 requestebByFatherSimpleTaskId = _requestedByFatherSimpleTask.getIdTask();
  
  try
  {
    //[важно: addReport() проинициализирует айди у wrongReport]
    _sqlHardWorker->addReport(wrongReport, requestebByFatherSimpleTaskId);
  }
  catch(FtpException)
  {
    return;
  }
  QString errorText = "test failed: cant provoke rollback";
  qCritical()<<errorText;
  throw TestFailed(errorText);
}

void SqlHardWorker_Tester::checkRollbackForAddReport(const Report& wrongReport)
{
  quint32 wrongReportId = wrongReport.getId();
  QList<quint32> wrongIds;
  wrongIds.append(wrongReportId);
  
  try
  {
    _sqlHardWorker->getReports(wrongIds);
  }
  catch(ExecutionAborted)
  {
    return;
  }
  QString errorText = "test failed: there was no rollback";
  qCritical()<<errorText;
  throw TestFailed(errorText);
}

void SqlHardWorker_Tester::reinitRequestedByFatherSimpleTask()
{
  QString description  = "description: updated simple task. ' ; DROP DATABASE TrackerDB;";
  QString comment = "comment: updated simple task. ' ; DROP DATABASE TrackerDB;";
  quint32 status = Task::inProgress;
  quint32 orderId  = _orderDrawAss.getId();
  quint32 authorId = _father.getId();
  quint32 reportId = _simpleReport.getId();
  quint32 projectId  = _projectWorldDomination.getId();
  quint32 importance = 1;
  quint32 sysadminId = _sysadmin.getId();
  quint32 designerId = _designer.getId();
  quint32 parentTaskId  = 5;
  quint32 responsibleId = _programmer.getId();
  QDate creationDate = QDate(2015, 11, 27);
  QDate deadlineDate = QDate(2015, 12, 15);
  QDate warningDate  = QDate(2015, 12, 5);
  bool isOverdue = true;
  QList<quint32> accomplices;
  accomplices.push_back(sysadminId);
  accomplices.push_back(designerId);
  QString file1;
  QString file2;
  QString file3;
  file1 = "C:/0_sugarbaron/0_programmism/qt_projects/Tracker313/ftp_shared_folder/test_task_file_3.txt";
  file2 = "C:/0_sugarbaron/0_programmism/qt_projects/Tracker313/ftp_shared_folder/evil_file'.txt";
  file3 = "C:/0_sugarbaron/0_programmism/qt_projects/Tracker313/ftp_shared_folder/evil_file;DROP DATABASE TrackerDB;.txt";
  QStringList files;
  files.push_back(file1);
  files.push_back(file2);
  files.push_back(file3);
  
  _requestedByFatherSimpleTask.setDescription(description);
  _requestedByFatherSimpleTask.setCreationDate(creationDate);
  _requestedByFatherSimpleTask.setComment(comment);
  _requestedByFatherSimpleTask.setDeadlineDate(deadlineDate);
  _requestedByFatherSimpleTask.setStatus(status);
  _requestedByFatherSimpleTask.setImportance(importance);
  _requestedByFatherSimpleTask.setParent(parentTaskId);
  _requestedByFatherSimpleTask.setIsOverdue(isOverdue);
  _requestedByFatherSimpleTask.setAuthor(authorId);
  _requestedByFatherSimpleTask.setResponsible(responsibleId);
  _requestedByFatherSimpleTask.setReportId(reportId);
  _requestedByFatherSimpleTask.setOrder(orderId);
  _requestedByFatherSimpleTask.setProject(projectId);
  _requestedByFatherSimpleTask.setWarningDate(warningDate);
  _requestedByFatherSimpleTask.setAccomplices(accomplices);
  _requestedByFatherSimpleTask.setFilesTask(files);
  qDebug()<<"task is prepared for update";
  return;
}

void SqlHardWorker_Tester::test_updateTask()
{
  quint32 requestedByFatherSimpleTaskId = _requestedByFatherSimpleTask.getIdTask();
  
  _sqlHardWorker->updateTask(_requestedByFatherSimpleTask);
  Task received = _sqlHardWorker->getTask(requestedByFatherSimpleTaskId);
  
  bool areEqual = received.isEqual(_requestedByFatherSimpleTask);
  if( !areEqual )
  {
    throw TestFailed("test failed: #updateTask()");
  }
  qDebug()<<"#test passed: #updateTask()";
  return;
}

void SqlHardWorker_Tester::test_updateSqlTaskData()
{
  _requestedByFatherSimpleTask.setComment("updated comment v2; ' DELETE * FROM TrackerDB.Tasks;");
  _requestedByFatherSimpleTask.setStatus(Task::isBidForReport);
  quint32 requestedByFatherSimpleTaskId = _requestedByFatherSimpleTask.getIdTask();
  
  _sqlHardWorker->updateSqlTaskData(_requestedByFatherSimpleTask);
  Task received = _sqlHardWorker->getTask(requestedByFatherSimpleTaskId);
  bool areEqual = received.isEqual(_requestedByFatherSimpleTask);
  if( !areEqual )
  {
    throw TestFailed("test failed: #test_updateSqlTaskData()");
  }
  qDebug()<<"#test passed: #test_updateSqlTaskData()";
  return;
}

void SqlHardWorker_Tester::test_sqlRollbackForUpdateTask()
{
  Task wrongTask = _requestedByFatherSimpleTask;
  wrongTask.setDescription("wrong task. testing rollback while updating sql task data");
  
  provokeSqlRollbackForUpdateTask(wrongTask);
  checkRollbackForUpdateTask(wrongTask);
  qDebug()<<"test passed: rollback while updating sql task data works correctly";
  return;
}

void SqlHardWorker_Tester::provokeSqlRollbackForUpdateTask(Task& wrongTask)
{
  const quint32 WRONG_ORDER_ID = 100;
  wrongTask.setOrder(WRONG_ORDER_ID);
  
  try
  {
    _sqlHardWorker->updateTask(wrongTask);
  }
  catch(SqlQueryException)
  {
    return;
  }
  QString errorText = "test failed: cant provoke rollback";
  qCritical()<<errorText;
  throw TestFailed(errorText);
}

void SqlHardWorker_Tester::test_ftpRollbackForUpdateTask()
{
  Task wrongTask = _requestedByFatherSimpleTask;
  wrongTask.setDescription("wrong task. testing rollback while updating sql task data");
  
  provokeFtpRollbackForUpdateTask(wrongTask);
  checkRollbackForUpdateTask(wrongTask);
  qDebug()<<"test passed: rollback while updating ftp task data works correctly";
  return;
}

void SqlHardWorker_Tester::provokeFtpRollbackForUpdateTask(Task& wrongTask)
{
  const QString WRONG_FILE = "C:/wrong_path/wrong_file.err";
  QStringList wrongFiles;
  wrongFiles.append(WRONG_FILE);
  wrongTask.setFilesTask(wrongFiles);
  
  try
  {
    _sqlHardWorker->updateTask(wrongTask);
  }
  catch(FtpException)
  {
    return;
  }
  QString errorText = "test failed: cant provoke rollback";
  qCritical()<<errorText;
  throw TestFailed(errorText);
}

void SqlHardWorker_Tester::test_rollbackForUpdateSqlTaskData()
{
  Task wrongTask = _requestedByFatherSimpleTask;
  wrongTask.setDescription("wrong task. testing rollback while updating sql task data");
  
  provokeRollbackForUpdateSqlTaskData(wrongTask);
  checkRollbackForUpdateTask(wrongTask);
  qDebug()<<"test passed: rollback while updating task works correctly";
  return;
}

void SqlHardWorker_Tester::provokeRollbackForUpdateSqlTaskData(Task& wrongTask)
{
  const quint32 WRONG_ORDER_ID = 100;
  wrongTask.setOrder(WRONG_ORDER_ID);
  
  try
  {
    _sqlHardWorker->updateSqlTaskData(wrongTask);
  }
  catch(SqlQueryException)
  {
    return;
  }
  QString errorText = "test failed: cant provoke rollback";
  qCritical()<<errorText;
  throw TestFailed(errorText);
}

void SqlHardWorker_Tester::checkRollbackForUpdateTask(const Task& wrongTask)
{
  QString wrongDescription = wrongTask.getDescription();
  quint32 id = wrongTask.getIdTask();
  
  Task receivedTask = _sqlHardWorker->getTask(id);
  
  QString receivedDescription = receivedTask.getDescription();
  if(receivedDescription == wrongDescription)
  {
    QString errorText = "test failed: there was no rollback";
    qCritical()<<errorText;
    throw TestFailed(errorText);
  }
  return;
}

void SqlHardWorker_Tester::reinitSimpleReport()
{
  QString text = "updated text for simple report. ' ; DROP DATABASE TrackerDB;";
  QDate   creationDate = QDate(2014, 3, 1);
  QString file1;
  QString file2;
  QString file3;
  file1 = "C:/0_sugarbaron/0_programmism/qt_projects/Tracker313/ftp_shared_folder/test_report_file_3.txt";
  file2 = "C:/0_sugarbaron/0_programmism/qt_projects/Tracker313/ftp_shared_folder/evil_file'.txt";
  file3 = "C:/0_sugarbaron/0_programmism/qt_projects/Tracker313/ftp_shared_folder/evil_file;DROP DATABASE TrackerDB;.txt";
  QStringList files;
  files.push_back(file1);
  files.push_back(file2);
  files.push_back(file3);
  
  _simpleReport.setText(text);
  _simpleReport.setCreationDate(creationDate);
  _simpleReport.setFiles(files);
  qDebug()<<"report is prepared for update";
  return;
}

void SqlHardWorker_Tester::test_updateReport()
{
  quint32 simpleReportId = _simpleReport.getId();
  QList<quint32> listForSimpleReportId;
  listForSimpleReportId.push_back(simpleReportId);
  
  _sqlHardWorker->updateReport(_simpleReport);
  QList<Report> receivedList = _sqlHardWorker->getReports(listForSimpleReportId);
  
  quint32 receivedListSize = receivedList.size();
  if(receivedListSize != 1)
  {
    QString errorMessage;
    errorMessage = "something strange: #receivedListSize is not 1)";
    throw NeedFixCode(errorMessage);
  }
  Report received = receivedList.takeLast();
  bool areEqual = received.isEqual(_simpleReport);
  if( !areEqual )
  {
    throw TestFailed("test failed: #updateReport()");
  }
  qDebug()<<"test passed: #updateReport()";
  return;
}

void SqlHardWorker_Tester::test_ftpRollbackForUpdateReport()
{
  Report wrongReport = _simpleReport;
  wrongReport.setText("wrong report. testing rollback for updateReport()");
  
  provokeFtpRollbackForUpdateReport(wrongReport);
  checkRollbackForUpdateReport(wrongReport);
  qDebug()<<"test passed: rollback while updating ftp report data works correctly";
  return;
}

void SqlHardWorker_Tester::provokeFtpRollbackForUpdateReport(Report& wrongReport)
{
  const QString WRONG_FILE = "C:/wrong_path/wrong_file.err";
  QStringList wrongFiles;
  wrongFiles.append(WRONG_FILE);
  wrongReport.setFiles(wrongFiles);
  
  try
  {
    _sqlHardWorker->updateReport(wrongReport);
  }
  catch(FtpException)
  {
    return;
  }
  QString errorText = "test failed: cant provoke rollback";
  qCritical()<<errorText;
  throw TestFailed(errorText);
}

void SqlHardWorker_Tester::checkRollbackForUpdateReport(const Report& wrongReport)
{
  const QString WRONG_REPORT_TEXT = wrongReport.getText();
  quint32 id = wrongReport.getId();
  QList<quint32> ids;
  ids.append(id);
  
  QList<Report> receivedReports = _sqlHardWorker->getReports(ids);
  quint32 receiedReportsQuantity = receivedReports.size();
  if(receiedReportsQuantity != 1)
  {
    QString errorText = "test failed: received reports quantity is not 1";
    qCritical()<<errorText;
    throw TestFailed(errorText);
  }
  
  Report receivedReport = receivedReports.first();
  QString receivedReportText = receivedReport.getText();
  if(receivedReportText == WRONG_REPORT_TEXT)
  {
    QString errorText = "test failed: there was no rollback";
    qCritical()<<errorText;
    throw TestFailed(errorText);
  }
  return;
}

template <typename T>
bool SqlHardWorker_Tester::ListsComparator<T>::isSameContent(QList<T> list1, QList<T> list2)
{
  bool areEqual = true;
  quint32 detectedElementIndex = 0;
  quint32 list1Size = list1.size();
  quint32 list2Size = list2.size();
  T currentList1Element;
  if(list1Size != list2Size)
  {
    return false;
  }
  while( !(list1.isEmpty()) )
  {
    currentList1Element = list1.takeFirst();
    try
    {
      detectedElementIndex = detectElementIndexInList(currentList1Element, list2);
    }
    catch(NotDetected)
    {
      areEqual = false;
      break;
    }
    list2.removeAt(detectedElementIndex);
  }
  return areEqual;
}

template <typename T>
quint32 SqlHardWorker_Tester::ListsComparator<T>::detectElementIndexInList(const T& required, const QList<T>& list)
{
  quint32 detectedIndex = 0;
  quint32 listSize = list.size();
  bool areEqual = false;
  bool isDetected = false;
  T current;
  for(quint32 currentIndex=0; currentIndex<listSize; currentIndex++)
  {
    current  = list.value(currentIndex);
    areEqual = current.isEqual(required);
    if(areEqual)
    {
      detectedIndex = currentIndex;
      isDetected = true;
      break;
    }
  }
  if( !isDetected )
  {
    throw NotDetected("required element is not detected in list");
  }
  return detectedIndex;
}

//секция_определения_статических_членов_класса_________________________________
///////////////////////////////////////////////////////////////////////////////
SqlHardWorker* SqlHardWorker_Tester::_sqlHardWorker;
SqlDatabaseCreator* SqlHardWorker_Tester::_sqlDatabaseCreator;
Worker  SqlHardWorker_Tester::_father;
Worker  SqlHardWorker_Tester::_sysadmin;
Worker  SqlHardWorker_Tester::_designer;
Worker  SqlHardWorker_Tester::_programmer;
Worker  SqlHardWorker_Tester::_nigga;
Order   SqlHardWorker_Tester::_orderEnlargePenis;
Order   SqlHardWorker_Tester::_orderEnlargeBoobs;
Order   SqlHardWorker_Tester::_orderDrawAss;
Project SqlHardWorker_Tester::_project1000000Dollars;
Project SqlHardWorker_Tester::_projectWorldDomination;
Project SqlHardWorker_Tester::_projectStarOfDeath;
Task    SqlHardWorker_Tester::_requestedByFatherSimpleTask;
Task    SqlHardWorker_Tester::_requestedByFatherCompleteTask;
Task    SqlHardWorker_Tester::_requestedByFatherForProgrammerTask;
Task    SqlHardWorker_Tester::_requestedByProgrammerTask;
Report  SqlHardWorker_Tester::_simpleReport;
Report  SqlHardWorker_Tester::_programmersReport;
Report  SqlHardWorker_Tester::_niggasReport;

#endif//TURN_ON_UNIT_TESTS
