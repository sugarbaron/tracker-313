#include "sqlhardworkerunit.h"
#include "sqlexchangeunit.h"
#include "exception.h"
#include "settings.h"
#include "clock.h"

void SqlHardWorkerUnit::create()
{
  checkUnitAbsence();
  _thisUnit.reset( new SqlHardWorkerUnit );
  _thisUnit->_exchangeUnit = createExchangeUnit();
  return;
}

void SqlHardWorkerUnit::checkUnitAbsence()
{
  bool isAlreadyCreated = isUnitAlreadyCreated();
  if(isAlreadyCreated)
  {
    QString errorText = "unit is already created. it is not allowed to create one more instances";
    qCritical()<<errorText;
    throw NeedFixCode(errorText);
  }
  return;
}

SqlHardWorkerUnit::SqlHardWorkerUnit()
{}

SqlExchange* SqlHardWorkerUnit::createExchangeUnit()
{
  Settings* settings = General::getSettings();
  QString ip = settings->getSqlIp();
  quint32 port  = settings->getSqlPort();
  QString login = settings->getSqlLogin();
  QString password = settings->getSqlPassword();
  
  SqlExchange* exchangeUnit = SqlExchangeUnit::create(ip, port, login, password);
  return exchangeUnit;
}

SqlHardWorkerUnit::~SqlHardWorkerUnit()
{}

SqlHardWorkerUnit* SqlHardWorkerUnit::getInstance()
{
  checkUnitExistance();
  return _thisUnit.data();
}

void SqlHardWorkerUnit::checkUnitExistance()
{
  bool isNotCreatedYet = isUnitNotCreatedYet();
  if(isNotCreatedYet)
  {
    QString errorText = "unit is not exist. unit was not created";
    qCritical()<<errorText;
    throw NeedFixCode(errorText);
  }
  return;
}

bool SqlHardWorkerUnit::isUnitAlreadyCreated()
{
  bool isAlreadyCreated = !( isUnitNotCreatedYet() );
  return isAlreadyCreated;
}

bool SqlHardWorkerUnit::isUnitNotCreatedYet()
{
  bool isNotCreatedYet = _thisUnit.isNull();
  return isNotCreatedYet;
}

void SqlHardWorkerUnit::addTask(Task& task) const
{
  synchronizeDateTime();
  
  if( !(task.isValid()) )
  {
    QString errorText = "invalid argument: #task";
    qCritical()<<errorText;
    throw NeedFixCode(errorText);
  }
  
  _exchangeUnit->beginTransaction();
  tryToAddTaskDataToSqlServer(task);
  tryToAddTaskDataToFtpServer(task);
  _exchangeUnit->commitTransaction();
  return;
}

void SqlHardWorkerUnit::tryToAddTaskDataToSqlServer(Task& task) const
{
  try
  {
    addTaskData(task);
    quint32 taskId = defineIdOfJustAddedTask();
    task.setIdTask(taskId);
    addRelatedDataForTask(task);
  }
  catch(...)
  {
    _exchangeUnit->rollbackTransaction();
    qCritical()<<"error: adding task to database failed. catched exception";
    throw;
  }
  return;
}

void SqlHardWorkerUnit::addTaskData(const Task& task) const
{
  const QString DESCRIPTION_PLACEHOLDER = ":description_placeholder";
  const QString IS_OVERDUE_PLACEHOLDER  = ":isOverdue_placeholder";
  const QString IMPORTANCE_PLACEHOLDER  = ":importance_placeholder";
  const QString RESPONSIBLE_ID_PLACEHOLDER = ":responsible_placeholder";
  const QString PARENT_TASK_ID_PLACEHOLDER = ":parentTaskId_placeholder";
  const QString CREATION_DATE_PLACEHOLDER  = ":creationDate_placeholder";
  const QString DEADLINE_DATE_PLACEHOLDER  = ":deadlineDate_placeholder";
  const QString AUTHOR_ID_PLACEHOLDER = ":author_placeholder";
  const QString STATUS_PLACEHOLDER = ":status_placeholder";
  QString queryText = "INSERT INTO TrackerDB.Tasks ("
                        "description, "
                        "author, "
                        "responsible, "
                        "creationDate, "
                        "deadlineDate, "
                        "status, "
                        "isOverdue, "
                        "importance, "
                        "parentTaskId)"
                      "VALUES("
                       + DESCRIPTION_PLACEHOLDER 
                 + "," + AUTHOR_ID_PLACEHOLDER
                 + "," + RESPONSIBLE_ID_PLACEHOLDER
                 + "," + CREATION_DATE_PLACEHOLDER
                 + "," + DEADLINE_DATE_PLACEHOLDER
                 + "," + STATUS_PLACEHOLDER
                 + "," + IS_OVERDUE_PLACEHOLDER
                 + "," + IMPORTANCE_PLACEHOLDER
                 + "," + PARENT_TASK_ID_PLACEHOLDER + ");";
  _exchangeUnit->getReadyFor(queryText);
  
  QString description = task.getDescription();
  quint32 responsibleId = task.getResponsibleId();
  quint32 status = task.getStatus();
  quint32 authorId = task.getAuthorId();
  quint32 parentTaskId = task.getParent();
  quint32 importance = task.getImportance();
  QDate creationDate = task.getCreationDate();
  QDate deadlineDate = task.getDeadlineDate();
  bool  isOverdue = task.getIsOverdue();
  
  _exchangeUnit->loadPlaceholder(DESCRIPTION_PLACEHOLDER, description);
  _exchangeUnit->loadPlaceholder(CREATION_DATE_PLACEHOLDER, creationDate);
  _exchangeUnit->loadPlaceholder(DEADLINE_DATE_PLACEHOLDER, deadlineDate);
  _exchangeUnit->loadPlaceholder(PARENT_TASK_ID_PLACEHOLDER, parentTaskId);
  _exchangeUnit->loadPlaceholder(RESPONSIBLE_ID_PLACEHOLDER, responsibleId);
  _exchangeUnit->loadPlaceholder(IMPORTANCE_PLACEHOLDER, importance);
  _exchangeUnit->loadPlaceholder(IS_OVERDUE_PLACEHOLDER, isOverdue);
  _exchangeUnit->loadPlaceholder(AUTHOR_ID_PLACEHOLDER, authorId);
  _exchangeUnit->loadPlaceholder(STATUS_PLACEHOLDER, status);
  
  _exchangeUnit->executeQuery();
  return;
}

quint32 SqlHardWorkerUnit::defineIdOfJustAddedTask() const
{
  QList<SqlRecord> queryResult = executeGetMaxTaskIdQuery();
  quint32 maxTaskId = extractMaxIdFromQueryResult(queryResult);
  return  maxTaskId;
}

QList<SqlRecord> SqlHardWorkerUnit::executeGetMaxTaskIdQuery() const
{
  QString queryText = "SELECT MAX(idTasks) "
                      "FROM TrackerDB.Tasks;";
  _exchangeUnit->getReadyFor(queryText);
  QList<SqlRecord> queryResult = _exchangeUnit->executeQuery();
  return queryResult;
}

quint32 SqlHardWorkerUnit::extractMaxIdFromQueryResult(QList<SqlRecord>& queryResult) const
{
  checkForTheOnlyRecordInQueryResult(queryResult);
  
  SqlRecord record = queryResult.takeFirst();
  quint32 maxId = extractMaxIdFromRecord(record);
  return  maxId;
}

quint32 SqlHardWorkerUnit::extractMaxIdFromRecord(const SqlRecord& record) const
{
  checkForTheOnlyFieldInRecord(record);
  
  bool isOk = false;
  quint32 maxId = record.first().toUInt(&isOk);
  if(isOk == false)
  {
    QString errorText = "(strange shit) error converting from QVariant";
    qCritical()<<errorText;
    throw CriticalOperationException(errorText);
  }
  
  if(maxId == 0)
  {
    QString errorText = "(strange shit) maxId == 0";
    qCritical()<<errorText;
    throw NeedFixCode(errorText);
  }
  return maxId;
}

void SqlHardWorkerUnit::addRelatedDataForTask(const Task& task) const
{
  addAccomplicesIfExist(task);
  addCommentIfExist(task);
  addWarningDateIfExist(task);
  addOrdersIfExist(task);
  addProjectsIfExist(task);
  addFilesIfExist(task);
  addReportIfExist(task);
  return;
}

void SqlHardWorkerUnit::addAccomplicesIfExist(const Task& task) const
{
  quint32 taskId = task.getIdTask();
  QList<quint32> accomplices = task.getAccomplicesId();
  if( !(accomplices.isEmpty()) )
  {
    addAccomplicesForTask(taskId, accomplices);
  }
  return;
}

void SqlHardWorkerUnit::addAccomplicesForTask(const quint32 taskId, const QList<quint32>& accomplices) const
{
  QString queryText = "INSERT INTO TrackerDB.relationshipsTaskAccomplices ("
                        "idTask, "
                        "idWorker)"
                      "VALUES";
  QString taskIdAsText = transformInText(taskId);
  quint32 currentAccompliceId = 0;
  QString accompliceIdAsText;
  quint32 accomplicesQuantity = accomplices.size();
  const quint32 LAST_INDEX = accomplicesQuantity - 1;
  for(quint32 index=0; index<accomplicesQuantity; index++)
  {
    currentAccompliceId = accomplices.at(index);
    accompliceIdAsText  = transformInText(currentAccompliceId);
    queryText += "(" + taskIdAsText + "," + accompliceIdAsText + ")";
    if(index < LAST_INDEX)
    {
      queryText += ",";
    }
  }
  queryText += ";";
  
  _exchangeUnit->getReadyFor(queryText);
  _exchangeUnit->executeQuery();
  return;
}

void SqlHardWorkerUnit::addCommentIfExist(const Task& task) const
{
  quint32 taskId = task.getIdTask();
  QString comment = task.getComment();
  if( !(comment.isEmpty()) )
  {
    addCommentForTask(taskId, comment);
  }
  return;
}

void SqlHardWorkerUnit::addCommentForTask(const quint32 taskId, const QString& comment) const
{
  const QString COMMENT_PLACEHOLDER = ":comment_placeholder";
  const QString TASK_ID_PLACEHOLDER = ":taskId_placeholder";
  
  QString queryText = "INSERT INTO TrackerDB.comments ("
                 "Comment, "
                 "idTasks) "
               "VALUES("
               + COMMENT_PLACEHOLDER + ","
               + TASK_ID_PLACEHOLDER + ");";
  
  _exchangeUnit->getReadyFor(queryText);
  _exchangeUnit->loadPlaceholder(COMMENT_PLACEHOLDER, comment);
  _exchangeUnit->loadPlaceholder(TASK_ID_PLACEHOLDER, taskId);
  _exchangeUnit->executeQuery();
  return;
}

void SqlHardWorkerUnit::addWarningDateIfExist(const Task& task) const
{
  quint32 taskId = task.getIdTask();
  QDate warningDate = task.getWarningDate();
  if( warningDate.isValid() )
  {
    addWarningDateForTask(taskId, warningDate);
  }
  return;
}

void SqlHardWorkerUnit::addWarningDateForTask(const quint32 taskId, const QDate& warningDate) const
{
  const QString WARNING_DATE_PLACEHOLDER = ":warningDsate_placeholder";
  const QString TASK_ID_PLACEHOLDER = ":taskId_placeholder";
  QString queryText = "INSERT INTO TrackerDB.warningDates ("
                        "date, "
                        "idTask) "
                      "VALUES("
                       + WARNING_DATE_PLACEHOLDER 
                 + "," + TASK_ID_PLACEHOLDER + ");";
  
  _exchangeUnit->getReadyFor(queryText);
  _exchangeUnit->loadPlaceholder(WARNING_DATE_PLACEHOLDER, warningDate);
  _exchangeUnit->loadPlaceholder(TASK_ID_PLACEHOLDER, taskId);
  _exchangeUnit->executeQuery();
  return;
}

void SqlHardWorkerUnit::addOrdersIfExist(const Task& task) const
{
  quint32 taskId  = task.getIdTask();
  quint32 orderId = task.getOrderId();
  if(orderId != 0)
  {
    QList<quint32> ordersIds;
    ordersIds.append(orderId);
    addOrdersForTask(taskId, ordersIds);
  }
  return;
}

void SqlHardWorkerUnit::addOrdersForTask(const quint32 taskId, const QList<quint32>& ordersIds) const
{
  QString queryText = "INSERT INTO TrackerDB.relationshipsTaskOrder ("
                        "idTask, "
                        "idOrder) "
                      "VALUES";
  QString taskIdAsText = transformInText(taskId);
  quint32 currentOrderId = 0;
  QString orderIdAsText;
  quint32 ordersQuantity = ordersIds.size();
  const quint32 LAST_INDEX = ordersQuantity - 1;
  for(quint32 index=0; index<ordersQuantity; index++)
  {
    currentOrderId = ordersIds.at(index);
    orderIdAsText  = transformInText(currentOrderId);
    queryText += "(" + taskIdAsText + "," + orderIdAsText + ")";
    if(index < LAST_INDEX)
    {
      queryText += ",";
    }
  }
  queryText += ";";
  
  _exchangeUnit->getReadyFor(queryText);
  _exchangeUnit->executeQuery();
  return;
}

void SqlHardWorkerUnit::addProjectsIfExist(const Task& task) const
{
  quint32 taskId = task.getIdTask();
  quint32 projectId = task.getProjectId();
  if(projectId != 0)
  {
    QList<quint32> projectsIds;
    projectsIds.append(projectId);
    addProjectsForTask(taskId, projectsIds);
  }
  return;
}

void SqlHardWorkerUnit::addProjectsForTask(const quint32 taskId, const QList<quint32>& projectsIds) const
{
  QString queryText = "INSERT INTO TrackerDB.relationshipsTaskProject ("
                        "idTask, "
                        "idProject) "
                      "VALUES";
  
  QString taskIdAsText = transformInText(taskId);
  quint32 currentProjectId = 0;
  QString projectIdAsText;
  quint32 projectsQuantity = projectsIds.size();
  const quint32 LAST_INDEX = projectsQuantity - 1;
  for(quint32 index=0; index<projectsQuantity; index++)
  {
    currentProjectId = projectsIds.at(index);
    projectIdAsText  = transformInText(currentProjectId);
    queryText += "(" + taskIdAsText + "," + projectIdAsText + ")";
    if(index < LAST_INDEX)
    {
      queryText += ",";
    }
  }
  queryText += ";";
  
  _exchangeUnit->getReadyFor(queryText);
  _exchangeUnit->executeQuery();
  return;
}

void SqlHardWorkerUnit::addFilesIfExist(const Task& task) const
{
  quint32 taskId = task.getIdTask();
  QStringList files = task.getFilesTask();
  if( !(files.isEmpty()) )
  {
    addFilesForTask(taskId, files);
  }
  return;
}

