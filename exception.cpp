#include "exception.h"

Exception::Exception()
{}

Exception::Exception(QString exceptionInfo): _exceptionInfo(exceptionInfo)
{}

QString Exception::getInfo()
{
  return _exceptionInfo;
}

SqlQueryException::SqlQueryException(QString exceptionInfo)
{
  _exceptionInfo = "[SqlQueryException]: ";
  _exceptionInfo += exceptionInfo;
}

NeedFixCode::NeedFixCode(QString exceptionInfo)
{
  _exceptionInfo = "[NeedFixCodeException]: ";
  _exceptionInfo += exceptionInfo;
}

CriticalOperationException::CriticalOperationException(QString exceptionInfo)
{
  _exceptionInfo = "[CriticalOperationException]: ";
  _exceptionInfo += exceptionInfo;
}

ConnectException::ConnectException(QString exceptionInfo)
{
  _exceptionInfo = "[ConnectExeption]: ";
  _exceptionInfo += exceptionInfo;
}

InitException::InitException(QString exceptionInfo)
{
  _exceptionInfo = "[InitExeption]: ";
  _exceptionInfo += exceptionInfo;
}

ExecutionAborted::ExecutionAborted()
{
  _exceptionInfo = "[ExecutionAborted]";
}

ExecutionAborted::ExecutionAborted(QString exceptionInfo)
{
  _exceptionInfo = "[ExecutionAborted]: ";
  _exceptionInfo += exceptionInfo;
}

FtpException::FtpException()
{
  _exceptionInfo = "[FtpException]";
}

FtpException::FtpException(QString exceptionInfo)
{
  _exceptionInfo = "[FtpException]: ";
  _exceptionInfo += exceptionInfo;
}


SmtpException::SmtpException()
{
  _exceptionInfo = "[SmtpException]";
}

SmtpException::SmtpException(QString exceptionInfo)
{
  _exceptionInfo = "[SmtpException]: ";
  _exceptionInfo += exceptionInfo;
}

SettingsFileException::SettingsFileException(QString exceptionInfo)
{
  _exceptionInfo = "[SettingsFileException]: ";
  _exceptionInfo += exceptionInfo;
}

TestFailed::TestFailed(QString exceptionInfo)
{
  _exceptionInfo = "[TestFailed]: ";
  _exceptionInfo += exceptionInfo;
}

NotDetected::NotDetected(QString exceptionInfo)
{
  _exceptionInfo = "[NotDetected]: ";
  _exceptionInfo += exceptionInfo;
}
