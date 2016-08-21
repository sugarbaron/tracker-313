/*! \file sqlhardworkerunit.h
 *  \brief sql-модуль. реализует потребности приложения по работе с базой
 *  \author sugarbaron ([bravery_and_stupidity] e-mail: sugarbaron1@mail.ru)
 *  \date 27.06.2016
 */
#ifndef SQLHARDWORKERUNIT_H
#define SQLHARDWORKERUNIT_H

#include "sqlhardworker.h"
#include "sqldatabasecreator.h"
#include "sqlexchange.h"
#include "general.h"
#include <QScopedPointer>

/*! \brief имплементация интерфейса SqlData.
 * sql-модуль. реализует потребности приложения по работе с базой */
class  SqlHardWorkerUnit : 
public SqlHardWorker 
#if TURN_ON_UNIT_TESTS
,public SqlDatabaseCreator 
#endif
{
  //секция_данных____________________________________________________________
  ///////////////////////////////////////////////////////////////////////////
  private: static QScopedPointer<SqlHardWorkerUnit> _thisUnit;
  
  private: SqlExchange* _exchangeUnit;

  private: enum WorkerTableColumns
  {
    wtc_id = 0,
    wtc_name,
    wtc_surname,
    wtc_patronymic,
    wtc_position,
    wtc_email,
    wtc_login,
    wtc_pass,
    wtc_is_god,
    wtc_fields_quantity
  };

  private: enum TasksTableColumns
  {
    ttc_id = 0,
    ttc_description,
    ttc_author,
    ttc_responsible,
    ttc_creation_date,
    ttc_deadline_date,
    ttc_status,
    ttc_is_overdue,
    ttc_importance,
    ttc_parent_task_id,
    ttc_fields_quantity
  };

  private: enum ReportsTableColumns
  {
    rtc_report_id = 0,
    rtc_text,
    rtc_creation_date,
    rtc_fields_quantity
  };
  
  private: enum OrdersTableColumns
  {
    otc_order_id = 0,
    otc_description,
    otc_fields_quantity
  };
  
  private: enum ProjectsTableColumns
  {
    ptc_project_id = 0,
    ptc_description,
    ptc_fields_quantity
  };

  //секция_конструкторов(деструкторов)_______________________________________
  ///////////////////////////////////////////////////////////////////////////
  public:  static void create();
  private: static void checkUnitAbsence();
  private: SqlHardWorkerUnit();
  private: static SqlExchange* createExchangeUnit();
  public:  virtual ~SqlHardWorkerUnit();
  
  public: static SqlHardWorkerUnit* getInstance();
  private: static void checkUnitExistance();
  private: static bool isUnitAlreadyCreated();
  private: static bool isUnitNotCreatedYet();

  //секция_методов___________________________________________________________
  ///////////////////////////////////////////////////////////////////////////
  /*! \brief  добавить новое поручение в базу
   *  \param  task - поручение */
  public:  void addTask(Task& task) const;
  
  private: void tryToAddTaskDataToSqlServer(Task& task) const;
  private: void addTaskData(const Task& task) const;
  private: quint32 defineIdOfJustAddedTask() const;
  private: QList<SqlRecord> executeGetMaxTaskIdQuery() const;
  private: quint32 extractMaxIdFromQueryResult(QList<SqlRecord>& queryResult) const;
  private: quint32 extractMaxIdFromRecord(const SqlRecord& record) const;
  private: void addRelatedDataForTask(const Task& task) const;
  private: void addAccomplicesIfExist(const Task& task) const;
  private: void addAccomplicesForTask(const quint32 taskId, const QList<quint32>& accomplices) const;
  private: void addCommentIfExist(const Task& task) const;
  private: void addCommentForTask(const quint32 taskId, const QString& comment) const;
  private: void addWarningDateIfExist(const Task& task) const;
  private: void addWarningDateForTask(const quint32 taskId, const QDate& warningDate) const;
  private: void addOrdersIfExist(const Task& task) const;
  private: void addOrdersForTask(const quint32 taskId, const QList<quint32>& ordersIds) const;
  private: void addProjectsIfExist(const Task& task) const;
  private: void addProjectsForTask(const quint32 taskId, const QList<quint32>& projectsIds) const;
  private: void addFilesIfExist(const Task& task) const;
  private: void addFilesForTask(const quint32 taskId, const QStringList& files) const;
  private: void addReportIfExist(const Task& task) const;
  private: void addReportForTask(const quint32 taskId, const quint32 reportId) const;
  private: void tryToAddTaskDataToFtpServer(const Task& task) const;
  private: void addTaskFilesToFtpServer(const Task& task) const;
  private: void removeFtpFilesAfterAddingTask(const Task& task) const;

  /*! \brief добавить отчёт по указанному поручению
   *  \param report  добавляемый отчёт
   *  \param taskId  айди поручения, по которому добавляется отчет */
  public:  void addReport(Report& report, const quint32 taskId) const;
  
  private: void tryToAddReportDataToSqlServer(Report& report, const quint32 taskId) const;
  private: void addReportData(const Report& report) const;
  private: quint32 defineIdOfJustAddedReport() const;
  private: QList<SqlRecord> executeGetMaxReportIdQuery() const;
  private: void addFilesIfExist(const Report& report) const;
  private: void addFilesForReport(const quint32 reportId, const QStringList& files) const;
  private: void tryToAddReportDataToFtpServer(const Report& report) const;
  private: void loadReportFilesToFtpServer(const Report& report) const;
  private: void removeFtpFilesAfterAddingReport(const Report& report) const;
  
  /*! \brief  получить из базы поручение по его айди
   *  \param  taskId - айди поручения
   *  \return поручение */
  public:  Task getTask(const qint32 taskId) const;
  
  /*! \brief  получить из базы список поручений, в которых автором является указанный воркер
   *  \param  authorId - айди автора задач
   *  \return список поручений */
  public:  QList<Task> getTasksRequestedBy(const quint32 authorId) const;
  
  /*! \brief  получить из базы список всех поручений, в которых участвует указанный воркер
   *  \param  workerId - айди воркера 
   *  \return список поручений */
  public:  QList<Task> getAllWorkerTasks(const quint32 workerId) const;
  
  /*! \brief  получить из базы список поручений, порученных указанному воркеру
   *  \param  responsibleId - айди воркера
   *  \return список поручений */
  public:  QList<Task> getTasksForResponsible(const quint32 responsibleId) const;
  
  /*! \brief  получить из базы список выполненных и снятых поручений для указанного воркера
   *  \param  workerId - айди воркера
   *  \return список поручений */
  public:  QList<Task> getCompleteTasksForWorker(const quint32 workerId) const;
  
  private: QList<Task> getTasksByCondition(const QString& condition) const;
  private: QList<Task> extractTasksDataFromQueryResultAndRelatedTables(QList<SqlRecord>& queryResult) const;
  private: Task extractTaskDataFromRecord(const SqlRecord& record) const;
  private: void fillTaskWithAccomplices(Task& task) const;
  private: QList<quint32> getAccomplicesForTask(const quint32 taskId) const;
  private: QList<quint32> extractAccomplicesFromQueryResult(QList<SqlRecord>& queryResult) const;
  private: quint32 extractAccomliceFromRecord(const SqlRecord& record) const;
  private: void fillTaskWithComment(Task& task) const;
  private: QString getCommentForTask(const quint32 taskId) const;
  private: QString extractCommentFromQueryResult(QList<SqlRecord>& queryResult) const;
  private: QString extractCommentFromRecord(const SqlRecord& record) const;
  private: void fillTaskWithWarningDate(Task& task) const;
  private: QDate getWarningDateForTask(const quint32 taskId) const;
  private: QDate extractWarningDateFromQueryResult(QList<SqlRecord>& queryResult) const;
  private: QDate extractWarningDateFromRecord(const SqlRecord& record) const;
  private: void fillTaskWithOrders(Task& task) const;
  private: QList<quint32> getOrdersForTask(const quint32 taskId) const;
  private: QList<quint32> extractOrdersFromQueryResult(QList<SqlRecord>& queryResult) const;
  private: quint32 extractOrderFromRecord(const SqlRecord& record) const;
  private: void fillTaskWithProjects(Task& task) const;
  private: QList<quint32> getProjectsForTask(const quint32 taskId) const;
  private: QList<quint32> extractProjectsFromQueryResult(QList<SqlRecord>& queryResult) const;
  private: quint32 extractProjectFromRecord(const SqlRecord& record) const;
  private: void fillTaskWithFiles(Task& task) const;
  private: QStringList getFilesForTask(const quint32 taskId) const;
  private: QStringList extractFilesFromQueryResult(QList<SqlRecord>& queryResult) const;
  private: QString extractFileFromRecord(const SqlRecord& record) const;
  private: void fillTaskWithReport(Task& task) const;
  private: quint32 getReportForTask(const quint32 taskId) const;
  private: quint32 extractReportFromQueryResult(QList<SqlRecord>& queryResult) const;
  private: quint32 extractReportFromRecord(const SqlRecord& record) const;
  
  /*! \brief  получить из базы воркера по его логину
   *  \param  login - логин пользователя
   *  \return воркер */
  public:  Worker getWorker(const QString& login) const;
  
  /*! \brief  получить из базы воркера по его айди
   *  \param  workerId - айди
   *  \return воркер */
  public:  Worker getWorker(const quint32 workerId) const;
  
  /*! \brief  получить из базы список подчинённых для воркера, указанного параметром
   *  \param  masterId - айди начальника
   *  \return список подчинённых */
  public:  QList<Worker> getListOfResponsibleForWorker(const quint32 masterId) const;
  
  /*! \brief  получить из базы список воркеров по списку их айди
   *  \param  workersIds - список айди воркеров
   *  \return список воркеров */
  public:  QList<Worker> getWorkers(const QList<quint32>& workersIds) const;
  
  private: QList<Worker> getWorkersByCondition(const QString& condition) const;
  private: QList<Worker> extractWorkersDataFromQueryResultAndRelatedTables(QList<SqlRecord>& queryResult) const;
  private: Worker extractWorkerDataFromRecord(const SqlRecord& record) const;
  private: void fillWorkerWithVices(Worker& worker) const;
  private: QList<quint32> getVicesForWorker(const quint32 workerId) const;
  private: QList<quint32> extractVicesFromQueryResult(QList<SqlRecord>& queryResult) const;
  private: quint32 extractViceFromRecord(const SqlRecord& record) const;
  
  /*! \brief  получить из базы заказ по айди
   *  \param  orderId - айди заказа
   *  \return заказ */
  public:  Order getOrder(const quint32 orderId) const;
  
  /*! \brief  получить из базы список заказов по списку их айди
   *  \param  ordersIds - перечень айди заказов
   *  \return список заказов */
  public:  QList<Order> getOrders(const QList<quint32>& ordersIds) const;
  
  /*! \brief  получить из базы список всех заказов
   *  \return список всех заказов */
  public:  QList<Order> getAllOrders() const;
  
  private: QList<Order> getOrdersByCondition(const QString& condition) const;
  private: QList<Order> extractOrdersDataFromQueryResult(QList<SqlRecord>& queryResult) const;
  private: Order extractOrderDataFromRecord(const SqlRecord& record) const;
  
  /*! \brief  получить из базы проект по айди
   *  \param  projectId - айди проекта
   *  \return проект */
  public:  Project getProject(const quint32 projectId) const;
  
  /*! \brief  получить из базы список проектов по списку их айди
   *  \param  projectsIds - список айди проектов
   *  \return список проектов */
  public:  QList<Project> getProjects(const QList<quint32>& projectsIds) const;
  
  /*! \brief  получить из базы список всех проектов
   *  \return список всех проектов */
  public:  QList<Project> getAllProjects() const;
  
  private: QList<Project> getProjectsByCondition(const QString& condition) const;
  private: QList<Project> extractProjectsDataFromQueryResult(QList<SqlRecord>& queryResult) const;
  private: Project extractProjectDataFromRecord(const SqlRecord& record) const;
  
  /*! \brief  получить из базы перечень отчётов по списку их айди
   *  \param  reportsIds - список айди отчётов
   *  \return список отчётов */
  public:  QList<Report> getReports(const QList<quint32>& reportsIds) const;
  
  private: QList<Report> getReportsByCondition(const QString& condition) const;
  private: QList<Report> extractReportsDataFromQueryResultAndRelatedTables(QList<SqlRecord>& queryResult) const;
  private: Report extractReportDataFromRecord(const SqlRecord& record) const;
  private: void fillReportWithFiles(Report& report) const;
  private: QStringList getFilesForReport(const quint32 reportId) const;
  
  /*! \brief обновить поручение в базе (только в части данных sql-сервера, то есть без загрузки файлов на ftp-сервер)
   *  \param updated обновлённое поручение */
  public: void updateSqlTaskData(const Task& updated) const;
  
  /*! \brief обновить поручение в базе
   *  \param updated обновлённое поручение
   *  \param needLoadFiles костыль для опредления необходимости
   *                       обновления файлов-вложений к поручению */
  public:  void updateTask(const Task& updated) const;
  
  private: void tryToUpdateTaskDataOnSqlServer(const Task& updated) const;
  private: void updateTaskData(const Task& updated) const;
  private: void deleteRelatedDataForTask(const Task& updated) const;
  private: void deleteAccomplicesForTask(const quint32 taskId) const;
  private: void deleteCommentForTask(const quint32 taskId) const;
  private: void deleteWarningDateForTask(const quint32 taskId) const;
  private: void deleteOrdersForTask(const quint32 taskId) const;
  private: void deleteProjectsForTask(const quint32 taskId) const;
  private: void deleteFilesForTask(const quint32 taskId) const;
  private: void deleteReportForTask(const quint32 taskId) const;
  private: void tryToUpdateTaskDataOnFtpServer(const Task& updated) const;
  private: void updateTaskFilesOnFtpServer(const Task& task) const;
  private: void removeFtpFilesAfterUpdatingTask(const Task& task) const;
  
  /*! \brief обновить отчёт в базе
   *  \param updated - обновлённый отчёт */
  public:  void updateReport(const Report& updated) const;
  
  private: void tryToUpdateReportDataOnSqlServer(const Report& report) const;
  private: void updateReportData(const Report& report) const;
  private: void deleteFilesForReport(const quint32 reportId) const;
  private: void tryToUpdateReportDataOnFtpServer(const Report& report) const;
  private: void updateReportFilesOnFtpServer(const Report& report) const;
  private: void removeFtpFilesAfterUpdatingReport(const Report& report) const;
  
  /*! \brief  синхронизировать текущие дату и время с базой */
  public:  void synchronizeDateTime() const;
  
  private: QList<SqlRecord> executeGetServerDateTimeQuery() const;
  private: QDateTime extractServerDateTimeFromQueryResult(QList<SqlRecord>& queryResult) const;
  private: QDateTime extractServerDateTimeFromRecord(const SqlRecord& record) const;
  
  /*! \brief  отключиться от sql-сервера */
  public:  void disconnect() const;
  
  private: QList<SqlRecord> executeSelectQuery(const QString& selectQuery) const;
  
  private: void checkForTheOnlyRecordInQueryResult(const QList<SqlRecord>& queryResult) const;
  
  private: void checkForTheOnlyFieldInRecord(const SqlRecord& record) const;
  
  private: QString transformInText(quint32 value) const;
  
  //[трансформировать QList<quint32>: [1],[2],[3],[4],[5] в QString: "1,2,3,4,5" ]
  private: QString transformListOfIdsInText(const QList<quint32>& ids) const;
  
  private: void removeFolderFromFtpServer(const QString& path) const;
  
  private: void renameFolderOnFtpServer(const QString& oldName, const QString& newName) const;
  
  private: void uploadFilesToFtpServer(const QStringList files, const QString& destinationPath) const;
  
  #if TURN_ON_UNIT_TESTS
  //инструменты_для_заполнения_базы_тестовыми_данными__________________________
  /////////////////////////////////////////////////////////////////////////////
  public:  void addWorker(Worker& newWorker) const;
  
  private: void tryToAddWorkerDataToSqlServer(Worker& newWorker) const;
  private: void addWorkerData(const Worker& worker) const;
  private: quint32 defineIdOfJustAddedWorker() const;
  private: QList<SqlRecord> executeGetMaxWorkerIdQuery() const;
  private: void addViceIfExist(const Worker& worker) const;
  
  public:  void addVice(const quint32 workerId, const quint32 viceId) const;
  public:  void addSlave(const quint32 masterId, const quint32 slaveId) const;
  public:  void addOrder(const Order& order) const;
  public:  void addProject(const Project& project) const;
  
  public:  void dropDatabase() const;
  
  public:  void createDatabase() const;
  public:  void createTables() const;
  
  private: void createWorkersTable() const;
  private: void createTasksTable() const;
  private: void createCommentsTable() const;
  private: void createWarningDatesTable() const;
  private: void createRelationshipsTasksTable() const;
  private: void createRelationshipsTaskAccomplicesTable() const;
  private: void createProjectsTable() const;
  private: void createRelationshipsTaskProjectTable() const;
  private: void createOrdersTable() const;
  private: void createRelationshipsTaskOrderTable() const;
  private: void createRelationshipsMasterSlaveTable() const;
  private: void createFilesTasksTable() const;
  private: void createReportsTable() const;
  private: void createFilesReportsTable() const;
  private: void createRelationshipsTasksReportsTable() const;
  private: void createVicesTable() const;
  private: void createChangesTable() const;
  private: void createRelationshipsChangesAccomplicesOldStateTable() const;
  private: void createRelationshipsChangesAccomplicesNewStateTable() const;
  private: void createRelationshipsChangesFilesTasksOldStateTable() const;
  private: void createRelationshipsChangesFilesTasksNewStateTable() const;
  private: void createRelationshipsChangesFilesOrdersOldStateTable() const;
  private: void createRelationshipsChangesFilesOrdersNewStateTable() const;
  private: void createRelationshipsChangesProjectOldStateTable() const;
  private: void createRelationshipsChangesProjectNewStateTable() const;
  private: void createRelationshipsChangesOrderOldStateTable() const;
  private: void createRelationshipsChangesOrderNewStateTable() const;
  private: void createChangedDataTable() const;
  private: void createDBTable(const QString& queryText) const;
  #endif//TURN_ON_UNIT_TESTS
};

#endif // SQLHARDWORKERUNIT_H