void SqlHardWorkerUnit::addFilesForTask(const quint32 taskId, const QStringList& files) const
{
  QString queryText = "INSERT INTO TrackerDB.filesTasks ("
                       "filePath, "
                       "idTask)"
                      "VALUES ";
  QStringList filesPlaceholders;
  QStringList tasksPlaceholders;
  QString currentFilePlaceholder;
  QString currentTaskPlaceholder;
  QString filePlaceholderBody = ":file_placeholder";
  QString taskPlaceholderBody = ":task_placeholder";
  QString placeholderNumber;
  quint32 filesQuantity = files.size();
  const quint32 LAST_INDEX = filesQuantity - 1;
  for(quint32 index=0; index<filesQuantity; index++)
  {
    placeholderNumber = transformInText(index);
    currentFilePlaceholder = filePlaceholderBody + placeholderNumber;
    currentTaskPlaceholder = taskPlaceholderBody + placeholderNumber;
    filesPlaceholders.append(currentFilePlaceholder);
    tasksPlaceholders.append(currentTaskPlaceholder);
    queryText += "(" + currentFilePlaceholder + "," + currentTaskPlaceholder + ")";
    if(index < LAST_INDEX)
    {
      queryText += ",";
    }
  }
  queryText += ";";
  
  _exchangeUnit->getReadyFor(queryText);
  
  QString currentFile;
  for(quint32 index=0; index<filesQuantity; index++)
  {
    currentFile = files.at(index);
    currentFilePlaceholder = filesPlaceholders.at(index);
    currentTaskPlaceholder = tasksPlaceholders.at(index);
    _exchangeUnit->loadPlaceholder(currentFilePlaceholder, currentFile);
    _exchangeUnit->loadPlaceholder(currentTaskPlaceholder, taskId);
  }
  
  _exchangeUnit->executeQuery();
  return;
}

void SqlHardWorkerUnit::addReportIfExist(const Task& task) const
{
  quint32 taskId = task.getIdTask();
  quint32 reportId = task.getReportId();
  if(reportId != 0)
  {
    addReportForTask(taskId, reportId);
  }
  return;
}

void SqlHardWorkerUnit::addReportForTask(const quint32 taskId, const quint32 reportId) const
{
  const QString TASK_ID_PLACEHOLDER = ":taskId_placeholder";
  const QString REPORT_ID_PLACEHOLDER = ":reportId_placeholder";
  QString queryText = "INSERT INTO TrackerDB.relationshipsTasksReports ("
                        "idTask, "
                        "idReport) "
                      "VALUES ("
                       + TASK_ID_PLACEHOLDER   
                 + "," + REPORT_ID_PLACEHOLDER + ");";
  _exchangeUnit->getReadyFor(queryText);
  _exchangeUnit->loadPlaceholder(TASK_ID_PLACEHOLDER, taskId);
  _exchangeUnit->loadPlaceholder(REPORT_ID_PLACEHOLDER, reportId);
  _exchangeUnit->executeQuery();
  return;
}

void SqlHardWorkerUnit::tryToAddTaskDataToFtpServer(const Task& task) const
{
  try
  {
    addTaskFilesToFtpServer(task);
  }
  catch(...)
  {
    _exchangeUnit->rollbackTransaction();
    removeFtpFilesAfterAddingTask(task);
    qCritical()<<"catched exception while loading task files to ftp-server";
    throw;
  }
  return;
}

void SqlHardWorkerUnit::addTaskFilesToFtpServer(const Task& task) const
{
  FtpHardWorker* ftpHardWorker = General::getFtpHardWorker();
  
  quint32 taskId = task.getIdTask();
  quint32 parentTaskId = task.getParent();
  QStringList files = task.getFilesTask();
  
  QString taskIdAsText = transformInText(taskId);
  QString parentTaskIdAsText = transformInText(parentTaskId);
  QString thisTaskPathInServer = "/Tasks/id_" + taskIdAsText + "/";
  QString parentTaskPathInServer = "/Tasks/id_" + parentTaskIdAsText + "/";
  if( (parentTaskId > 0) && (!files.isEmpty()) )
  {
    //[если перепорученная задача]
    ftpHardWorker->copy(parentTaskPathInServer, thisTaskPathInServer);
  }
  else
  {
    //[если задача не перепорученная, просто загружаем файлы]
    uploadFilesToFtpServer(files, thisTaskPathInServer);
  }
  return;
}

void SqlHardWorkerUnit::removeFtpFilesAfterAddingTask(const Task& task) const
{
  quint32 taskId = task.getIdTask();
  QString taskIdAsText = transformInText(taskId);
  QString pathInServer = "/Tasks/id_" + taskIdAsText + "/";
  
  removeFolderFromFtpServer(pathInServer);
  return;
}

void SqlHardWorkerUnit::addReport(Report& report, const quint32 taskId) const
{
  synchronizeDateTime();
  
  if( !(report.isValid())||(taskId == 0) )
  {
    QString errorText = "invalid argument";
    qCritical()<<errorText;
    throw NeedFixCode(errorText);
  }
  
  _exchangeUnit->beginTransaction();
  tryToAddReportDataToSqlServer(report, taskId);
  tryToAddReportDataToFtpServer(report);
  _exchangeUnit->commitTransaction();
  return;
}

void SqlHardWorkerUnit::tryToAddReportDataToSqlServer(Report& report, const quint32 taskId) const
{
  try
  {
    addReportData(report);
    quint32 reportId = defineIdOfJustAddedReport();
    report.setId(reportId);
    addFilesIfExist(report);
    addReportForTask(taskId, reportId);
  }
  catch(...)
  {
    _exchangeUnit->rollbackTransaction();
    qCritical()<<"error: adding report to database failed. catched exception";
    throw;
  }
  return;
}

void SqlHardWorkerUnit::addReportData(const Report& report) const
{
  const QString TEXT_PLACEHOLDER = ":text_placeholder";
  const QString CREATION_DATE_PLACEHOLDER = ":creationDate_placeholder";
  QString queryText = "INSERT INTO TrackerDB.reports ("
                        "text, "
                        "creationDate)"
                      "VALUES ("
                       + TEXT_PLACEHOLDER
                 + "," + CREATION_DATE_PLACEHOLDER + ");";
  _exchangeUnit->getReadyFor(queryText);
  
  QString text = report.getText();
  QDate creationDate = report.getCreationDate();
  
  _exchangeUnit->loadPlaceholder(TEXT_PLACEHOLDER, text);
  _exchangeUnit->loadPlaceholder(CREATION_DATE_PLACEHOLDER, creationDate);
  
  _exchangeUnit->executeQuery();
  return;
}

quint32 SqlHardWorkerUnit::defineIdOfJustAddedReport() const
{
  QList<SqlRecord> queryResult = executeGetMaxReportIdQuery();
  quint32 maxId = extractMaxIdFromQueryResult(queryResult);
  return  maxId;
}

QList<SqlRecord> SqlHardWorkerUnit::executeGetMaxReportIdQuery() const
{
  QString queryText = "SELECT MAX(idReport) "
                        "FROM TrackerDB.reports;";
  _exchangeUnit->getReadyFor(queryText);
  QList<SqlRecord> queryResult = _exchangeUnit->executeQuery();
  return queryResult;
}

void SqlHardWorkerUnit::addFilesIfExist(const Report& report) const
{
  quint32  reportId = report.getId();
  QStringList files = report.getFiles();
  if( !(files.isEmpty()) )
  {
    addFilesForReport(reportId, files);
  }
  return;
}

void SqlHardWorkerUnit::addFilesForReport(const quint32 reportId, const QStringList& files) const
{
  QString queryText = "INSERT INTO TrackerDB.filesReports ("
                        "filePath, "
                        "idReport)"
                      "VALUES ";
  QStringList filesPlaceholders;
  QStringList reportsIdsPlaceholders;
  QString currentFilePlaceholder;
  QString currentReportIdPlaceholder;
  QString filePlaceholderBody = ":file_placeholder";
  QString reportIdPlaceholderBody = ":task_placeholder";
  QString placeholderNumber;
  quint32 filesQuantity = files.size();
  const quint32 LAST_INDEX = filesQuantity - 1;
  for(quint32 index=0; index<filesQuantity; index++)
  {
    placeholderNumber = transformInText(index);
    currentFilePlaceholder = filePlaceholderBody + placeholderNumber;
    currentReportIdPlaceholder = reportIdPlaceholderBody + placeholderNumber;
    filesPlaceholders.append(currentFilePlaceholder);
    reportsIdsPlaceholders.append(currentReportIdPlaceholder);
    queryText += "(" + currentFilePlaceholder + "," + currentReportIdPlaceholder + ")";
    if(index < LAST_INDEX)
    {
      queryText += ",";
    }
  }
  queryText += ";";
  
  _exchangeUnit->getReadyFor(queryText);
  
  QString currentFile;
  for(quint32 index=0; index<filesQuantity; index++)
  {
    currentFile = files.at(index);
    currentFilePlaceholder = filesPlaceholders.at(index);
    currentReportIdPlaceholder = reportsIdsPlaceholders.at(index);
    _exchangeUnit->loadPlaceholder(currentFilePlaceholder, currentFile);
    _exchangeUnit->loadPlaceholder(currentReportIdPlaceholder, reportId);
  }
  
  _exchangeUnit->executeQuery();
  return;
}

void SqlHardWorkerUnit::tryToAddReportDataToFtpServer(const Report& report)const
{
  try
  {
    loadReportFilesToFtpServer(report);
  }
  catch(...)
  {
    _exchangeUnit->rollbackTransaction();
    removeFtpFilesAfterAddingReport(report);
    qCritical()<<"catched exception while loading report files to ftp-server";
    throw;
  }
  return;
}

void SqlHardWorkerUnit::loadReportFilesToFtpServer(const Report& report) const
{
  quint32 reportId = report.getId();
  QString reportIdAsText = transformInText(reportId);
  QStringList files = report.getFiles();
  QString filePathInServer = "/Reports/id_" + reportIdAsText + "/";
  
  uploadFilesToFtpServer(files, filePathInServer);
  return;
}

void SqlHardWorkerUnit::removeFtpFilesAfterAddingReport(const Report& report) const
{
  quint32 reportId = report.getId();
  QString reportIdAsText = transformInText(reportId);
  QString pathInServer = "/Reports/id_" + reportIdAsText + "/";
  
  removeFolderFromFtpServer(pathInServer);
  return;
}

Task SqlHardWorkerUnit::getTask(const qint32 taskId) const
{
  if(taskId == 0)
  {
    QString errorText = "invalid argument: #id == 0";
    qCritical()<<errorText;
    throw NeedFixCode(errorText);
  }
  
  QString idAsText  = transformInText(taskId);
  QString condition = "idTasks = " + idAsText;
  QList<Task> tasks = getTasksByCondition(condition);
  
  const quint32 REQUIRED_TASKS_QUANTITY = 1;
  quint32 receivedTasksQuantity = tasks.size();
  if(receivedTasksQuantity != REQUIRED_TASKS_QUANTITY)
  {
    QString errorText = "there is more than 1 task in database have id #" + idAsText;
    qCritical()<<errorText;
    throw NeedFixCode(errorText);
  }
  
  Task task = tasks.first();
  return task;
}

QList<Task> SqlHardWorkerUnit::getTasksRequestedBy(const quint32 authorId) const
{
  if(authorId == 0)
  {
    QString errorText = "invalid argument: #workerId == 0";
    qCritical()<<errorText;
    throw NeedFixCode(errorText);
  }
  
  QString authorIdAsText = transformInText(authorId);
  QString condition = "author = " + authorIdAsText;
  QList<Task> tasks = getTasksByCondition(condition);
  return tasks;
}

QList<Task> SqlHardWorkerUnit::getAllWorkerTasks(const quint32 workerId) const
{
  if(workerId == 0)
  {
    QString errorText = "invalid argument: #workerId == 0";
    qCritical()<<errorText;
    throw NeedFixCode(errorText);
  }
  
  QString workerIdAsText = transformInText(workerId);
  QString condition = "(responsible = " + workerIdAsText + ")OR"
                      "(author = "      + workerIdAsText + ")OR"
                      "(idTasks IN(SELECT idTask "
                                  "FROM TrackerDB.relationshipsTaskAccomplices "
                                  "WHERE idWorker = " + workerIdAsText + "))";
  QList<Task> tasks = getTasksByCondition(condition);
  return tasks;
}

QList<Task> SqlHardWorkerUnit::getTasksForResponsible(const quint32 responsibleId) const
{
  if(responsibleId == 0)
  {
    QString errorText = "invalid argument: #responsibleId == 0";
    qCritical()<<errorText;
    throw NeedFixCode(errorText);
  }
  
  QString responsibleIdAsText = transformInText(responsibleId);
  QString condition = "(responsible = " + responsibleIdAsText + ")OR"
                      "(idTasks IN(SELECT idTask "
                                  "FROM TrackerDB.relationshipsTaskAccomplices "
                                  "WHERE idWorker = " + responsibleIdAsText + "))";
  QList<Task> tasks = getTasksByCondition(condition);
  return tasks;
}

QList<Task> SqlHardWorkerUnit::getCompleteTasksForWorker(const quint32 workerId) const
{
  if(workerId == 0)
  {
    QString errorText = "invalid argument: #workerId == 0";
    qCritical()<<errorText;
    throw NeedFixCode(errorText);
  }
  
  QString completeStatusAsText = transformInText(Task::complete);
  QString canceledStatusAsText = transformInText(Task::canceled);
  QString workerIdAsText = transformInText(workerId);
  QString condition = "((status = " + completeStatusAsText + ")OR"
                       "(status = " + canceledStatusAsText + "))"
                                                                "AND"
                      "((responsible = " + workerIdAsText + ")OR"
                       "(author = "      + workerIdAsText + ")OR"
                       "(idTasks IN(SELECT idTask "
                                   "FROM TrackerDB.relationshipsTaskAccomplices "
                                   "WHERE idWorker = " + workerIdAsText + ")))";
  
  QList<Task> tasks = getTasksByCondition(condition);
  return tasks;
}

QList<Task> SqlHardWorkerUnit::getTasksByCondition(const QString& condition) const
{
  synchronizeDateTime();
  
  if( condition.isEmpty() )
  {
    QString errorText = "invalid argument: #condition is empty string";
    qCritical()<<errorText;
    throw NeedFixCode(errorText);
  }
  
  QString queryText = "SELECT "
                        "idTasks, "
                        "description, "
                        "author, "
                        "responsible, "
                        "creationDate, "
                        "deadlineDate, "
                        "status, "
                        "isOverdue, "
                        "importance, "
                        "parentTaskId "
                      "FROM TrackerDB.Tasks "
                      "WHERE (" + condition + ");";
  
  QList<SqlRecord> queryResult = executeSelectQuery(queryText);
  QList<Task> tasks = extractTasksDataFromQueryResultAndRelatedTables(queryResult);
  return tasks;
}

