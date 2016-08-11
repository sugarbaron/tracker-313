/*! \file exception.h
 *  \brief Описание интерфейсов классов исключений
 *  \author Бубенщиков А.И.
 *  \date 22.12.2015
 */
#ifndef EXCEPTION_H
#define EXCEPTION_H
#include <QtCore>

/*!
 * \brief Базовый класс исключения
 */
class Exception
{
  protected:
    QString _exceptionInfo;
  public:
    QString getInfo();
    Exception();
    explicit Exception(QString exceptionInfo);
};

/*!
 * \brief Исключение генерируемое при ошибке sql запроса
 */
class SqlQueryException: public Exception
{
  public:
    explicit SqlQueryException(QString exceptionInfo);
};

/*!
 * \brief Исключение генерируемое при ошибке для устранения
 * которой необходимо править исходный код
 */
class NeedFixCode: public Exception
{
  public:
    explicit NeedFixCode(QString exceptionInfo);
};

/*!
 * \brief Исключение генерируемое при возниконовении критической ошибки
 */
class CriticalOperationException: public Exception
{
  public:
    explicit CriticalOperationException(QString exceptionInfo);
};

/*!
 * \brief Исключение генерируемое при неудачной попытке подключения
 */
class ConnectException:public Exception
{
  public:
   explicit ConnectException(QString exceptionInfo);
};

/*!
 * \brief Исключение генерируемое при неудачной попытке инициализации
 */
class InitException:public Exception
{
  public:
   explicit InitException(QString exceptionInfo);
};

/*!
 * \brief Исключение, генерируемое в том случае если существуют условия,
 * не позволяющие методу выполнить свою задачу, однако никаких ошибок
 * функционирования системы не зафиксировано
 */
class ExecutionAborted: public Exception
{
  public:
    ExecutionAborted();
    explicit ExecutionAborted(QString exceptionInfo);
};

/// Исключение, генерируемое в случае неудачной скачки/загруски файлов с/на FTP сервер
class FtpException: public Exception
{
  public:
    FtpException();
    explicit FtpException(QString exceptionInfo);
};

/// Исключение, генерируемое в случае неудачной попытки отправки email
class SmtpException: public Exception
{
  public:
    SmtpException();
    explicit SmtpException(QString exceptionInfo);
};

/*!
 * \brief Исключение, генерируемое в случае обнаружения проблем с файлом настроек
 */
class SettingsFileException: public Exception
{
  public:
    explicit SettingsFileException(QString exceptionInfo);
};

class TestFailed: public Exception
{
  public:
    explicit TestFailed(QString exceptionInfo);
};

class NotDetected: public Exception
{
  public:
    explicit NotDetected(QString exceptionInfo);
};

#endif // EXCEPTION_H
