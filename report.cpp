#include "report.h"
#include "exception.h"
#include "sqlhardworker.h"
#include "general.h"

Report::Report():_id(0)
{}

bool Report::operator ==(const Report &r) const
{
  if(this->_id == r._id)
  {return true;}
  else
  {return false;}
}

bool Report::operator ==(const quint32 id) const
{
  if(this->_id == id)
  {return true;}
  else
  {return false;}
}

void Report::setId(quint32 id)
{
  _id = id;
}

void Report::setText(const QString& text)
{
  _text = text;
}

void Report::setFiles(const QStringList& files)
{
  _files = files;
}

void Report::setCreationDate(const QDate& date)
{
  _creationDate = date;
}

quint32 Report::getId() const
{
  return _id;
}

QString Report::getText() const
{
  return _text;
}

QStringList Report::getFiles() const
{
  return _files;
}

QDate Report::getCreationDate() const
{
  return _creationDate;
}

bool Report::isValid() const
{
  if((_creationDate.isValid())
    &&(!(_text.isEmpty())))
  {return true;}
  else
  {return false;}
}

void Report::synhronizeWithDb(quint32 id)
{
  if(id == 0)
  {
    QString msg = "wrong param, id is not valid";
    qCritical()<<msg;
    throw NeedFixCode(msg);
  }
  SqlHardWorker* sqlHardWorker = General::getSqlHardWorker();
  QList<quint32> ids;
  ids.push_back(id);
  QList<Report> reports = sqlHardWorker->getReports(ids);
  *this = reports.takeLast();
}

bool Report::isEqual(const Report& other) const
{
  bool areEqual = true;
  
  QString otherReportText = other.getText();
  QDate   otherReportCreationDate = other.getCreationDate();
  QStringList    otherReportFiles = other.getFiles();
  
  if( (_text   != otherReportText)||
      (_creationDate != otherReportCreationDate)||
      (_files != otherReportFiles) )
  {
    areEqual = false;
  }
  return areEqual;
}