QList<Task> SqlHardWorkerUnit::extractTasksDataFromQueryResultAndRelatedTables(QList<SqlRecord>& queryResult) const
{
  if( queryResult.isEmpty() )
  {
    QString errorText = "invalid argument: #queryResult is empty";
    qCritical()<<errorText;
    throw NeedFixCode(errorText);
  }
  
  QList<Task> tasks;
  Task currentTask;
  SqlRecord currentRecord;
  while( !(queryResult.isEmpty()) )
  {
    currentRecord = queryResult.takeFirst();
    currentTask = extractTaskDataFromRecord(currentRecord);
    
    fillTaskWithAccomplices(currentTask);
    fillTaskWithComment(currentTask);
    fillTaskWithWarningDate(currentTask);
    fillTaskWithOrders(currentTask);
    fillTaskWithProjects(currentTask);
    fillTaskWithFiles(currentTask);
    fillTaskWithReport(currentTask);
    
    tasks.push_back(currentTask);
  }
  return tasks;
}

Task SqlHardWorkerUnit::extractTaskDataFromRecord(const SqlRecord& record) const
{
  const quint32 REQUIRED_FIELDS_QUANTITY = ttc_fields_quantity;
  quint32 fieldsQuantityInRecord = record.size();
  if(fieldsQuantityInRecord != REQUIRED_FIELDS_QUANTITY)
  {
    QString errorText = "invalid argument: #record contains wrong quantity of fields";
    qCritical()<<errorText;
    throw NeedFixCode(errorText);
  }
  
  bool isIdOk = false;
  bool isAuthorOk = false;
  bool isStatusOk = false;
  bool isResponsibleOk  = false;
  bool isImportanceOk   = false;
  bool isParentTaskIdOk = false;
  quint32 id = record.at(ttc_id).toUInt(&isIdOk);
  quint32 status = record.at(ttc_status).toUInt(&isStatusOk);
  quint32 authorId = record.at(ttc_author).toUInt(&isAuthorOk);
  quint32 importance = record.at(ttc_importance).toUInt(&isImportanceOk);
  quint32 parentTaskId = record.at(ttc_parent_task_id).toUInt(&isParentTaskIdOk);
  quint32 responsibleId = record.at(ttc_responsible).toUInt(&isResponsibleOk);
  QString description = record.at(ttc_description).toString();
  QDate creationDate  = record.at(ttc_creation_date).toDate();
  QDate deadlineDate  = record.at(ttc_deadline_date).toDate();
  bool  isOverdue = record.at(ttc_is_overdue).toBool();
  //[проверяем успешность приведения к требуемым типам]
  if( (description.isEmpty())||
     !(creationDate.isValid())||
     !(deadlineDate.isValid())||
      (isIdOk           == false)||
      (isAuthorOk       == false)||
      (isImportanceOk   == false)||
      (isResponsibleOk  == false)||
      (isStatusOk       == false)||
      (isParentTaskIdOk == false) )
  {
    QString errorText = "(strange shit) error converting from QVariant";
    qCritical()<<errorText;
    throw CriticalOperationException(errorText);
  }
  
  Task task;
  task.setIdTask(id);
  task.setDescription(description);
  task.setAuthor(authorId);
  task.setResponsible(responsibleId);
  task.setCreationDate(creationDate);
  task.setDeadlineDate(deadlineDate);
  task.setStatus(status);
  task.setIsOverdue(isOverdue);
  task.setImportance(importance);
  task.setParent(parentTaskId);
  return task;
}

void SqlHardWorkerUnit::fillTaskWithAccomplices(Task& task) const
{
  try
  {
    quint32 id = task.getIdTask();
    QList<quint32> accomplices = getAccomplicesForTask(id);
    task.setAccomplices(accomplices);
  }
  catch(ExecutionAborted){/*если нет соисполнителей - то и фиг с ними*/}
  return;
}

QList<quint32> SqlHardWorkerUnit::getAccomplicesForTask(const quint32 taskId) const
{
  if(taskId == 0)
  {
    QString errorText = "invalid argument: #taskId == 0";
    qCritical()<<errorText;
    throw NeedFixCode(errorText);
  }
  
  QString taskIdAsText = transformInText(taskId);
  QString queryText = "SELECT idWorker "
                      "FROM TrackerDB.relationshipsTaskAccomplices "
                      "WHERE idTask = " + taskIdAsText + ";";
  
  QList<SqlRecord> queryResult = executeSelectQuery(queryText);
  QList<quint32> accomplices = extractAccomplicesFromQueryResult(queryResult);
  return accomplices;
}

QList<quint32> SqlHardWorkerUnit::extractAccomplicesFromQueryResult(QList<SqlRecord>& queryResult) const
{
  if( queryResult.isEmpty() )
  {
    QString errorText = "invalid argument: #queryResult is empty";
    qCritical()<<errorText;
    throw NeedFixCode(errorText);
  }
  
  QList<quint32> accomplices;
  quint32 accompliceId = 0;
  SqlRecord currentRecord;
  while( !(queryResult.isEmpty()) )
  {
    currentRecord = queryResult.takeFirst();
    accompliceId = extractAccomliceFromRecord(currentRecord);
    accomplices.push_back(accompliceId);
  }
  return accomplices;
}

quint32 SqlHardWorkerUnit::extractAccomliceFromRecord(const SqlRecord& record) const
{
  checkForTheOnlyFieldInRecord(record);
  
  bool isOk = false;
  quint32 accompliceId = record.first().toUInt(&isOk);
  if( !isOk )
  {
    QString errorText = "(strange shit) error converting from QVariant";
    qCritical()<<errorText;
    throw CriticalOperationException(errorText);
  }
  return accompliceId;
}

void SqlHardWorkerUnit::fillTaskWithComment(Task& task) const
{
  try
  {
    quint32 id = task.getIdTask();
    QString comment = getCommentForTask(id);
    task.setComment(comment);
  }
  catch(ExecutionAborted){/*если нет комментария - то и фиг с ним*/}
  return;
}

QString SqlHardWorkerUnit::getCommentForTask(const quint32 taskId) const
{
  if(taskId == 0)
  {
    QString errorText = "invalid argument: #taskId == 0";
    qCritical()<<errorText;
    throw NeedFixCode(errorText);
  }
  
  QString taskIdAsText = transformInText(taskId);
  QString queryText = "SELECT Comment "
                      "FROM TrackerDB.comments "
                      "WHERE idTasks = " + taskIdAsText + ";";
  
  QList<SqlRecord> queryResult = executeSelectQuery(queryText);
  QString comment = extractCommentFromQueryResult(queryResult);
  return  comment;
}

QString SqlHardWorkerUnit::extractCommentFromQueryResult(QList<SqlRecord>& queryResult) const
{
  checkForTheOnlyRecordInQueryResult(queryResult);
  
  SqlRecord record = queryResult.takeFirst();
  QString comment = extractCommentFromRecord(record);
  return  comment;
}

QString SqlHardWorkerUnit::extractCommentFromRecord(const SqlRecord& record) const
{
  checkForTheOnlyFieldInRecord(record);
  
  QString comment = record.first().toString();
  if( comment.isEmpty() )
  {
    QString errorText = "(strange shit) error converting from QVariant";
    qCritical()<<errorText;
    throw CriticalOperationException(errorText);
  }
  return comment;
}

void SqlHardWorkerUnit::fillTaskWithWarningDate(Task& task) const
{
  try
  {
    quint32 id = task.getIdTask();
    QDate warningDate = getWarningDateForTask(id);
    task.setWarningDate(warningDate);
  }
  catch(ExecutionAborted){/*если нет даты напоминания - то и фиг с ней*/}
  return;
}

QDate SqlHardWorkerUnit::getWarningDateForTask(const quint32 taskId) const
{
  //[проверяем корректность аргументов]
  if(taskId == 0)
  {
    QString errorMessage = "invalid argument: #taskId == 0";
    qCritical()<<errorMessage;
    throw NeedFixCode(errorMessage);
  }
  
  QString taskIdAsText = transformInText(taskId);
  QString queryText = "SELECT date "
                      "FROM TrackerDB.warningDates "
                      "WHERE idTask = " + taskIdAsText;
  
  QList<SqlRecord> queryResult = executeSelectQuery(queryText);
  QDate  warningDate = extractWarningDateFromQueryResult(queryResult);
  return warningDate;
}

QDate SqlHardWorkerUnit::extractWarningDateFromQueryResult(QList<SqlRecord>& queryResult) const
{
  checkForTheOnlyRecordInQueryResult(queryResult);
  
  SqlRecord   record = queryResult.takeFirst();
  QDate  warningDate = extractWarningDateFromRecord(record);
  return warningDate;
}

QDate SqlHardWorkerUnit::extractWarningDateFromRecord(const SqlRecord& record) const
{
  checkForTheOnlyFieldInRecord(record);
  
  QDate warningDate = record.first().toDate();
  if( !(warningDate.isValid()) )
  {
    QString errorText = "(strange shit) error converting from QVariant";
    qCritical()<<errorText;
    throw CriticalOperationException(errorText);
  }
  return warningDate;
}

void SqlHardWorkerUnit::fillTaskWithOrders(Task& task) const
{
  try
  {
    quint32 taskId = task.getIdTask();
    QList<quint32> ordersIds = getOrdersForTask(taskId);
    quint32 orderId = ordersIds.takeLast();
    task.setOrder(orderId);
  }
  catch(ExecutionAborted){/*если нет заказов - то и фиг с ними*/}
  return;
}

QList<quint32> SqlHardWorkerUnit::getOrdersForTask(const quint32 taskId) const
{
  if(taskId == 0)
  {
    QString errorText = "invalid argument: #taskId == 0";
    qCritical()<<errorText;
    throw NeedFixCode(errorText);
  }
  
  QString taskIdAsText = transformInText(taskId);
  QString queryText = "SELECT idOrder "
                      "FROM TrackerDB.relationshipsTaskOrder "
                      "WHERE idTask = " + taskIdAsText;
  
  QList<SqlRecord> queryResult = executeSelectQuery(queryText);
  QList<quint32> ordersIds = extractOrdersFromQueryResult(queryResult);
  return ordersIds;
}

QList<quint32> SqlHardWorkerUnit::extractOrdersFromQueryResult(QList<SqlRecord>& queryResult) const
{
  if( queryResult.isEmpty() )
  {
    QString errorText = "invalid argument: #queryResult is empty";
    qCritical()<<errorText;
    throw NeedFixCode(errorText);
  }
  
  QList<quint32> ordersIds;
  quint32 currentOrderId = 0;
  SqlRecord currentRecord;
  while( !(queryResult.isEmpty()) )
  {
    currentRecord = queryResult.takeFirst();
    currentOrderId = extractOrderFromRecord(currentRecord);
    ordersIds.push_back(currentOrderId);
  }
  return ordersIds;
}

quint32 SqlHardWorkerUnit::extractOrderFromRecord(const SqlRecord& record) const
{
  checkForTheOnlyFieldInRecord(record);
  
  bool isOk = false;
  quint32 orderId = record.first().toUInt(&isOk);
  if( !isOk )
  {
    QString errorText = "(strange shit) error converting from QVariant";
    qCritical()<<errorText;
    throw CriticalOperationException(errorText);
  }
  return orderId;
}

void SqlHardWorkerUnit::fillTaskWithProjects(Task& task) const
{
  try
  {
    quint32 taskId = task.getIdTask();
    QList<quint32> projectsIds = getProjectsForTask(taskId);
    quint32 projectId = projectsIds.takeLast();
    task.setProject(projectId);
  }
  catch(ExecutionAborted){/*если нет проектов - то и фиг с ним*/}
  return;
}

QList<quint32> SqlHardWorkerUnit::getProjectsForTask(const quint32 taskId) const
{
  if(taskId == 0)
  {
    QString errorText = "invalid argument: #taskId == 0";
    qCritical()<<errorText;
    throw NeedFixCode(errorText);
  }
  
  QString taskIdAsText = transformInText(taskId);
  QString queryText = "SELECT idProject "
                      "FROM TrackerDB.relationshipsTaskProject "
                      "WHERE idTask = " + taskIdAsText;
  
  QList<SqlRecord> queryResult = executeSelectQuery(queryText);
  QList<quint32> projectsIds = extractProjectsFromQueryResult(queryResult);
  return projectsIds;
}

QList<quint32> SqlHardWorkerUnit::extractProjectsFromQueryResult(QList<SqlRecord>& queryResult) const
{
  if( queryResult.isEmpty() )
  {
    QString errorText = "invalid argument: #queryResult is empty";
    qCritical()<<errorText;
    throw NeedFixCode(errorText);
  }
  
  QList<quint32> projectsIds;
  quint32 currentProjectId = 0;
  SqlRecord currentRecord;
  while( !(queryResult.isEmpty()) )
  {
    currentRecord = queryResult.takeFirst();
    currentProjectId = extractProjectFromRecord(currentRecord);
    projectsIds.push_back(currentProjectId);
  }
  return projectsIds;
}

quint32 SqlHardWorkerUnit::extractProjectFromRecord(const SqlRecord& record) const
{
  checkForTheOnlyFieldInRecord(record);
  
  bool isOk = false;
  quint32 projectId = record.first().toUInt(&isOk);
  if( !isOk )
  {
    QString errorText = "(strange shit) error converting from QVariant";
    qCritical()<<errorText;
    throw CriticalOperationException(errorText);
  }
  return projectId;
}

void SqlHardWorkerUnit::fillTaskWithFiles(Task& task) const
{
  try
  {
    quint32 id = task.getIdTask();
    QStringList files = getFilesForTask(id);
    task.setFilesTask(files);
  }
  catch(ExecutionAborted){/*если нет файлов-вложений - то и фиг с ними*/}
  return;
}

QStringList SqlHardWorkerUnit::getFilesForTask(const quint32 taskId) const
{
  if(taskId == 0)
  {
    QString errorText = "invalid argument: #taskId == 0";
    qCritical()<<errorText;
    throw NeedFixCode(errorText);
  }
  
  QString taskIdAsText = transformInText(taskId);
  QString queryText = "SELECT filePath "
                      "FROM TrackerDB.filesTasks "
                      "WHERE idTask = " + taskIdAsText;
  
  QList<SqlRecord> queryResult = executeSelectQuery(queryText);
  QStringList files = extractFilesFromQueryResult(queryResult);
  return files;
}

QStringList SqlHardWorkerUnit::extractFilesFromQueryResult(QList<SqlRecord>& queryResult) const
{
  if( queryResult.isEmpty() )
  {
    QString errorText = "invalid argument: #queryResult is empty";
    qCritical()<<errorText;
    throw NeedFixCode(errorText);
  }
  
  QStringList files;
  QString currentFile;
  SqlRecord currentRecord;
  while( !(queryResult.isEmpty()) )
  {
    currentRecord = queryResult.takeFirst();
    currentFile = extractFileFromRecord(currentRecord);
    files.push_back(currentFile);
  }
  return files;
}

