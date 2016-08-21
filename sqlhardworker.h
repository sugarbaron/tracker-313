/*! \file sqlhardworker.h
 *  \brief интерфейс sql-модуля приложения
 *  \author sugarbaron ([bravery_and_stupidity] e-mail: sugarbaron1@mail.ru)
 *  \date 02.12.2015
 */
#ifndef SQLHARDWORKER_H
#define SQLHARDWORKER_H
#include "worker.h"
#include "task.h"
#include "project.h"
#include "order.h"
#include "report.h"

/*! \class SqlHardWorker
 *  \brief интерфейс sql-модуля приложения */
class SqlHardWorker
{
  /*! \brief синхронизировать текущие дату и время с sql-сервером */
  public: virtual void synchronizeDateTime() const = 0;

  /*! \brief  получить работника из базы по айди
   *  \param  id - айди работника
   *  \return работника */
  public: virtual Worker getWorker(const quint32 id) const = 0;

  /*! \brief  получить работника из базы по его логину
   *  \param  login - логин пользователя
   *  \return работника */
  public: virtual Worker getWorker(const QString& login) const = 0;

  /*! \brief  получить список подчинённых для работника, указанного параметром
   *  \param  masterId - айди работника
   *  \return список подчинённых для работника, указанного параметром */
  public: virtual QList<Worker> getListOfResponsibleForWorker(const quint32 masterId) const = 0;

  /*! \brief  получить список заказов
   *  \return список заказов */
  public: virtual QList<Order> getAllOrders() const = 0;

  /*! \brief получить список проектов
   *  \return список проектов */
  public: virtual QList<Project> getAllProjects() const = 0;

  /*! \brief  получить список задач, порученных указанным работником
   *  \param  id - айди работника - автора задач
   *  \return список задач */
  public: virtual QList<Task> getTasksRequestedBy(const quint32 id) const = 0;

  /*! \brief  получить список всех задач для указанного работника
   *  \param  id - айди воркера - участника задач
   *  \return список задач */
  public: virtual QList<Task> getAllWorkerTasks(const quint32 id) const = 0;

  /*! \brief  получить список задач, порученных указанному работнику
   *  \param  id - айди работника
   *  \return список задач */
  public: virtual QList<Task> getTasksForResponsible(const quint32 id) const = 0;

  /*! \brief  получить список выполненных и снятых задач для указанного работника
   *  \param  id - айди работника
   *  \return список задач */
  public: virtual QList<Task> getCompleteTasksForWorker(const quint32 id) const = 0;

  /*! \brief  получить из базы задачу по айди
   *  \param  id - айди задачи
   *  \return поручение */
  public: virtual Task getTask(const qint32 id) const = 0;

  /*! \brief  получить из базы проект по айди
   *  \param  id - айди проекта
   *  \return проект */
  public: virtual Project getProject(const quint32 id) const = 0;

  /*! \brief  получить из базы список проектов в соответствии по списку их айди
   *  \param  ids - список айди проектов
   *  \return список проектов */
  public: virtual QList<Project> getProjects(const QList<quint32>& ids) const = 0;

  /*! \brief  получить из базы заказ по айди
   *  \param  id - айди заказа
   *  \return заказ */
  public: virtual Order getOrder(const quint32 id) const = 0;

  /*! \brief  получить из базы список заказов по списку их айди
   *  \param  ids - список айди заказов
   *  \return список заказов */
  public: virtual QList<Order> getOrders(const QList<quint32>& ids) const = 0;

  /*! \brief  получить список работников по списку их id
   *  \param  id - список id работников
   *  \return список работников */
  public: virtual QList<Worker> getWorkers(const QList<quint32>& idList) const = 0;

  /*! \brief  получить из базы список отчётов по списку их айди
   *  \param  ids - список айди отчётов
   *  \return список отчётов */
  public: virtual QList<Report> getReports(const QList<quint32>& ids) const = 0;

  /*! \brief добавить новую задачу в базу
   *  \param task - задача */
  public: virtual void addTask(Task& task) const = 0;

  /*! \brief добавить в базу отчёт по указанному поручению
   *  \param report - отчёт
   *  \param taskId - айди поручения по которому создан отчет */
  public: virtual void addReport(Report& report, const quint32 taskId) const = 0;

  /*! \brief обновить поручение в базе (только в части данных sql-сервера, то есть без загрузки файлов на ftp-сервер)
   *  \param updated обновлённое поручение */
  public: virtual void updateSqlTaskData(const Task& updated) const = 0;
  
  /*! \brief обновить задачу в базе
   *  \param updated - обновлённая задача
   *  \param needLoadFiles - костыль для опредления необходимости обновления файлов-вложений к поручению */
  public: virtual void updateTask(const Task& updated) const = 0;

  /*! \brief обновить отчёт в базе
   *  \param updated - обновлённый отчёт */
  public: virtual void updateReport(const Report& updated) const = 0;

  /*! \brief disconnect отсоединиться от sql-сервера */
  public: virtual void disconnect() const = 0;
};

#endif // SQLHARDWORKER_H
