/*! \file task.h
 *  \brief Описание интерфейса класса Task
 *  \author Бубенщиков А.И.
 *  \date 25.12.2015
 */

#ifndef TASK_H
#define TASK_H

#include <QObject>
#include <QDate>
#include <QStringList>
#include <QTextStream>
#include "worker.h"
#include "project.h"
#include "order.h"
#include "report.h"


/*!
 * \brief Класс, описывающий сущьность - поручение
 */
class Task
{
  private:
    /// Границы изменения важности поручения
    enum importance
    {
      minImportance = 1,
      maxImportance = 10,
      importanceValuesQuantity
    };
    /// ID поручения
    quint32 _idTask;
    /// Суть поручения
    QString _description;
    /// Комментарий к поручению
    QString _comment;
    /// Id автора поручения
    quint32 _authorId;
    /// Id ответственного за поручение
    quint32 _responsibleId;
    /// Список Id соисполнителей поручения
    QList<quint32> _accomplicesId;
    /// Дата создания
    QDate _creationDate;
    /// Дата контроля
    QDate _deadlineDate;
    /// Дата начала напоминаний о поручения
    QDate _warningDate;
    /// Id заказа
    quint32 _orderId;
    /// Id проекта
    quint32 _projectId;
    /// Id отчета по поручению
    quint32 _reportId;
    /// Список файлов вложения поручения
    QStringList _filesTask;
    /// Статут поручения (0 - в работе, 1-выполнено, 2 - снято, 3 - новое, 4 - есть обновления)
    quint32 _status;
    /// Было ли поручение просрочено(true - просрочено, false - нет)
    bool _isOverdue;
    /// Важность поручения (minImportance-maxImportance)
    quint32 _importance;
    /// Есть ли заявка на отчет(true - есть, false - нет)
    bool _isBidForReport;
    /// Принят ли отчет(true - принят, false - нет)
    bool _reportApply;
    /// Есть ли обновленное состояние(true - есть, false - нет)
    bool _isNewState;
    /// Айди родительского поручения
    quint32 _parentTaskId;

  public:
    /// Операторы сравнения
    bool operator ==(const Task& w) const;
    bool operator < (const Task& w) const;
    bool operator ==(const quint32 id) const;

    /// Статусы поручения
    enum TaskStatus
    {
      /// Новое
      newTask = 0,
      /// Есть заявка на отчет
      isBidForReport = 1,
      /// Есть обновление
      updated = 2,
      /// В работе
      inProgress = 3,
      /// Снято
      canceled = 4,
      /// Выполнено
      complete = 5,
      /// Значение не определено
      statusValuesQuantity
    };

    /// Конструктор
    Task();

    /// Метод преобразующий данные о поручении в строку
    QString toString() const;

    //========== Интерфейсные методы доступа для записи к закрытым полям класса=================
    void setIdTask(quint32 idTask);
    void setDescription(QString description);
    void setComment(QString comment);
    void setAuthor(quint32 authorId);
    void setResponsible(quint32 responsibleId);
    void setAccomplices(QList<quint32> accomplicesId);
    void setCreationDate(QDate creationDate);
    void setDeadlineDate(QDate deadlineDate);
    void setWarningDate(QDate warningDate);
    void setOrder(quint32 order);
    void setProject(quint32 projectId);
    void setFilesTask(QStringList filesTask);
    void setStatus(quint32 status);
    void setStatus(QString status);
    void setIsOverdue(bool isOverdue);
    void setImportance(quint32 inportance);
    void setIsBidForReport(bool isBidForReport);
    void setReportApply(bool reportApply);
    void setIsNewState(bool isNewState);
    void setParent(quint32 parentTaskId);
    void setReportId(quint32 reportId);

    //========== Интерфейсные методы доступа для чтения к закрытым полям класса=================
    quint32 getIdTask() const;
    QString getDescription() const;
    QString getComment() const;
    quint32 getAuthorId() const;
    quint32 getResponsibleId() const;
    QList<quint32> getAccomplicesId() const;
    QDate getCreationDate() const;
    QDate getDeadlineDate() const;
    QDate getWarningDate() const;
    quint32 getOrderId() const;
    quint32 getProjectId() const;
    QStringList getFilesTask() const;
    quint32 getStatus() const;
    QString getTextStatus() const;
    bool getIsOverdue() const;
    quint32 getImportance() const;
    bool getIsBidForReport() const;
    bool getReportApply() const;
    bool getIsNewState() const;
    quint32 getParent() const;
	quint32 getReportId() const;

    //=============================================================================================
    /// Метод проверяющий что все обязательные поля объекта заполнены и их содержимое корректно
    bool isValid() const;

    //=============================================================================================
    //====================== Методы, запрашивающие информацию из БД================================

    /*!
     * \brief Возвращает короткие имена соисполнителей поручения в виде строки
     * \throws NeedFixCode если воркер, передаваемый аргументом не валидный
     * \throws SqlQueryExcepiton если произошла ошибка запроса к базе данных
     */
    QString getStringAccomplicesDb() const;

    /*!
     * \brief Возвращает список соисполнителей для поручения(информация запрашивается из БД)
     * \throws ExecutionAborted в случае, если в поручении нет соисполнителей
     * \throws SqlQueryExcepiton если произошла ошибка запроса к базе данных
     */
    QList<Worker> getAccomplicesDb() const;

    /*!
     * \brief Метод синхронизирует объект БД
     * \param id работника с которым будет выполнена синхронизация
     * \throws NeedFixCode если воркер, передаваемый аргументом не валидный
     * \throws SqlQueryExcepiton если произошла ошибка запроса к базе данных
     */
    void synhronizeWithDb(quint32 id);

    /*!
     * \brief Возвращает автора поручения(информация запрашивается из БД)
     * \throws NeedFixCode в случае, если id автора не определен в поручении
     * \throws SqlQueryExcepiton если произошла ошибка запроса к базе данных
     * \return Автора поручения
     */
    Worker getAuthorDb() const;

    /*!
     * \brief Возвращает проект в который включено поручение(информация запрашивается из БД)
     * \throws ExecutionAborted в случае, если поручение не включено в проект
     * \throws SqlQueryExcepiton если произошла ошибка запроса к базе данных
     * \return Проект
     */
    Project getProjectDb() const;

    /*!
     * \brief Возвращает заказ к которому относится поручение(информация запрашивается из БД)
     * \throws ExecutionAborted в случае, если поручение не относится к заказу
     * \throws SqlQueryExcepiton если произошла ошибка запроса к базе данных
     * \return Проект
     */
    Order getOrderDb() const;

    /*!
     * \brief Возвращает ответственного за поручение(информация запрашивается из БД)
     * \throws NeedFixCode в случае, если id ответственного не определен в поручении
     * \throws SqlQueryExcepiton если произошла ошибка запроса к базе данных
     * \return Ответственного за поручение
     */
    Worker getResponsibleDb() const;

    /*!
     * \brief Возвращает отчет по поручению(информация запрашивается из БД)
     * \throws NeedFixCode в случае, если по поручению не создан отчет
     * \throws SqlQueryExcepiton если произошла ошибка запроса к базе данных
     * \return Ответственного за поручение
     */
    Report getReportDb() const;
    
    bool isEqual(const Task& other) const;

  signals:

  public slots:

};

#endif // TASK_H