QString SqlHardWorkerUnit::extractFileFromRecord(const SqlRecord& record) const
{
  checkForTheOnlyFieldInRecord(record);
  
  QString file = record.first().toString();
  if( file.isEmpty() )
  {
    QString errorText = "(strange shit) error converting from QVariant";
    qCritical()<<errorText;
    throw CriticalOperationException(errorText);
  }
  return file;
}

void SqlHardWorkerUnit::fillTaskWithReport(Task& task) const
{
  try
  {
    quint32 taskId = task.getIdTask();
    quint32 reportId = getReportForTask(taskId);
    task.setReportId(reportId);
  }
  catch(ExecutionAborted){/*если нет айди отчёта - то и фиг с ним*/}
  return;
}

quint32 SqlHardWorkerUnit::getReportForTask(const quint32 taskId) const
{
  if(taskId == 0)
  {
    QString errorText = "invalid argument: #taskId == 0";
    qCritical()<<errorText;
    throw NeedFixCode(errorText);
  }
  
  QString taskIdAsText = transformInText(taskId);
  QString queryText = "SELECT "
                        "idReport "
                      "FROM TrackerDB.relationshipsTasksReports "
                      "WHERE idTask = " + taskIdAsText + ";";
  
  QList<SqlRecord> queryResult = executeSelectQuery(queryText);
  quint32 reportId = extractReportFromQueryResult(queryResult);
  return  reportId;
}

quint32 SqlHardWorkerUnit::extractReportFromQueryResult(QList<SqlRecord>& queryResult) const
{
  checkForTheOnlyRecordInQueryResult(queryResult);
  
  SqlRecord record = queryResult.first();
  quint32 reportId = extractReportFromRecord(record);
  return  reportId;
}

quint32 SqlHardWorkerUnit::extractReportFromRecord(const SqlRecord& record) const
{
  checkForTheOnlyFieldInRecord(record);
  
  bool isOk = false;
  quint32 reportId = record.first().toUInt(&isOk);
  if( !isOk )
  {
    QString errorText = "(strange shit) error converting from QVariant";
    qCritical()<<errorText;
    throw CriticalOperationException(errorText);
  }
  return reportId;
}

Worker SqlHardWorkerUnit::getWorker(const QString& login) const
{
  if(login.isEmpty())
  {
    QString errorText = "invalid argument: #login is empty string";
    qCritical()<<errorText;
    throw NeedFixCode(errorText);
  }
  
  QString placeholder = ":login";
  QString queryText = "SELECT "
                        "idWorker, "
                        "name, "
                        "surname, "
                        "patronymic, "
                        "position, "
                        "email, "
                        "login, "
                        "pass, "
                        "isGod "
                      "FROM TrackerDB.workers "
                      "WHERE (login = " + placeholder + ");";
  _exchangeUnit->getReadyFor(queryText);
  _exchangeUnit->loadPlaceholder(placeholder, login);
  QList<SqlRecord> queryResult = _exchangeUnit->executeQuery();
  
  QList<Worker> workers = extractWorkersDataFromQueryResultAndRelatedTables(queryResult);
  
  quint32 receivedWorkersQuantity = workers.size();
  if(receivedWorkersQuantity == 0)
  {
    throw ExecutionAborted("there is no workers in database for specified login");
  }
  
  const quint32 REQUIRED_WORKERS_QUANTITY = 1;
  if(receivedWorkersQuantity != REQUIRED_WORKERS_QUANTITY)
  {
    QString errorText = "there is more than 1 worker in database have login #" + login;
    qCritical()<<errorText;
    throw NeedFixCode(errorText);
  }
  
  Worker worker = workers.first();
  return worker;
}

Worker SqlHardWorkerUnit::getWorker(const quint32 workerId) const
{
  if(workerId == 0)
  {
    QString errorText = "invalid argument: #workerId == 0";
    qCritical()<<errorText;
    throw NeedFixCode(errorText);
  }
  
  QString workerIdAsText = transformInText(workerId);
  QString condition = "idWorker = " + workerIdAsText;
  QList<Worker> workers = getWorkersByCondition(condition);
  
  const quint32 REQUIRED_WORKERS_QUANTITY = 1;
  quint32 receivedWorkersQuantity = workers.size();
  if(receivedWorkersQuantity != REQUIRED_WORKERS_QUANTITY)
  {
    QString errorText = "there is more than 1 worker in database have id #" + workerIdAsText;
    qCritical()<<errorText;
    throw NeedFixCode(errorText);
  }
  
  Worker worker = workers.first();
  return worker;
}

QList<Worker> SqlHardWorkerUnit::getListOfResponsibleForWorker(const quint32 masterId) const
{
  if( masterId == 0 )
  {
    QString errorText = "invalid argument: #masterId == 0";
    qCritical()<<errorText;
    throw NeedFixCode(errorText);
  }
  
  QString masterIdAsText = transformInText(masterId);
  QString condition = "idWorker IN (SELECT idSlave "
                                   "FROM TrackerDB.relationshipsMasterSlave "
                                   "WHERE idMaster = " + masterIdAsText + ")";
  QList<Worker> slaves = getWorkersByCondition(condition);
  return slaves;
}

QList<Worker> SqlHardWorkerUnit::getWorkers(const QList<quint32>& workersIds) const
{
  if( workersIds.isEmpty() )
  {
    throw ExecutionAborted("not requested any workers: #workersIds is empty");
  }
  
  QString workersIdsAsText = transformListOfIdsInText(workersIds);
  QString condition = "idWorker IN (" + workersIdsAsText + ")";
  QList<Worker> workers = getWorkersByCondition(condition);
  return workers;
}

QList<Worker> SqlHardWorkerUnit::getWorkersByCondition(const QString& condition) const
{
  synchronizeDateTime();
  
  if( condition.isEmpty() )
  {
    QString errorText = "invalid argument: #condition is empty string";
    qCritical()<<errorText;
    throw NeedFixCode(errorText);
  }
  
  QString queryText = "SELECT "
                        "idWorker, "
                        "name, "
                        "surname, "
                        "patronymic, "
                        "position, "
                        "email, "
                        "login, "
                        "pass, "
                        "isGod "
                      "FROM TrackerDB.workers "
                      "WHERE (" + condition + ");";
  
  QList<SqlRecord> queryResult = executeSelectQuery(queryText);
  QList<Worker> workers = extractWorkersDataFromQueryResultAndRelatedTables(queryResult);
  return workers;
}

QList<Worker> SqlHardWorkerUnit::extractWorkersDataFromQueryResultAndRelatedTables(QList<SqlRecord>& queryResult) const
{
  if( queryResult.isEmpty() )
  {
    QString errorText = "invalid argument: #queryResult is empty";
    qCritical()<<errorText;
    throw NeedFixCode(errorText);
  }
  
  QList<Worker> workers;
  Worker currentWorker;
  SqlRecord currentRecord;
  while( !(queryResult.isEmpty()) )
  {
    currentRecord = queryResult.takeFirst();
    currentWorker = extractWorkerDataFromRecord(currentRecord);
    
    fillWorkerWithVices(currentWorker);
    
    workers.push_back(currentWorker);
  }
  return workers;
}

Worker SqlHardWorkerUnit::extractWorkerDataFromRecord(const SqlRecord& record) const
{
  const quint32 REQUIRED_FIELDS_QUANTITY = wtc_fields_quantity;
  quint32 fieldsQuantityInRecord = record.size();
  if(fieldsQuantityInRecord != REQUIRED_FIELDS_QUANTITY)
  {
    QString errorText = "invalid argument: #record contains wrong quantity of fields";
    qCritical()<<errorText;
    throw NeedFixCode(errorText);
  }
  
  bool isOk  = false;
  quint32 id = record.at(wtc_id).toUInt(&isOk);
  QString name  = record.at(wtc_name).toString();
  QString pass  = record.at(wtc_pass).toString();
  QString email = record.at(wtc_email).toString();
  QString login = record.at(wtc_login).toString();
  QString surname  = record.at(wtc_surname).toString();
  QString position = record.at(wtc_position).toString();
  QString patronymic = record.at(wtc_patronymic).toString();
  bool isGod = record.at(wtc_is_god).toBool();
  //[проверяем, успешность извлечения информации из QVariant в QString]
  if( name.isEmpty()||
      surname.isEmpty()||
      patronymic.isEmpty()||
      position.isEmpty()||
      email.isEmpty()||
      login.isEmpty()||
      pass.isEmpty()||
      (isOk == false) )
  {
    QString errorText = "(strange shit) error converting from QVariant";
    qCritical()<<errorText;
    throw CriticalOperationException(errorText);
  }
  
  Worker worker;
  worker.setId(id);
  worker.setName(name);
  worker.setSurname(surname);
  worker.setPatronymic(patronymic);
  worker.setPosition(position);
  worker.setEmail(email);
  worker.setLogin(login);
  worker.setPass(pass);
  worker.setIsGod(isGod);
  return worker;
}

void SqlHardWorkerUnit::fillWorkerWithVices(Worker& worker) const
{
  try
  {
    quint32 workerId = worker.getId();
    QList<quint32> vicesIds = getVicesForWorker(workerId);
    quint32 viceId = vicesIds.takeLast();
    worker.setVice(viceId);
  }
  catch(ExecutionAborted){/* если нет заместителей - то и фиг с ними */}
  return;
}

QList<quint32> SqlHardWorkerUnit::getVicesForWorker(const quint32 workerId) const
{
  if( workerId == 0 )
  {
    QString errorText = "invalid argument: #workerId == 0";
    qCritical()<<errorText;
    throw NeedFixCode(errorText);
  }
  
  QString workerIdAsText = transformInText(workerId);
  QString queryText = "SELECT "
                        "idVices "
                      "FROM TrackerDB.vices "
                      "WHERE idWorker = " + workerIdAsText + ";";
  
  QList<SqlRecord> queryResult = executeSelectQuery(queryText);
  QList<quint32> vicesIds = extractVicesFromQueryResult(queryResult);
  return vicesIds;
}

QList<quint32> SqlHardWorkerUnit::extractVicesFromQueryResult(QList<SqlRecord>& queryResult) const
{
  if( queryResult.isEmpty() )
  {
    QString errorText = "invalid argument: #queryResult is empty";
    qCritical()<<errorText;
    throw NeedFixCode(errorText);
  }
  
  QList<quint32> vicesIds;
  quint32 currentViceId = 0;
  SqlRecord currentRecord;
  while( !(queryResult.isEmpty()) )
  {
    currentRecord = queryResult.takeFirst();
    currentViceId = extractViceFromRecord(currentRecord);
    vicesIds.push_back(currentViceId);
  }
  return vicesIds;
}

quint32 SqlHardWorkerUnit::extractViceFromRecord(const SqlRecord& record) const
{
  checkForTheOnlyFieldInRecord(record);
  
  bool isOk = false;
  quint32 viceId = record.first().toUInt(&isOk);
  if( !isOk )
  {
    QString errorText = "(strange shit) error converting from QVariant";
    qCritical()<<errorText;
    throw CriticalOperationException(errorText);
  }
  return viceId;
}

Order SqlHardWorkerUnit::getOrder(const quint32 orderId) const
{
  if(orderId == 0)
  {
    QString errorText = "invalid argument: #orderId == 0";
    qCritical()<<errorText;
    throw NeedFixCode(errorText);
  }
  
  QString orderIdAsText = transformInText(orderId);
  QString condition = "idOrder = " + orderIdAsText;
  QList<Order> orders = getOrdersByCondition(condition);
  
  quint32 receivedOrdersQuantity = orders.size();
  const quint32 REQUIRED_ORDERS_QUANTITY = 1;
  if(receivedOrdersQuantity != REQUIRED_ORDERS_QUANTITY)
  {
    QString errorText = "there is more than 1 order in database with id #" + orderIdAsText;
    qCritical()<<errorText;
    throw NeedFixCode(errorText);
  }
  
  Order order = orders.first();
  return order;
}

QList<Order> SqlHardWorkerUnit::getOrders(const QList<quint32>& ordersIds) const
{
  if( ordersIds.isEmpty() )
  {
    throw ExecutionAborted("not requested any orders: #ordersIds is empty");
  }
  
  QString ordersIdsAsText = transformListOfIdsInText(ordersIds);
  QString condition = "idOrder IN (" + ordersIdsAsText + ")";
  QList<Order> orders = getOrdersByCondition(condition);
  return orders;
}

QList<Order> SqlHardWorkerUnit::getAllOrders() const
{
  QList<Order> orders = getOrdersByCondition("idOrder > 0");
  return orders;
}

QList<Order> SqlHardWorkerUnit::getOrdersByCondition(const QString& condition) const
{
  synchronizeDateTime();
  
  if( condition.isEmpty() )
  {
    QString errorText = "invalid argument: #condition is empty string";
    qCritical()<<errorText;
    throw NeedFixCode(errorText);
  }
  
  QString queryText = "SELECT "
                        "idOrder, "
                        "description "
                      "FROM TrackerDB.orders "
                      "WHERE (" + condition + ");";
  
  QList<SqlRecord> queryResult = executeSelectQuery(queryText);
  QList<Order> orders = extractOrdersDataFromQueryResult(queryResult);
  return orders;
}

QList<Order> SqlHardWorkerUnit::extractOrdersDataFromQueryResult(QList<SqlRecord>& queryResult) const
{
  if( queryResult.isEmpty() )
  {
    QString errorText = "invalid argument: #queryResult is empty";
    qCritical()<<errorText;
    throw NeedFixCode(errorText);
  }
  
  QList<Order> orders;
  Order currentOrder;
  SqlRecord currentRecord;
  while( !(queryResult.isEmpty()) )
  {
    currentRecord = queryResult.takeFirst();
    currentOrder = extractOrderDataFromRecord(currentRecord);
    orders.push_back(currentOrder);
  }
  return orders;
}

Order SqlHardWorkerUnit::extractOrderDataFromRecord(const SqlRecord& record) const
{
  const quint32 REQUIRED_FIELDS_QUANTITY = otc_fields_quantity;
  quint32 fieldsQuantityInRecord = record.size();
  if(fieldsQuantityInRecord != REQUIRED_FIELDS_QUANTITY)
  {
    QString errorText = "invalid argument: #record contains wrong quantity of fields";
    qCritical()<<errorText;
    throw NeedFixCode(errorText);
  }
  
  bool isOk = false;
  quint32 id = record.at(otc_order_id).toUInt(&isOk);
  QString name = record.at(otc_description).toString();
  if( (isOk == false) || (name.isEmpty()) )
  {
    QString errorText = "(strange shit) error converting from QVariant";
    qCritical()<<errorText;
    throw CriticalOperationException(errorText);
  }
  
  Order order;
  order.setId(id);
  order.setName(name);
  return order;
}

