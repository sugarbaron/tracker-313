#include "task.h"
#include "exception.h"
#include "general.h"


Task::Task():_idTask(0),
             _description(""),
             _comment(""),
             _authorId(0),
             _responsibleId(0),
             _accomplicesId(),
             _creationDate(),
             _deadlineDate(),
             _warningDate(),
             _orderId(0),
             _projectId(0),
             _reportId(0),
             _filesTask(),
             _status(statusValuesQuantity),
             _isOverdue(false),
             _importance(importanceValuesQuantity),
             _isBidForReport(false),
             _reportApply(false),
             _isNewState(false),
             _parentTaskId(0)
{}

bool Task::operator ==(const Task &w) const
{
  if(this->_idTask == w._idTask)
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool Task::operator ==(const quint32 id) const
{
  if(this->_idTask == id)
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool Task::operator < (const Task& w) const
{
  // сначала сравниваем статусы
  if(this->getStatus() < w.getStatus())
  {
    return true;
  }
  else
  { // если статусы совпали
    if(this->getStatus() == w.getStatus())
    {
      if((this->getIsOverdue())&&(!w.getIsOverdue()))
      {
        return true;
      }
      else
      {
        // если одно поручение просрочено, а второе нет
        Clock* systemClock = General::getSystemClock();
        QDateTime now = systemClock->getTime();
        QDate today = now.date();
        if( (this->getDeadlineDate() < today)&&
            (w.getDeadlineDate() >= today) )
        {
          return true;
        }
        else
        {
          // сравниваем даты контроля
          if(this->getDeadlineDate() < w.getDeadlineDate())
          {
            return true;
          }
          else
          { // если даты контроля совпали
            if(this->getDeadlineDate() == w.getDeadlineDate())
            { // сравниваем важность
              if(this->getImportance() > w.getImportance())
              {
                return true;
              }
            }
          }
        }
      }
    }
    return false;
  }
}

QString Task::toString() const
{
  QString str;
  QTextStream out(&str,QIODevice::ReadWrite);
  out<<"_idTask = "<<QString::number((this->_idTask),10)<<endl;
  out<<"_description = "<<this->_description<<endl;
  out<<"_comment = "<<this->_comment<<endl;
  out<<"_author = "<<this->_authorId<<endl;
  out<<"_responsible = "<<this->_responsibleId<<endl;
  foreach (quint32 id, this->_accomplicesId)
  {
    out<<"_accomplices = "<<id<<endl;
  }
  out<<"_creationDate = "<<this->_creationDate.toString()<<endl;
  out<<"_deadlineDate = "<<this->_deadlineDate.toString()<<endl;
  out<<"_warningDate = "<<this->_warningDate.toString()<<endl;
  out<<"_order = "<<this->_orderId<<endl;
  out<<"_project = "<<this->_projectId<<endl;
  foreach (QString str, this->_filesTask)
  {
    out<<"_filesTask = "<<str<<endl;
  }
  /*foreach (QString str, this->_filesReport)
  {
    out<<"_filesReport = "<<str<<endl;
  }
  out<<"_status = "<<QString::number(this->_status,10)<<endl;
  out<<"_isOverdue = "<<QString::number(this->_isOverdue,10)<<endl;
  out<<"_importance = "<<QString::number(this->_importance,10)<<endl;
  out<<"_isBidForReport = "<<QString::number(this->_isBidForReport,10)<<endl;
  out<<"_reportApply = "<<QString::number(this->_reportApply,10)<<endl;
  out<<"_isNewState = "<<QString::number(this->_isNewState,10)<<endl;*/
  return str;
}



void Task::setIdTask(quint32 idTask)
{
  _idTask = idTask;
}

void Task::setDescription(QString description)
{
  _description = description;
}

void Task::setComment(QString comment)
{
  _comment = comment;
}

void Task::setAuthor(quint32 authorId)
{
  _authorId = authorId;
}

void Task::setResponsible(quint32 responsibleId)
{
  _responsibleId = responsibleId;
}

void Task::setAccomplices(QList<quint32> accomplicesId)
{
  _accomplicesId = accomplicesId;
}

void Task::setCreationDate(QDate creationDate)
{
  _creationDate = creationDate;
}

void Task::setDeadlineDate(QDate deadlineDate)
{
  _deadlineDate = deadlineDate;
}

void Task::setWarningDate(QDate warningDate)
{
  _warningDate = warningDate;
}

void Task::setOrder(quint32 orderId)
{
  _orderId = orderId;
}

void Task::setProject(quint32 projectId)
{
  _projectId = projectId;
}

void Task::setFilesTask(QStringList filesTask)
{
  _filesTask = filesTask;
}

void Task::setStatus(QString status)
{
  if(status == QObject::tr("В работе"))
  {
    _status = inProgress;
  }
  else
  if(status == QObject::tr("Выполнено"))
  {
    _status = complete;
  }
  else
  if(status == QObject::tr("Снято"))
  {
    _status = canceled;
  }
  else
  if(status == QObject::tr("Новое"))
  {
    _status = newTask;
  }
  else
  if(status == QObject::tr("Есть обновление"))
  {
    _status = updated;
  }
  else
  if(status == QObject::tr("Заявка на отчет"))
  {
    _status = isBidForReport;
  }
  else
  {
    QString message = "wrong argument: #" + status;
    qCritical()<<message;
    throw NeedFixCode(message);
  }
}

void Task::setStatus(quint32 status)
{
  _status = status;
}

void Task::setIsOverdue(bool isOverdue)
{
  _isOverdue = isOverdue;
}

void Task::setImportance(quint32 importance)
{
  _importance = importance;
}

void Task::setIsBidForReport(bool isBidForReport)
{
  _isBidForReport =isBidForReport;
}

void Task::setReportApply(bool reportApply)
{
  _reportApply = reportApply;
}

void Task::setIsNewState(bool isNewState)
{
  _isNewState = isNewState;
}

void Task::setParent(quint32 parentTaskId)
{
  _parentTaskId = parentTaskId;
}

quint32 Task::getIdTask() const
{
  return _idTask;
}

QString Task::getDescription() const
{
  return _description;
}

QString Task::getComment() const
{
  return _comment;
}

quint32 Task::getAuthorId() const
{
  return _authorId;
}

void Task::setReportId(quint32 reportId)
{
  _reportId = reportId;
}

quint32 Task::getReportId() const
{
  return _reportId;
}

Worker Task::getAuthorDb() const
{
  if(0 == _authorId)
  {
    QString msg = "authorId not init";
    qCritical()<<msg;
    throw NeedFixCode(msg);
  }
  Worker author;
  author.synhronizeWithDb(_authorId);
  return author;
}

Project Task::getProjectDb() const
{
  if(0 == _projectId)
  {
    throw ExecutionAborted();
  }
  Project project;
  project.synhronizeWithDb(_projectId);
  return project;
}

Order Task::getOrderDb() const
{
  if(0 == _orderId)
  {
    throw ExecutionAborted();
  }
  Order order;
  order.synhronizeWithDb(_orderId);
  return order;
}

Worker Task::getResponsibleDb() const
{
  if(0 == _responsibleId)
  {
    QString msg = "responsible id not init";
    qCritical()<<msg;
    throw NeedFixCode(msg);
  }
  Worker responsible;
  responsible.synhronizeWithDb(_responsibleId);
  return responsible;
}

Report Task::getReportDb() const
{
  if(0 == _reportId)
  {
    QString msg = "responsible id not init";
    qCritical()<<msg;
    throw NeedFixCode(msg);
  }
  Report report;
  report.synhronizeWithDb(_reportId);
  return report;
}

QList<Worker> Task::getAccomplicesDb() const
{
  if(_accomplicesId.isEmpty())
  {
    throw ExecutionAborted();
  }
  SqlHardWorker* sqlHardWorker = General::getSqlHardWorker();
  QList<Worker>accomplices;
  accomplices = sqlHardWorker->getWorkers(_accomplicesId);
  return accomplices;
}

quint32 Task::getResponsibleId() const
{
  return _responsibleId;
}

QList<quint32> Task::getAccomplicesId() const
{
  return _accomplicesId;
}

QDate Task::getCreationDate() const
{
  return _creationDate;
}

QDate Task::getDeadlineDate() const
{
  return _deadlineDate;
}

QDate Task::getWarningDate() const
{
  return _warningDate;
}

quint32 Task::getOrderId() const
{
  return _orderId;
}

quint32 Task::getProjectId() const
{
  return _projectId;
}

QStringList Task::getFilesTask() const
{
  return _filesTask;
}

quint32 Task::getStatus() const
{
  return _status;
}

bool Task::getIsOverdue() const
{
  return _isOverdue;
}

quint32 Task::getImportance() const
{
  return _importance;
}

bool Task::getIsBidForReport() const
{
  return _isBidForReport;
}

bool Task::getReportApply() const
{
  return _reportApply;
}

bool Task::getIsNewState() const
{
  return _isNewState;
}

QString Task::getTextStatus() const
{
  QString ret;
  switch(_status)
  {
    case inProgress:
      ret = QObject::tr("В работе");
      break;
    case complete:
      ret = QObject::tr("Выполнено");
      break;
    case canceled:
      ret = QObject::tr("Снято");
      break;
    case newTask:
      ret = QObject::tr("Новое");
      break;
    case updated:
      ret = QObject::tr("Есть обновление");
      break;
    case isBidForReport:
      ret = QObject::tr("Заявка на отчет");
      break;
    default:
      qCritical()<<"default branch, bull shit, status = "<<_status;
      throw NeedFixCode("default branch");
  }
  return ret;
}

quint32 Task::getParent() const
{
  return _parentTaskId;
}

bool Task::isValid() const
{
  if((!_description.isEmpty())
     &&(_creationDate.isValid())
     &&(_deadlineDate.isValid())
     &&(_status <= complete)
     &&(_importance <= maxImportance))
  {
    return true;
  }
  else
  {
    return false;
  }
}

QString Task::getStringAccomplicesDb() const
{
  QString stringAccomplices;
  Worker worker;
  foreach (quint32 id, _accomplicesId)
  {
    worker.synhronizeWithDb(id);
    if(_accomplicesId.last() == id)
    {
      stringAccomplices += worker.getShortName();
    }
    else
    {
      stringAccomplices += worker.getShortName() + "\n";
    }
  }
  return stringAccomplices;
}

void Task::synhronizeWithDb(quint32 id)
{
  SqlHardWorker* sqlHardWorker = General::getSqlHardWorker();
  *this = sqlHardWorker->getTask(id);
}

bool Task::isEqual(const Task& other) const
{
  bool areEqual = true;
  
  quint32 otherTaskStatus = other.getStatus();
  quint32 otherTaskOrderId  = other.getOrderId();
  quint32 otherTaskParentId = other.getParent();
  quint32 otherTaskAuthorId = other.getAuthorId();
  quint32 otherTaskReportId = other.getReportId();
  quint32 otherTaskProjectId  = other.getProjectId();
  quint32 otherTaskImportance = other.getImportance();
  quint32 otherTaskResponsibleId = other.getResponsibleId();
  QString otherTaskComment = other.getComment();
  QString otherTaskDescription = other.getDescription();
  QDate otherTaskCreationDate = other.getCreationDate();
  QDate otherTaskDeadlineDate = other.getDeadlineDate();
  QDate otherTaskWarningDate  = other.getWarningDate();
  bool  otherTaskIsOverdue = other.getIsOverdue();
  QStringList otherTaskFiles = other.getFilesTask();
  QList<quint32> otherTaskAccomplicesIds = other.getAccomplicesId();
  
  if( (_description != otherTaskDescription)
    ||(_comment != otherTaskComment)
    ||(_parentTaskId != otherTaskParentId)
    ||(_authorId != otherTaskAuthorId)
    ||(_responsibleId != otherTaskResponsibleId)
    ||(_reportId != otherTaskReportId)
    ||(_orderId  != otherTaskOrderId)
    ||(_projectId != otherTaskProjectId)
    ||(_creationDate != otherTaskCreationDate)
    ||(_deadlineDate != otherTaskDeadlineDate)
    ||(_warningDate  != otherTaskWarningDate)
    ||(_isOverdue  != otherTaskIsOverdue)
    ||(_importance != otherTaskImportance)
    ||(_status != otherTaskStatus)
    ||(_filesTask != otherTaskFiles)
    ||(_accomplicesId != otherTaskAccomplicesIds) )
  {
    areEqual = false;
  }
  return areEqual;
}