Project SqlHardWorkerUnit::getProject(const quint32 projectId) const
{
  if(projectId == 0)
  {
    QString errorText = "invalid argument: #projectId == 0";
    qCritical()<<errorText;
    throw NeedFixCode(errorText);
  }
  
  QString projectIdAsText = transformInText(projectId);
  QString condition = "idProject = " + projectIdAsText;
  QList<Project> projects = getProjectsByCondition(condition);
  
  quint32 receivedProjectsQuantity = projects.size();
  const quint32 REQUIRED_PROJECTS_QUANTITY = 1;
  if(receivedProjectsQuantity != REQUIRED_PROJECTS_QUANTITY)
  {
    QString errorText = "there is more than 1 project in database with id #" + projectIdAsText;
    qCritical()<<errorText;
    throw NeedFixCode(errorText);
  }
  
  Project project = projects.first();
  return  project;
}

QList<Project> SqlHardWorkerUnit::getProjects(const QList<quint32>& projectsIds) const
{
  if( projectsIds.isEmpty() )
  {
    throw ExecutionAborted("not requested any projects: #projectsIds is empty");
  }
  
  QString projectsIdsAsText = transformListOfIdsInText(projectsIds);
  QString condition = "idProject IN (" + projectsIdsAsText + ")";
  QList<Project> projects = getProjectsByCondition(condition);
  return projects;
}

QList<Project> SqlHardWorkerUnit::getAllProjects() const
{
  QList<Project> projects = getProjectsByCondition("idProject > 0");
  return projects;
}

QList<Project> SqlHardWorkerUnit::getProjectsByCondition(const QString& condition) const
{
  synchronizeDateTime();
  
  if( condition.isEmpty() )
  {
    QString errorText = "invalid argument: #condition is empty";
    qCritical()<<errorText;
    throw NeedFixCode(errorText);
  }
  
  QString queryText = "SELECT "
                        "idProject, "
                        "description "
                      "FROM TrackerDB.projects "
                      "WHERE (" + condition + ");";
  
  QList<SqlRecord> queryResult = executeSelectQuery(queryText);
  QList<Project> projects = extractProjectsDataFromQueryResult(queryResult);
  return projects;
}

QList<Project> SqlHardWorkerUnit::extractProjectsDataFromQueryResult(QList<SqlRecord>& queryResult) const
{
  if( queryResult.isEmpty() )
  {
    QString errorText = "invalid argument: #queryResult is empty";
    qCritical()<<errorText;
    throw NeedFixCode(errorText);
  }
  
  QList<Project> projects;
  Project currentProject;
  SqlRecord currentRecord;
  while( !(queryResult.isEmpty()) )
  {
    currentRecord  = queryResult.takeFirst();
    currentProject = extractProjectDataFromRecord(currentRecord);
    projects.push_back(currentProject);
  }
  return projects;
}

Project SqlHardWorkerUnit::extractProjectDataFromRecord(const SqlRecord& record) const
{
  const quint32 REQUIRED_FIELDS_QUANTITY = ptc_fields_quantity;
  quint32 fieldsQuantityInRecord = record.size();
  if(fieldsQuantityInRecord != REQUIRED_FIELDS_QUANTITY)
  {
    QString errorText = "invalid argument: #record contains wrong quantity of fields";
    qCritical()<<errorText;
    throw NeedFixCode(errorText);
  }
  
  bool isOk = false;
  quint32 id = record.at(ptc_project_id).toUInt(&isOk);
  QString name = record.at(ptc_description).toString();
  if( (isOk == false) || (name.isEmpty()) )
  {
    QString errorText = "(strange shit) error converting from QVariant";
    qCritical()<<errorText;
    throw CriticalOperationException(errorText);
  }
  
  Project project;
  project.setId(id);
  project.setName(name);
  return project;
}

QList<Report> SqlHardWorkerUnit::getReports(const QList<quint32>& reportsIds) const
{
  if( reportsIds.isEmpty() )
  {
    throw ExecutionAborted("not requested any reports: #reportsIds is empty");
  }
  
  QString reportsIdsAsText = transformListOfIdsInText(reportsIds);
  QString condition = "idReport IN (" + reportsIdsAsText + ")";
  QList<Report> reports = getReportsByCondition(condition);
  return reports;
}

QList<Report> SqlHardWorkerUnit::getReportsByCondition(const QString& condition) const
{
  synchronizeDateTime();
  
  if( condition.isEmpty() )
  {
    QString errorText = "invalid argument: #condition is empty string";
    qCritical()<<errorText;
    throw NeedFixCode(errorText);
  }
  
  QString queryText = "SELECT "
                        "idReport, "
                        "text, "
                        "creationDate "
                      "FROM TrackerDB.reports "
                      "WHERE (" + condition + ");";
  
  QList<SqlRecord> queryResult = executeSelectQuery(queryText);
  QList<Report> reports = extractReportsDataFromQueryResultAndRelatedTables(queryResult);
  return reports;
}

QList<Report> SqlHardWorkerUnit::extractReportsDataFromQueryResultAndRelatedTables(QList<SqlRecord>& queryResult) const
{
  if( queryResult.isEmpty() )
  {
    QString errorText = "invalid argument: #queryResult is empty";
    qCritical()<<errorText;
    throw NeedFixCode(errorText);
  }
  
  QList<Report> reports;
  Report currentReport;
  SqlRecord currentRecord;
  while( !(queryResult.isEmpty()) )
  {
    currentRecord = queryResult.takeFirst();
    currentReport = extractReportDataFromRecord(currentRecord);
    
    fillReportWithFiles(currentReport);
    
    reports.push_back(currentReport);
  }
  return reports;
}

Report SqlHardWorkerUnit::extractReportDataFromRecord(const SqlRecord& record) const
{
  const quint32 REQUIRED_FIELDS_QUANTITY = rtc_fields_quantity;
  quint32 fieldsQuantityInRecord = record.size();
  if(fieldsQuantityInRecord != REQUIRED_FIELDS_QUANTITY)
  {
    QString errorText = "invalid argument: #record contains wrong quantity of fields";
    qCritical()<<errorText;
    throw NeedFixCode(errorText);
  }
  
  bool isOk = false;
  quint32 id = record.at(rtc_report_id).toInt(&isOk);
  QString text = record.at(rtc_text).toString();
  QDate creationDate = record.at(rtc_creation_date).toDate();
  //[проверяем успешность преобразования]
  if( (text.isEmpty())||
     !(creationDate.isValid())||
      (isOk == false) )
  {
    QString errorText = "(strange shit) error converting from QVariant";
    qCritical()<<errorText;
    throw CriticalOperationException(errorText);
  }
  
  Report report;
  report.setId(id);
  report.setText(text);
  report.setCreationDate(creationDate);
  return report;
}

void SqlHardWorkerUnit::fillReportWithFiles(Report& report) const
{
  try
  {
    qint32 reportId = report.getId();
    QStringList files = getFilesForReport(reportId);
    report.setFiles(files);
  }
  catch(ExecutionAborted){/* если нет файлов-вложений - то и фиг с ними */}
  return;
}

QStringList SqlHardWorkerUnit::getFilesForReport(const quint32 reportId) const
{
  if( reportId == 0 )
  {
    QString errorText = "invalid argument: #reportId == 0";
    qCritical()<<errorText;
    throw NeedFixCode(errorText);
  }
  
  QString reportIdAsText = transformInText(reportId);
  QString queryText = "SELECT filePath "
                      "FROM TrackerDB.filesReports "
                      "WHERE idReport = " + reportIdAsText + ";";
  
  QList<SqlRecord> queryResult = executeSelectQuery(queryText);
  QStringList files = extractFilesFromQueryResult(queryResult);
  return files;
}

void SqlHardWorkerUnit::updateSqlTaskData(const Task& updated) const
{
  synchronizeDateTime();
  
  if( !(updated.isValid()) )
  {
    QString errorText = "invalid argument: #updated";
    qCritical()<<errorText;
    throw NeedFixCode(errorText);
  }
  
  _exchangeUnit->beginTransaction();
  tryToUpdateTaskDataOnSqlServer(updated);
  _exchangeUnit->commitTransaction();
  return;
}

void SqlHardWorkerUnit::updateTask(const Task& updated) const
{
  synchronizeDateTime();
  
  if( !(updated.isValid()) )
  {
    QString errorText = "invalid argument: #updated";
    qCritical()<<errorText;
    throw NeedFixCode(errorText);
  }
  
  _exchangeUnit->beginTransaction();
  tryToUpdateTaskDataOnSqlServer(updated);
  tryToUpdateTaskDataOnFtpServer(updated);
  _exchangeUnit->commitTransaction();
  return;
}

void SqlHardWorkerUnit::tryToUpdateTaskDataOnSqlServer(const Task& updated) const
{
  try
  {
    updateTaskData(updated);
    deleteRelatedDataForTask(updated);
    addRelatedDataForTask(updated);
  }
  catch(...)
  {
    _exchangeUnit->rollbackTransaction();
    qCritical()<<"catched exception while updating task data on sql-server";
    throw;
  }
  return;
}

void SqlHardWorkerUnit::updateTaskData(const Task& updated) const
{
  const QString DESCRIPTION_PLACEHOLDER = ":description_placeholder";
  const QString IS_OVERDUE_PLACEHOLDER  = ":isOverdue_placeholder";
  const QString IMPORTANCE_PLACEHOLDER  = ":importance_placeholder";
  const QString RESPONSIBLE_ID_PLACEHOLDER = ":responsible_placeholder";
  const QString PARENT_TASK_ID_PLACEHOLDER = ":parentTaskId_placeholder";
  const QString CREATION_DATE_PLACEHOLDER  = ":creationDate_placeholder";
  const QString DEADLINE_DATE_PLACEHOLDER  = ":deadlineDate_placeholder";
  const QString AUTHOR_ID_PLACEHOLDER = ":author_placeholder";
  const QString STATUS_PLACEHOLDER = ":status_placeholder";
  const QString TASK_ID_PLACEHOLDER = ":taskId_placeholder";
  QString queryText = "UPDATE TrackerDB.Tasks "
                      "SET "
                        "description = " + DESCRIPTION_PLACEHOLDER + ","
                        "author = " + AUTHOR_ID_PLACEHOLDER + ","
                        "responsible = "  + RESPONSIBLE_ID_PLACEHOLDER + ","
                        "creationDate = " + CREATION_DATE_PLACEHOLDER  + ","
                        "deadlineDate = " + DEADLINE_DATE_PLACEHOLDER  + ","
                        "status = " + STATUS_PLACEHOLDER + ","
                        "isOverdue = "  + IS_OVERDUE_PLACEHOLDER + ","
                        "importance = " + IMPORTANCE_PLACEHOLDER + ","
                        "parentTaskId = " + PARENT_TASK_ID_PLACEHOLDER + " "
                      "WHERE idTasks = "  + TASK_ID_PLACEHOLDER + ";";
  _exchangeUnit->getReadyFor(queryText);
  
  quint32 taskId = updated.getIdTask();
  quint32 status = updated.getStatus();
  quint32 authorId = updated.getAuthorId();
  quint32 importance  = updated.getImportance();
  QString description = updated.getDescription();
  quint32 parentTaskId  = updated.getParent();
  quint32 responsibleId = updated.getResponsibleId();
  QDate creationDate = updated.getCreationDate();
  QDate deadlineDate = updated.getDeadlineDate();
  bool  isOverdue = updated.getIsOverdue();
  
  _exchangeUnit->loadPlaceholder(TASK_ID_PLACEHOLDER, taskId);
  _exchangeUnit->loadPlaceholder(STATUS_PLACEHOLDER, status);
  _exchangeUnit->loadPlaceholder(AUTHOR_ID_PLACEHOLDER, authorId);
  _exchangeUnit->loadPlaceholder(IS_OVERDUE_PLACEHOLDER, isOverdue);
  _exchangeUnit->loadPlaceholder(IMPORTANCE_PLACEHOLDER, importance);
  _exchangeUnit->loadPlaceholder(DESCRIPTION_PLACEHOLDER, description);
  _exchangeUnit->loadPlaceholder(CREATION_DATE_PLACEHOLDER, creationDate);
  _exchangeUnit->loadPlaceholder(DEADLINE_DATE_PLACEHOLDER, deadlineDate);
  _exchangeUnit->loadPlaceholder(PARENT_TASK_ID_PLACEHOLDER, parentTaskId);
  _exchangeUnit->loadPlaceholder(RESPONSIBLE_ID_PLACEHOLDER, responsibleId);
  
  _exchangeUnit->executeQuery();
  return;
}

void SqlHardWorkerUnit::deleteRelatedDataForTask(const Task& updated) const
{
  quint32 taskId = updated.getIdTask();
  
  deleteAccomplicesForTask(taskId);
  deleteCommentForTask(taskId);
  deleteWarningDateForTask(taskId);
  deleteOrdersForTask(taskId);
  deleteProjectsForTask(taskId);
  deleteFilesForTask(taskId);
  deleteReportForTask(taskId);
  return;
}

void SqlHardWorkerUnit::deleteAccomplicesForTask(const quint32 taskId) const
{
  QString taskIdAsText = transformInText(taskId);
  QString queryText = "DELETE FROM TrackerDB.relationshipsTaskAccomplices "
                      "WHERE idTask = " + taskIdAsText + ";";
  _exchangeUnit->getReadyFor(queryText);
  _exchangeUnit->executeQuery();
  return;
}

void SqlHardWorkerUnit::deleteCommentForTask(const quint32 taskId) const
{
  QString taskIdAsText = transformInText(taskId);
  QString queryText = "DELETE FROM TrackerDB.comments "
                      "WHERE idTasks = " + taskIdAsText + ";";
  _exchangeUnit->getReadyFor(queryText);
  _exchangeUnit->executeQuery();
  return;
}

void SqlHardWorkerUnit::deleteWarningDateForTask(const quint32 taskId) const
{
  QString taskIdAsText = transformInText(taskId);
  QString queryText = "DELETE FROM TrackerDB.warningDates "
                      "WHERE idTask = " + taskIdAsText + ";";
  _exchangeUnit->getReadyFor(queryText);
  _exchangeUnit->executeQuery();
  return;
}

void SqlHardWorkerUnit::deleteOrdersForTask(const quint32 taskId) const
{
  QString taskIdAsText = transformInText(taskId);
  QString queryText = "DELETE FROM TrackerDB.relationshipsTaskOrder "
                      "WHERE idTask = " + taskIdAsText + ";";
  _exchangeUnit->getReadyFor(queryText);
  _exchangeUnit->executeQuery();
  return;
}

void SqlHardWorkerUnit::deleteProjectsForTask(const quint32 taskId) const
{
  QString taskIdAsText = transformInText(taskId);
  QString queryText = "DELETE FROM TrackerDB.relationshipsTaskProject "
                      "WHERE idTask = " + taskIdAsText + ";";
  _exchangeUnit->getReadyFor(queryText);
  _exchangeUnit->executeQuery();
  return;
}

void SqlHardWorkerUnit::deleteFilesForTask(const quint32 taskId) const
{
  QString taskIdAsText = transformInText(taskId);
  QString queryText = "DELETE FROM TrackerDB.filesTasks "
                      "WHERE idTask = " + taskIdAsText + ";";
  _exchangeUnit->getReadyFor(queryText);
  _exchangeUnit->executeQuery();
  return;
}

void SqlHardWorkerUnit::deleteReportForTask(const quint32 taskId) const
{
  QString taskIdAsText = transformInText(taskId);
  QString queryText = "DELETE FROM TrackerDB.relationshipsTasksReports "
                      "WHERE idTask = " + taskIdAsText + ";";
  _exchangeUnit->getReadyFor(queryText);
  _exchangeUnit->executeQuery();
  return;
}

void SqlHardWorkerUnit::tryToUpdateTaskDataOnFtpServer(const Task& updated) const
{
  try
  {
    updateTaskFilesOnFtpServer(updated);
  }
  catch(...)
  {
    _exchangeUnit->rollbackTransaction();
    removeFtpFilesAfterUpdatingTask(updated);
    qCritical()<<"catched exception while updating task files to ftp-server";
    throw;
  }
  return;
}

void SqlHardWorkerUnit::updateTaskFilesOnFtpServer(const Task& task) const
{
  quint32 taskId = task.getIdTask();
  QString taskIdAsText = transformInText(taskId);
  QString pathInServer  = "/Tasks/id_" + taskIdAsText + "/";
  QString renamedFolder = "/Tasks/id_" + taskIdAsText + "_tmp/";
  QStringList files = task.getFilesTask();
  bool isNeedRemoveFiles = files.isEmpty();
  
  if(isNeedRemoveFiles)
  {
    removeFolderFromFtpServer(pathInServer);
  }
  renameFolderOnFtpServer(pathInServer, renamedFolder);
  uploadFilesToFtpServer(files, pathInServer);
  removeFolderFromFtpServer(renamedFolder);
  return;
}

void SqlHardWorkerUnit::removeFtpFilesAfterUpdatingTask(const Task& task) const
{
  FtpHardWorker* ftpHardWorker = General::getFtpHardWorker();
  
  quint32 taskId = task.getIdTask();
  QString taskIdAsText  = transformInText(taskId);
  QString pathInServer  = "/Tasks/id_" + taskIdAsText + "/";
  QString renamedFolder = "/Tasks/id_" + taskIdAsText + "_tmp/";
  bool isRenamedExist = ftpHardWorker->existFtpFolder(renamedFolder);
  
  if(isRenamedExist)
  {
    removeFolderFromFtpServer(pathInServer);
    renameFolderOnFtpServer(renamedFolder, pathInServer);
  }
  return;
}

void SqlHardWorkerUnit::updateReport(const Report& updated) const
{
  synchronizeDateTime();
  
  if( !(updated.isValid()) )
  {
    QString errorText = "invalid argument: #updated";
    qCritical()<<errorText;
    throw NeedFixCode(errorText);
  }

  _exchangeUnit->beginTransaction();
  tryToUpdateReportDataOnSqlServer(updated);
  tryToUpdateReportDataOnFtpServer(updated);
  _exchangeUnit->commitTransaction();
  return;
}

void SqlHardWorkerUnit::tryToUpdateReportDataOnSqlServer(const Report& report) const
{
  try
  {
    quint32 reportId = report.getId();
    updateReportData(report);
    deleteFilesForReport(reportId);
    addFilesIfExist(report);
  }
  catch(...)
  {
    _exchangeUnit->rollbackTransaction();
    qCritical()<<"catched exception while updating report data on sql-server";
    throw;
  }
  return;
}

void SqlHardWorkerUnit::updateReportData(const Report& report) const
{
  const QString TEXT_PLACEHOLDER = ":text_placeholder";
  const QString CREATION_DATE_PLACEHOLDER = ":creationDate_placeholder";
  const QString REPORT_ID_PLACEHOLDER = ":reportId_placeholder";
  QString queryText = "UPDATE TrackerDB.reports "
                      "SET "
                        "text = " + TEXT_PLACEHOLDER + ","
                        "creationDate = " + CREATION_DATE_PLACEHOLDER + " "
                      "WHERE idReport = " + REPORT_ID_PLACEHOLDER + ";";
  _exchangeUnit->getReadyFor(queryText);
  
  quint32 reportId = report.getId();
  QString text = report.getText();
  QDate creationDate = report.getCreationDate();
  
  _exchangeUnit->loadPlaceholder(TEXT_PLACEHOLDER, text);
  _exchangeUnit->loadPlaceholder(CREATION_DATE_PLACEHOLDER, creationDate);
  _exchangeUnit->loadPlaceholder(REPORT_ID_PLACEHOLDER, reportId);
  
  _exchangeUnit->executeQuery();
  return;
}

void SqlHardWorkerUnit::deleteFilesForReport(const quint32 reportId) const
{
  QString reportIdAsText = transformInText(reportId);
  QString queryText = "DELETE FROM TrackerDB.filesReports "
                      "WHERE idReport = " + reportIdAsText + ";";
  _exchangeUnit->getReadyFor(queryText);
  _exchangeUnit->executeQuery();
  return;
}

void SqlHardWorkerUnit::tryToUpdateReportDataOnFtpServer(const Report& report) const
{
  try
  {
    updateReportFilesOnFtpServer(report);
  }
  catch(...)
  {
    _exchangeUnit->rollbackTransaction();
    removeFtpFilesAfterUpdatingReport(report);
    qCritical()<<"catched exception while updating report files to ftp-server";
    throw;
  }
  return;
}

void SqlHardWorkerUnit::updateReportFilesOnFtpServer(const Report& report) const
{
  quint32 reportId = report.getId();
  QString reportIdAsText = transformInText(reportId);
  QString pathInServer  = "/Reports/id_" + reportIdAsText + "/";
  QString renamedFolder = "/Reports/id_" + reportIdAsText + "_tmp/";
  QStringList files = report.getFiles();
  
  renameFolderOnFtpServer(pathInServer, renamedFolder);
  uploadFilesToFtpServer(files, pathInServer);
  removeFolderFromFtpServer(renamedFolder);
  return;
}

void SqlHardWorkerUnit::removeFtpFilesAfterUpdatingReport(const Report& report) const
{
  FtpHardWorker* ftpHardWorker = General::getFtpHardWorker();
  
  quint32 reportId = report.getId();
  QString reportIdAsText = transformInText(reportId);
  QString pathInServer  = "/Reports/id_" + reportIdAsText + "/";
  QString renamedFolder = "/Reports/id_" + reportIdAsText + "_tmp/";
  bool isExistRenamed = ftpHardWorker->existFtpFolder(renamedFolder);
  
  if(isExistRenamed)
  {
    removeFolderFromFtpServer(pathInServer);
    renameFolderOnFtpServer(renamedFolder, pathInServer);
  }
  return;
}

void SqlHardWorkerUnit::synchronizeDateTime() const
{
  QList<SqlRecord> queryResult = executeGetServerDateTimeQuery();
  QDateTime receivedTime = extractServerDateTimeFromQueryResult(queryResult);
  
  Clock* systemClock = General::getSystemClock();
  systemClock->setTime(receivedTime);
  return;
}

QList<SqlRecord> SqlHardWorkerUnit::executeGetServerDateTimeQuery() const
{
  _exchangeUnit->getReadyFor("SELECT SYSDATE();");
  QList<SqlRecord> queryResult = _exchangeUnit->executeQuery();
  return queryResult;
}

QDateTime SqlHardWorkerUnit::extractServerDateTimeFromQueryResult(QList<SqlRecord>& queryResult) const
{
  checkForTheOnlyRecordInQueryResult(queryResult);
  
  SqlRecord receivedRecord = queryResult.takeFirst();
  QDateTime receivedTime = extractServerDateTimeFromRecord(receivedRecord);
  return receivedTime;
}

QDateTime SqlHardWorkerUnit::extractServerDateTimeFromRecord(const SqlRecord& record) const
{
  checkForTheOnlyFieldInRecord(record);
  
  QDateTime receivedTime = record.first().toDateTime();
  bool isOk = receivedTime.isValid();
  if( isOk == false )
  {
    QString errorText = "(strange shit) error converting from QVariant";
    qCritical()<<errorText;
    throw CriticalOperationException(errorText);
  }
  
  return receivedTime;
}

void SqlHardWorkerUnit::disconnect() const
{
  _exchangeUnit->disconnect();
  return;
}

QList<SqlRecord> SqlHardWorkerUnit::executeSelectQuery(const QString& selectQuery) const
{
  if( selectQuery.isEmpty() )
  {
    QString errorText = "invalid argument: #selectQuery is empty string";
    qCritical()<<errorText;
    throw NeedFixCode(errorText);
  }
  
  _exchangeUnit->getReadyFor(selectQuery);
  QList<SqlRecord> queryResult = _exchangeUnit->executeQuery();
  
  if( queryResult.isEmpty() )
  {
    throw ExecutionAborted("there is no information in database for specified query");
  }
  return queryResult;
}

void SqlHardWorkerUnit::checkForTheOnlyRecordInQueryResult(const QList<SqlRecord>& queryResult) const
{
  const quint32 REQUIRED_RECORDS_QUANTITY = 1;
  quint32 receivedRecordsQuantity = queryResult.size();
  if(receivedRecordsQuantity != REQUIRED_RECORDS_QUANTITY)
  {
    QString errorText = "received wrong quantity of records in query result (must be 1)";
    qCritical()<<errorText;
    throw NeedFixCode(errorText);
  }
  return;
}

void SqlHardWorkerUnit::checkForTheOnlyFieldInRecord(const SqlRecord& record) const
{
  quint32 fieldsQuantityInRecord = record.size();
  const quint32 REQUIRED_FIELDS_QUANTITY = 1;
  if(fieldsQuantityInRecord != REQUIRED_FIELDS_QUANTITY)
  {
    QString errorText = "received wrong quantity of fields in query result (must be 1)";
    qCritical()<<errorText;
    throw NeedFixCode(errorText);
  }
  return;
}

QString SqlHardWorkerUnit::transformInText(quint32 value) const
{
  return QString::number( (int)value );
}

//[трансформировать QList<quint32>: [1],[2],[3],[4],[5] в QString: "1,2,3,4,5" ]
QString SqlHardWorkerUnit::transformListOfIdsInText(const QList<quint32>& ids) const
{
  quint32 currentId = 0;
  QString currentIdAsText;
  QString resultText;
  quint32 idsQuantity = ids.size();
  const quint32 LAST_INDEX = idsQuantity - 1;
  for(quint32 index=0; index<idsQuantity; index++)
  {
    currentId = ids.at(index);
    currentIdAsText = transformInText(currentId);
    resultText += currentIdAsText;
    if(index < LAST_INDEX)
    {
      resultText += ",";
    }
  }
  return resultText;
}

void SqlHardWorkerUnit::removeFolderFromFtpServer(const QString& path) const
{
  FtpHardWorker* ftpHardWorker = General::getFtpHardWorker();
  ftpHardWorker->removeFtpFolder(path);
  return;
}

void SqlHardWorkerUnit::renameFolderOnFtpServer(const QString& oldName, const QString& newName) const
{
  FtpHardWorker* ftpHardWorker = General::getFtpHardWorker();
  
  bool isFolderExist = ftpHardWorker->existFtpFolder(oldName);
  if(isFolderExist)
  {
    ftpHardWorker->rename(oldName, newName);
  }
  return;
}

void SqlHardWorkerUnit::uploadFilesToFtpServer(const QStringList files, const QString& destinationPath) const
{
  FtpHardWorker* ftpHardWorker = General::getFtpHardWorker();
  
  foreach(QString file, files)
  {
    ftpHardWorker->uploadFile(file, destinationPath);
  }
  return;
}


#if TURN_ON_UNIT_TESTS
//инструменты_для_заполнения_базы_тестовыми_данными__________________________
/////////////////////////////////////////////////////////////////////////////
void SqlHardWorkerUnit::addWorker(Worker &newWorker) const
{
  if( !(newWorker.isValid()) )
  {
    QString errorText = "invalid argument: #newWorker";
    qCritical()<<errorText;
    throw NeedFixCode(errorText);
  }
  
  _exchangeUnit->beginTransaction();
  tryToAddWorkerDataToSqlServer(newWorker);
  _exchangeUnit->commitTransaction();
  return;
}

void SqlHardWorkerUnit::tryToAddWorkerDataToSqlServer(Worker& newWorker) const
{
  try
  {
    addWorkerData(newWorker);
    quint32 workerId = defineIdOfJustAddedWorker();
    newWorker.setId(workerId);
    addViceIfExist(newWorker);
  }
  catch(...)
  {
    _exchangeUnit->rollbackTransaction();
    qCritical()<<"error: catched exception while adding new worker";
    throw;
  }
  return;
}

void SqlHardWorkerUnit::addWorkerData(const Worker& worker) const
{
  QString name  = worker.getName();
  QString email = worker.getEmail();
  QString login = worker.getLogin();
  QString pass  = worker.getPass();
  QString position = worker.getPosition();
  QString surname  = worker.getSurname();
  QString patronymic = worker.getPatronymic();
  bool isGod = worker.isGod();
  QString isGodAsText = transformInText(isGod);
  
  QString queryText = "INSERT INTO TrackerDB.workers ("
                        "name, "
                        "surname, "
                        "patronymic, "
                        "position, "
                        "email, "
                        "login, "
                        "pass, "
                        "isGod) "
                      "VALUES('"
                      + name       
              + "','" + surname 
              + "','" + patronymic
              + "','" + position
              + "','" + email
              + "','" + login
              + "','" + pass
              + "',"  + isGodAsText + ");";
  _exchangeUnit->getReadyFor(queryText);
  _exchangeUnit->executeQuery();
  return;
}

quint32 SqlHardWorkerUnit::defineIdOfJustAddedWorker() const
{
  QList<SqlRecord> queryResult = executeGetMaxWorkerIdQuery();
  quint32 maxId = extractMaxIdFromQueryResult(queryResult);
  return  maxId;
}

QList<SqlRecord> SqlHardWorkerUnit::executeGetMaxWorkerIdQuery() const
{
  QString queryText = "SELECT MAX(idWorker)"
                      "FROM TrackerDB.workers";
  _exchangeUnit->getReadyFor(queryText);
  QList<SqlRecord> queryResult = _exchangeUnit->executeQuery();
  return queryResult;
}

void SqlHardWorkerUnit::addViceIfExist(const Worker& worker) const
{
  quint32 workerId = worker.getId();
  quint32 viceId = worker.getVice();
  if(viceId != 0)
  {
    addVice(workerId, viceId);
  }
  return;
}

void SqlHardWorkerUnit::addVice(const quint32 workerId, const quint32 viceId) const
{
  if( (workerId == 0)||(viceId == 0) )
  {
    QString errorText = "invalid arguments";
    qCritical()<<errorText;
    throw NeedFixCode(errorText);
  }
  
  QString workerIdAsText = transformInText(workerId);
  QString viceIdAsText = transformInText(viceId);
  QString queryText = "INSERT INTO TrackerDB.vices ("
                        "idVices,"
                        "idWorker) "
                      "VALUES ("
                       + viceIdAsText
                 + "," + workerIdAsText + ");";
  _exchangeUnit->getReadyFor(queryText);
  _exchangeUnit->executeQuery();
  return;
}

void SqlHardWorkerUnit::addSlave(const quint32 masterId, const quint32 slaveId) const
{
  if( (masterId == 0)||(slaveId == 0) )
  {
    QString errorText = "invalid arguments";
    qCritical()<<errorText;
    throw NeedFixCode(errorText);
  }
  
  QString masterIdAsText = transformInText(masterId);
  QString slaveIdAsText  = transformInText(slaveId);
  QString queryText = "INSERT INTO TrackerDB.relationshipsMasterSlave ("
                        "idMaster,"
                        "idSlave) "
                      "VALUES ("
                       + masterIdAsText
                 + "," + slaveIdAsText + ");";
  _exchangeUnit->getReadyFor(queryText);
  _exchangeUnit->executeQuery();
  return;
}

void SqlHardWorkerUnit::addOrder(const Order& order) const
{
  if( !(order.isValid()) )
  {
    QString errorText = "invalid argument: #order";
    qCritical()<<errorText;
    throw NeedFixCode(errorText);
  }
  
  QString description = order.getName();
  QString queryText = "INSERT INTO TrackerDB.orders (description)"
                      "VALUES('" + description + "');";
  _exchangeUnit->getReadyFor(queryText);
  _exchangeUnit->executeQuery();
  return;
}

void SqlHardWorkerUnit::addProject(const Project& project) const
{
  if( !(project.isValid()) )
  {
    QString errorText = "invalid argument: #project";
    qCritical()<<errorText;
    throw NeedFixCode(errorText);
  }
  
  QString description = project.getName();
  QString queryText = "INSERT INTO TrackerDB.projects (description)"
                      "VALUES('" + description + "');";
  _exchangeUnit->getReadyFor(queryText);
  _exchangeUnit->executeQuery();
  return;
}

void SqlHardWorkerUnit::dropDatabase() const
{
  _exchangeUnit->getReadyFor("DROP DATABASE TrackerDB;");
  _exchangeUnit->executeQuery();
  qDebug()<<"database dropped";
  return;
}

void SqlHardWorkerUnit::createDatabase() const
{
  _exchangeUnit->getReadyFor("CREATE SCHEMA IF NOT EXISTS TrackerDB DEFAULT CHARACTER SET cp1251;");
  _exchangeUnit->executeQuery();
  qDebug()<<"database created. database is empty";
  return;
}

void SqlHardWorkerUnit::createTables() const
{
  createWorkersTable();
  createTasksTable();
  createCommentsTable();
  createWarningDatesTable();
  createRelationshipsTaskAccomplicesTable();
  createProjectsTable();
  createRelationshipsTaskProjectTable();
  createOrdersTable();
  createRelationshipsTaskOrderTable();
  createRelationshipsMasterSlaveTable();
  createFilesTasksTable();
  createReportsTable();
  createFilesReportsTable();
  createRelationshipsTasksReportsTable();
  createVicesTable();
  //[создаём таблицу наследования поручений]
  //createRelationshipsTasksTable();
  //[создаём таблицу для хранения изменений в поручениях]
  //createChangesTable();
  //[создаём таблицу связи изменения и соисполнителей(состояние до момента изменения)]
  //createRelationshipsChangesAccomplicesOldStateTable();
  //[создаём таблицу связи изменения и соисполнителей(состояние после изменения)]
  //createRelationshipsChangesAccomplicesNewStateTable();
  //[создаём таблицу связи изменения и файлов вложения поручения(состояние до изменения)]
  //createRelationshipsChangesFilesTasksOldStateTable();
  //[создаём таблицу связи изменения и файлов вложения поручения(состояние после изменения)]
  //createRelationshipsChangesFilesTasksNewStateTable();
  //[создаём таблицу связи изменения и файлов вложения отчета(состояние до изменения)]
  //createRelationshipsChangesFilesOrdersOldStateTable();
  //[создаём таблицу связи изменения и файлов вложения отчета(состояние после изменения)]
  //createRelationshipsChangesFilesOrdersNewStateTable();
  //[создаём таблицу связи изменения и проектов(состояние до изменения)]
  //createRelationshipsChangesProjectOldStateTable();
  //[создаём таблицу связи изменения и проектов(состояние после изменения)]
  //createRelationshipsChangesProjectNewStateTable();
  //[создаём таблицу связи изменения и заказов(состояние до изменения)]
  //createRelationshipsChangesOrderOldStateTable();
  //[создаём таблицу связи изменения и заказов(состояние после изменения)]
  //createRelationshipsChangesOrderNewStateTable();
  //[создаём таблицу связи изменения и измененных данных]
  //createChangedDataTable();
  qDebug()<<"database tables are created. tables are empty.";
  return;
}

//длинная_партянка_медодов_создания_всех_таблиц_базы___________________________
///////////////////////////////////////////////////////////////////////////////
void SqlHardWorkerUnit::createWorkersTable() const
{
  createDBTable("CREATE TABLE IF NOT EXISTS TrackerDB.workers ("
                "idWorker INT(11) NOT NULL AUTO_INCREMENT,"
                "name TINYTEXT NOT NULL,"
                "surname TINYTEXT NOT NULL,"
                "patronymic TINYTEXT NOT NULL,"
                "position TINYTEXT NOT NULL,"
                "email TINYTEXT NOT NULL,"
                "login VARCHAR(256) NOT NULL,"
                "pass VARCHAR(256) NOT NULL,"
                "isGod TINYINT(1) NOT NULL,"
                "PRIMARY KEY (idWorker),"
                "UNIQUE INDEX login_UNIQUE (login ASC)) "
                "ENGINE = InnoDB "
                "DEFAULT CHARACTER SET = cp1251; ");
  return;
}

void SqlHardWorkerUnit::createTasksTable() const
{
  createDBTable("CREATE TABLE IF NOT EXISTS TrackerDB.Tasks ("
                "idTasks INT(11) NOT NULL AUTO_INCREMENT,"
                "description MEDIUMTEXT NOT NULL,"
                "author INT(11) NOT NULL,"
                "responsible INT(11) NOT NULL,"
                "creationDate DATE NOT NULL,"
                "deadlineDate DATE NOT NULL,"
                "status INT(11) NOT NULL,"
                "isOverdue TINYINT(1) NOT NULL,"
                "importance INT(11) NOT NULL,"
                "parentTaskId INT(11) NOT NULL,"
                "PRIMARY KEY (idTasks),"
                "INDEX authorFK_idx (author ASC),"
                "FOREIGN KEY (author) "
                "REFERENCES TrackerDB.workers (idWorker)"
                "ON DELETE RESTRICT "
                "ON UPDATE RESTRICT,"
                "FOREIGN KEY (responsible) "
                "REFERENCES TrackerDB.workers (idWorker)"
                "ON DELETE RESTRICT "
                "ON UPDATE RESTRICT)"
                "ENGINE = InnoDB "
                "DEFAULT CHARACTER SET = cp1251;");
  return;
}

void SqlHardWorkerUnit::createCommentsTable() const
{
  createDBTable("CREATE TABLE IF NOT EXISTS TrackerDB.comments ("
                "idComment INT(11) NOT NULL AUTO_INCREMENT,"
                "Comment MEDIUMTEXT NOT NULL,"
                "idTasks INT(11) NOT NULL,"
                "PRIMARY KEY (idComment),"
                "INDEX idTasksFK_idx (idTasks ASC),"
                "UNIQUE INDEX idTasks_UNIQUE (idTasks ASC),"
                "FOREIGN KEY (idTasks) "
                "REFERENCES TrackerDB.Tasks(idTasks)"
                "ON DELETE RESTRICT "
                "ON UPDATE RESTRICT) "
                "ENGINE = InnoDB "
                "DEFAULT CHARACTER SET = cp1251;");
  return;
}

void SqlHardWorkerUnit::createWarningDatesTable() const
{
  createDBTable("CREATE TABLE IF NOT EXISTS TrackerDB.warningDates ("
                "idDate INT(11) NOT NULL AUTO_INCREMENT,"
                "date DATE NOT NULL,"
                "idTask INT(11) NOT NULL,"
                "PRIMARY KEY (idDate),"
                "UNIQUE INDEX idTask_UNIQUE (idTask ASC),"
                "FOREIGN KEY (idTask)"
                "REFERENCES TrackerDB.Tasks (idTasks)"
                "ON DELETE RESTRICT "
                "ON UPDATE RESTRICT) "
                "ENGINE = InnoDB "
                "DEFAULT CHARACTER SET = cp1251;");
  return;
}

void SqlHardWorkerUnit::createRelationshipsTasksTable() const
{
  createDBTable("CREATE TABLE IF NOT EXISTS TrackerDB.relationshipsTasks ("
                "idCurrentTask INT(11) NOT NULL,"
                "idParentTask INT(11) NOT NULL,"
                "UNIQUE INDEX idCurrentTask_UNIQUE (idCurrentTask ASC),"
                "UNIQUE INDEX idParentTask_UNIQUE (idParentTask ASC),"
                "FOREIGN KEY (idCurrentTask) "
                "REFERENCES TrackerDB.Tasks (idTasks) "
                "ON DELETE RESTRICT "
                "ON UPDATE RESTRICT,"
                "FOREIGN KEY (idParentTask)"
                "REFERENCES TrackerDB.Tasks (idTasks)"
                "ON DELETE RESTRICT "
                "ON UPDATE RESTRICT)"
                "ENGINE = InnoDB "
                "DEFAULT CHARACTER SET = cp1251");
  return;
}

void SqlHardWorkerUnit::createRelationshipsTaskAccomplicesTable() const
{
  createDBTable("CREATE TABLE IF NOT EXISTS TrackerDB.relationshipsTaskAccomplices ("
                "idTask INT(11) NOT NULL,"
                "idWorker INT(11) NOT NULL,"
                "INDEX idTaskFK_idx (idTask ASC),"
                "INDEX idWorkerFK_idx (idWorker ASC),"
                "FOREIGN KEY (idTask)"
                "REFERENCES TrackerDB.Tasks (idTasks)"
                "ON DELETE RESTRICT "
                "ON UPDATE RESTRICT,"
                "FOREIGN KEY (idWorker) "
                "REFERENCES TrackerDB.workers (idWorker)"
                "ON DELETE RESTRICT "
                "ON UPDATE RESTRICT)"
                "ENGINE = InnoDB "
                "DEFAULT CHARACTER SET = cp1251;");
  return;
}

void SqlHardWorkerUnit::createProjectsTable() const
{
  createDBTable("CREATE TABLE IF NOT EXISTS TrackerDB.projects ("
                "idProject INT(11) NOT NULL AUTO_INCREMENT,"
                "description TEXT(65535) NOT NULL,"
                "PRIMARY KEY (idProject))"
                "ENGINE = InnoDB "
                "DEFAULT CHARACTER SET = cp1251;");
  return;
}

void SqlHardWorkerUnit::createRelationshipsTaskProjectTable() const
{
  createDBTable("CREATE TABLE IF NOT EXISTS TrackerDB.relationshipsTaskProject ("
                "idTask INT(11) NOT NULL,"
                "idProject INT(11) NOT NULL,"
                "INDEX IdTaskFK_idx (idTask ASC),"
                "INDEX idProjectFK_idx (idProject ASC),"
                "FOREIGN KEY (idTask)"
                "REFERENCES TrackerDB.Tasks (idTasks)"
                "ON DELETE RESTRICT "
                "ON UPDATE RESTRICT, "
                "FOREIGN KEY (idProject)"
                "REFERENCES TrackerDB.projects (idProject)"
                "ON DELETE RESTRICT "
                "ON UPDATE RESTRICT) "
                "ENGINE = InnoDB "
                "DEFAULT CHARACTER SET = cp1251;");
  return;
}

void SqlHardWorkerUnit::createOrdersTable() const
{
  createDBTable("CREATE TABLE IF NOT EXISTS TrackerDB.orders ("
                "idOrder INT(11) NOT NULL AUTO_INCREMENT,"
                "description TEXT(65535) NOT NULL,"
                "PRIMARY KEY (idOrder))"
                "ENGINE = InnoDB "
                "DEFAULT CHARACTER SET = cp1251;");
  return;
}

void SqlHardWorkerUnit::createRelationshipsTaskOrderTable() const
{
  createDBTable("CREATE TABLE IF NOT EXISTS TrackerDB.relationshipsTaskOrder ("
                "idTask INT(11) NOT NULL,"
                "idOrder INT(11) NULL DEFAULT NULL,"
                "INDEX idTaskFK_idx (idTask ASC),"
                "INDEX idOrderFK_idx (idOrder ASC),"
                "FOREIGN KEY (idTask)"
                "REFERENCES TrackerDB.Tasks (idTasks)"
                "ON DELETE RESTRICT "
                "ON UPDATE RESTRICT,"
                "FOREIGN KEY (idOrder)"
                "REFERENCES TrackerDB.orders (idOrder)"
                "ON DELETE RESTRICT "
                "ON UPDATE RESTRICT)"
                "ENGINE = InnoDB "
                "DEFAULT CHARACTER SET = cp1251;");
  return;
}

void SqlHardWorkerUnit::createRelationshipsMasterSlaveTable() const
{
  createDBTable("CREATE TABLE IF NOT EXISTS TrackerDB.relationshipsMasterSlave ("
                "idMaster INT(11) NOT NULL,"
                "idSlave INT(11) NOT NULL,"
                "INDEX idMasterFK_idx (idMaster ASC),"
                "INDEX idSlaveFK_idx (idSlave ASC),"
                "FOREIGN KEY (idMaster)"
                "REFERENCES TrackerDB.workers (idWorker)"
                "ON DELETE RESTRICT "
                "ON UPDATE RESTRICT,"
                "FOREIGN KEY (idSlave)"
                "REFERENCES TrackerDB.workers (idWorker)"
                "ON DELETE RESTRICT "
                "ON UPDATE RESTRICT)"
                "ENGINE = InnoDB "
                "DEFAULT CHARACTER SET = cp1251;");
  return;
}

void SqlHardWorkerUnit::createFilesTasksTable() const
{
  createDBTable("CREATE TABLE IF NOT EXISTS TrackerDB.filesTasks ("
                "idFile INT(11) NOT NULL AUTO_INCREMENT,"
                "filePath MEDIUMTEXT NULL DEFAULT NULL,"
                "idTask INT(11) NOT NULL,"
                "PRIMARY KEY (idFile),"
                "INDEX idTaskFK_idx (idTask ASC),"
                "FOREIGN KEY (idTask)"
                "REFERENCES TrackerDB.Tasks (idTasks)"
                "ON DELETE RESTRICT "
                "ON UPDATE RESTRICT)"
                "ENGINE = InnoDB "
                "DEFAULT CHARACTER SET = cp1251;");
  return;
}

void SqlHardWorkerUnit::createReportsTable() const
{
  createDBTable("CREATE TABLE IF NOT EXISTS TrackerDB.reports ("
                "idReport INT(11) NOT NULL AUTO_INCREMENT,"
                "text MEDIUMTEXT NOT NULL,"
                "creationDate DATE NOT NULL,"
                "PRIMARY KEY (idReport))"
                "ENGINE = InnoDB "
                "DEFAULT CHARACTER SET = cp1251; ");
  return;
}
void SqlHardWorkerUnit::createFilesReportsTable() const
{
  createDBTable("CREATE TABLE IF NOT EXISTS TrackerDB.filesReports ("
                "idFile INT(11) NOT NULL AUTO_INCREMENT,"
                "filePath MEDIUMTEXT NULL DEFAULT NULL,"
                "idReport INT(11) NOT NULL,"
                "PRIMARY KEY (idFile),"
                "INDEX idReportFK_idx (idReport ASC),"
                "FOREIGN KEY (idReport)"
                "REFERENCES TrackerDB.reports (idReport)"
                "ON DELETE RESTRICT "
                "ON UPDATE RESTRICT)"
                "ENGINE = InnoDB "
                "DEFAULT CHARACTER SET = cp1251;");
  return;
}

void SqlHardWorkerUnit::createRelationshipsTasksReportsTable() const
{
  createDBTable("CREATE TABLE IF NOT EXISTS TrackerDB.relationshipsTasksReports ("
                "idTask INT(11) NOT NULL,"
                "idReport INT(11) NOT NULL,"
                "FOREIGN KEY (idTask)"
                "REFERENCES TrackerDB.Tasks (idTasks)"
                "ON DELETE RESTRICT "
                "ON UPDATE RESTRICT,"
                "FOREIGN KEY (idReport)"
                "REFERENCES TrackerDB.reports (idReport)"
                "ON DELETE RESTRICT "
                "ON UPDATE RESTRICT)"
                "ENGINE = InnoDB "
                "DEFAULT CHARACTER SET = cp1251;");
  return;
}

void SqlHardWorkerUnit::createVicesTable() const
{
  createDBTable("CREATE TABLE IF NOT EXISTS TrackerDB.vices ("
                "idVices INT(11) NOT NULL,"
                "idWorker INT(11) NOT NULL,"
                "INDEX idVicesFK_idx (idVices ASC),"
                "INDEX idWorkerFK_idx (idWorker ASC),"
                "FOREIGN KEY (idVices)"
                "REFERENCES TrackerDB.workers (idWorker)"
                "ON DELETE RESTRICT "
                "ON UPDATE RESTRICT,"
                "FOREIGN KEY (idWorker)"
                "REFERENCES TrackerDB.workers (idWorker)"
                "ON DELETE RESTRICT "
                "ON UPDATE RESTRICT)"
                "ENGINE = InnoDB "
                "DEFAULT CHARACTER SET = cp1251;");
  return;
}

void SqlHardWorkerUnit::createChangesTable() const
{
  createDBTable("CREATE TABLE IF NOT EXISTS TrackerDB.change ("
                "idChange INT(11) NOT NULL AUTO_INCREMENT,"
                "idTask INT(11) NOT NULL,"
                "dateTimeChange DATETIME NOT NULL,"
                "AuthorChange INT(11) NOT NULL,"
                "rowChanged TINYTEXT NOT NULL,"
                "PRIMARY KEY (idChange),"
                "INDEX idTaskChangeFK_idx (idTask ASC),"
                "INDEX authorChangeFK_idx (AuthorChange ASC),"
                "FOREIGN KEY (idTask)"
                "REFERENCES TrackerDB.Tasks (idTasks)"
                "ON DELETE RESTRICT "
                "ON UPDATE RESTRICT,"
                "FOREIGN KEY (AuthorChange)"
                "REFERENCES TrackerDB.workers (idWorker)"
                "ON DELETE RESTRICT "
                "ON UPDATE RESTRICT)"
                "ENGINE = InnoDB "
                "DEFAULT CHARACTER SET = cp1251;");
  return;
}

void SqlHardWorkerUnit::createRelationshipsChangesAccomplicesOldStateTable() const
{
  createDBTable("CREATE TABLE IF NOT EXISTS TrackerDB.relationshipsChangesAccomplicesOldState ("
                "idChange INT(11) NOT NULL,"
                "idWorker INT(11) NOT NULL,"
                "INDEX idChangeRlChAccFK_idx (idChange ASC),"
                "INDEX idWorkerRlChAccFK_idx (idWorker ASC),"
                "FOREIGN KEY (idChange)"
                "REFERENCES TrackerDB.change (idChange)"
                "ON DELETE RESTRICT "
                "ON UPDATE RESTRICT, "
                "FOREIGN KEY (idWorker)"
                "REFERENCES TrackerDB.workers (idWorker)"
                "ON DELETE RESTRICT "
                "ON UPDATE RESTRICT)"
                "ENGINE = InnoDB "
                "DEFAULT CHARACTER SET = cp1251;");
  return;
}

void SqlHardWorkerUnit::createRelationshipsChangesAccomplicesNewStateTable() const
{
  createDBTable("CREATE TABLE IF NOT EXISTS TrackerDB.relationshipsChangesAccomplicesNewState ("
                "idChange INT(11) NOT NULL,"
                "idWorker INT(11) NOT NULL,"
                "INDEX idChangeRlChAccFK_idx (idChange ASC),"
                "INDEX idWorkerRlChAccFK_idx (idWorker ASC),"
                "FOREIGN KEY (idChange)"
                "REFERENCES TrackerDB.change (idChange)"
                "ON DELETE RESTRICT "
                "ON UPDATE RESTRICT, "
                "FOREIGN KEY (idWorker)"
                "REFERENCES TrackerDB.workers (idWorker)"
                "ON DELETE RESTRICT "
                "ON UPDATE RESTRICT)"
                "ENGINE = InnoDB "
                "DEFAULT CHARACTER SET = cp1251;");
  return;
}

void SqlHardWorkerUnit::createRelationshipsChangesFilesTasksOldStateTable() const
{
  createDBTable("CREATE TABLE IF NOT EXISTS TrackerDB.relationshipsChangesFilesTasksOldState ("
                "idChange INT(11) NOT NULL,"
                "idFileTask INT(11) NULL DEFAULT NULL,"
                "INDEX idChanggeRlFlTskOld_idx (idChange ASC),"
                "INDEX idFileTaskRlFlTskOldFK_idx (idFileTask ASC),"
                "FOREIGN KEY (idChange)"
                "REFERENCES TrackerDB.change (idChange)"
                "ON DELETE RESTRICT "
                "ON UPDATE RESTRICT, "
                "FOREIGN KEY (idFileTask)"
                "REFERENCES TrackerDB.filesTasks (idFile)"
                "ON DELETE RESTRICT "
                "ON UPDATE RESTRICT)"
                "ENGINE = InnoDB "
                "DEFAULT CHARACTER SET = cp1251;");
  return;
}

void SqlHardWorkerUnit::createRelationshipsChangesFilesTasksNewStateTable() const
{
  createDBTable("CREATE TABLE IF NOT EXISTS TrackerDB.relationshipsChangesFilesTasksNewState ("
                "idChange INT(11) NOT NULL,"
                "idFileTask INT(11) NULL DEFAULT NULL,"
                "INDEX idChanggeRlFlTskOld_idx (idChange ASC),"
                "INDEX idFileTaskRlFlTskOldFK_idx (idFileTask ASC),"
                "FOREIGN KEY (idChange)"
                "REFERENCES TrackerDB.change (idChange)"
                "ON DELETE RESTRICT "
                "ON UPDATE RESTRICT,"
                "FOREIGN KEY (idFileTask)"
                "REFERENCES TrackerDB.filesTasks (idFile)"
                "ON DELETE RESTRICT "
                "ON UPDATE RESTRICT) "
                "ENGINE = InnoDB "
                "DEFAULT CHARACTER SET = cp1251;");
  return;
}

void SqlHardWorkerUnit::createRelationshipsChangesFilesOrdersOldStateTable() const
{
  createDBTable("CREATE TABLE IF NOT EXISTS TrackerDB.relationshipsChangesFilesOrdersOldState ("
                "idChange INT(11) NOT NULL,"
                "idFileReport INT(11) NOT NULL,"
                "INDEX idChanggeRlFlTskOld_idx (idChange ASC),"
                "INDEX idFileTaskRlFlTskOldFK1_idx (idFileReport ASC),"
                "FOREIGN KEY (idChange)"
                "REFERENCES TrackerDB.change (idChange)"
                "ON DELETE RESTRICT "
                "ON UPDATE RESTRICT,"
                "FOREIGN KEY (idFileReport)"
                "REFERENCES TrackerDB.filesReports (idFile)"
                "ON DELETE RESTRICT "
                "ON UPDATE RESTRICT) "
                "ENGINE = InnoDB "
                "DEFAULT CHARACTER SET = cp1251;");
  return;
}

void SqlHardWorkerUnit::createRelationshipsChangesFilesOrdersNewStateTable() const
{
  createDBTable("CREATE TABLE IF NOT EXISTS TrackerDB.relationshipsChangesFilesOrdersNewState ("
                "idChange INT(11) NOT NULL,"
                "idFileReport INT(11) NOT NULL,"
                "INDEX idChanggeRlFlTskOld_idx (idChange ASC),"
                "INDEX idFileTaskRlFlTskOldFK1_idx (idFileReport ASC),"
                "FOREIGN KEY (idChange)"
                "REFERENCES TrackerDB.change (idChange)"
                "ON DELETE RESTRICT "
                "ON UPDATE RESTRICT,"
                "FOREIGN KEY (idFileReport)"
                "REFERENCES TrackerDB.filesReports (idFile)"
                "ON DELETE RESTRICT "
                "ON UPDATE RESTRICT)"
                "ENGINE = InnoDB "
                "DEFAULT CHARACTER SET = cp1251;");
  return;
}

void SqlHardWorkerUnit::createRelationshipsChangesProjectOldStateTable() const
{
  createDBTable("CREATE TABLE IF NOT EXISTS TrackerDB.relationshipsChangesProjectOldState ("
                "idChange INT(11) NOT NULL,"
                "idProject INT(11) NULL DEFAULT NULL,"
                "INDEX idChangeRlChPrOld_idx (idChange ASC),"
                "INDEX idProjectRlChPrOld_idx (idProject ASC),"
                "FOREIGN KEY (idChange)"
                "REFERENCES TrackerDB.change (idChange)"
                "ON DELETE RESTRICT "
                "ON UPDATE RESTRICT,"
                "FOREIGN KEY (idProject)"
                "REFERENCES TrackerDB.projects (idProject)"
                "ON DELETE RESTRICT "
                "ON UPDATE RESTRICT)"
                "ENGINE = InnoDB "
                "DEFAULT CHARACTER SET = cp1251;");
  return;
}

void SqlHardWorkerUnit::createRelationshipsChangesProjectNewStateTable() const
{
  createDBTable("CREATE TABLE IF NOT EXISTS TrackerDB.relationshipsChangesProjectNewState ("
                "idChange INT(11) NOT NULL,"
                "idProject INT(11) NULL DEFAULT NULL,"
                "INDEX idChangeRlChPrOld_idx (idChange ASC),"
                "INDEX idProjectRlChPrOld_idx (idProject ASC),"
                "FOREIGN KEY (idChange)"
                "REFERENCES TrackerDB.change (idChange)"
                "ON DELETE RESTRICT "
                "ON UPDATE RESTRICT,"
                "FOREIGN KEY (idProject)"
                "REFERENCES TrackerDB.projects (idProject)"
                "ON DELETE RESTRICT "
                "ON UPDATE RESTRICT)"
                "ENGINE = InnoDB "
                "DEFAULT CHARACTER SET = cp1251;");
  return;
}

void SqlHardWorkerUnit::createRelationshipsChangesOrderOldStateTable() const
{
  createDBTable("CREATE TABLE IF NOT EXISTS TrackerDB.relationshipsChangesOrderOldState ("
                "idChange INT(11) NOT NULL,"
                "idOrder INT(11) NOT NULL,"
                "INDEX idChangeRlChPrOld_idx (idChange ASC),"
                "INDEX idProjectRlChPrOld00_idx (idOrder ASC),"
                "FOREIGN KEY (idChange)"
                "REFERENCES TrackerDB.change (idChange)"
                "ON DELETE RESTRICT "
                "ON UPDATE RESTRICT,"
                "FOREIGN KEY (idOrder)"
                "REFERENCES TrackerDB.orders (idOrder)"
                "ON DELETE RESTRICT "
                "ON UPDATE RESTRICT)"
                "ENGINE = InnoDB "
                "DEFAULT CHARACTER SET = cp1251;");
  return;
}

void SqlHardWorkerUnit::createRelationshipsChangesOrderNewStateTable() const
{
  createDBTable("CREATE TABLE IF NOT EXISTS TrackerDB.relationshipsChangesOrderNewState ("
                "idChange INT(11) NOT NULL,"
                "idOrder INT(11) NOT NULL,"
                "INDEX idChangeRlChPrOld_idx (idChange ASC),"
                "INDEX idProjectRlChPrOld00_idx (idOrder ASC),"
                "FOREIGN KEY (idChange)"
                "REFERENCES TrackerDB.change (idChange)"
                "ON DELETE RESTRICT "
                "ON UPDATE RESTRICT,"
                "FOREIGN KEY (idOrder)"
                "REFERENCES TrackerDB.orders (idOrder)"
                "ON DELETE RESTRICT "
                "ON UPDATE RESTRICT)"
                "ENGINE = InnoDB "
                "DEFAULT CHARACTER SET = cp1251;");
  return;
}

void SqlHardWorkerUnit::createChangedDataTable() const
{
  createDBTable("CREATE TABLE IF NOT EXISTS TrackerDB.changedData ("
                "idChange INT(11) NOT NULL,"
                "oldState MEDIUMTEXT NOT NULL,"
                "newState MEDIUMTEXT NOT NULL,"
                "INDEX idChangeChDataFK_idx (idChange ASC),"
                "FOREIGN KEY (idChange)"
                "REFERENCES TrackerDB.change (idChange)"
                "ON DELETE RESTRICT "
                "ON UPDATE RESTRICT)"
                "ENGINE = InnoDB "
                "DEFAULT CHARACTER SET = cp1251;");
  return;
}

void SqlHardWorkerUnit::createDBTable(const QString& queryText) const
{
  //[создаём таблицу]
  _exchangeUnit->getReadyFor(queryText);
  _exchangeUnit->executeQuery();
  return;
}
#endif//TURN_ON_UNIT_TESTS

//секция_определения_статических_членов_класса_________________________________
///////////////////////////////////////////////////////////////////////////////
QScopedPointer<SqlHardWorkerUnit> SqlHardWorkerUnit::_thisUnit;
