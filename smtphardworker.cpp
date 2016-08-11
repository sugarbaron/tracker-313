#include "smtphardworker.h"
#include "exception.h"
#include "general.h"

SmtpHardWorker::SmtpHardWorker()
{
  // Подключаем обработчик ошибок smtp сервера
  QObject::connect(&(_smtp),SIGNAL(smtpError(SmtpClient::SmtpError)),
                   this,SLOT(SmtpErrorHandler(SmtpClient::SmtpError)));
}

SmtpHardWorker* SmtpHardWorker::getInstance()
{
  if(_instance.isNull())
  {
    _instance.reset(new SmtpHardWorker);
  }
  return _instance.data();
}

void SmtpHardWorker::connectToSmtpServer(const QString& adress,
                                          quint32 port,
                                          const QString& login,
                                          const QString& pass)
{
  _smtp.setHost(adress);
  _smtp.setPort(port);
  _smtp.setAuthMethod(SmtpClient::AuthLogin);
  _smtp.setUser(login);
  _smtp.setPassword(pass);
  _smtp.setConnectionTimeout(_connectionTimout);
  //_smtp.setConnectionType(SmtpClient::SslConnection);  //dbg
  if (!(_smtp.connectToHost()))
  {
    qCritical()<<QObject::tr("Failed to connect to host!");
    return;
  }
  if (!(_smtp.login()))
  {
    qCritical()<<QObject::tr("Failed to login!");
    return;
  }
  //_isConnected = true;
  qDebug()<<QObject::tr("Connected to SMTP server: ")<<adress;
}

void SmtpHardWorker::sendEmail(const Email& email)
{
  if(!email.isValid())
  {
    QString msg = QObject::tr("Email is not valid");
    throw NeedFixCode(msg);
  }
  
  Clock* systemClock = General::getSystemClock();
  Settings* settings = General::getSettings();
  // Получаем реквизиты для подключения к smtp серверу
  QString emailIp    = settings->getEmailIp();
  quint32 emailPort  = settings->getEmailPort();
  QString emailLogin = settings->getEmailLogin();
  QString emailPass  = settings->getEmailPassword();

  // Подключаемся к smtp серверу
  connectToSmtpServer(emailIp, emailPort, emailLogin, emailPass);

  // Заполняем сообщение
  MimeMessage message;
  EmailAddress sender(email.getSender());
  message.setSender(&sender);
  EmailAddress to(email.getRecipient());
  message.addRecipient(&to);
  message.setSubject(email.getSubject());
  MimeText text;
  text.setText(email.getText());
  message.addPart(&text);

  // Отправляем сообщение
  if (!(_smtp.sendMail(message)))
  {
    QString msg = QObject::tr("Failed to send mail!");
    qCritical()<<msg;
    disconnect();
    throw SmtpException(msg);
  }
  qDebug()<<"Email to "<<email.getRecipient()<<" sended";
  disconnect();
  // Пауза необходима для возможности отправки нескольких
  // сообщений в цикле, без задержки сервер не успевает
  // обработать заявки и возврщает ошибку
  systemClock->msThreadSleep(50);
}

void SmtpHardWorker::SmtpErrorHandler(SmtpClient::SmtpError e)
{
  switch(e)
  {
    case SmtpClient::ConnectionTimeoutError:
      qCritical()<<QObject::tr("Connection Timeout Error");
      break;
    case SmtpClient::ResponseTimeoutError:
      qCritical()<<QObject::tr("Response Timeout Error");
      break;
    case SmtpClient::SendDataTimeoutError:
      qCritical()<<QObject::tr("Send Data Timeout Error");
      break;
    case SmtpClient::AuthenticationFailedError:
      qCritical()<<QObject::tr("Authentication Failed Error");
      break;
    case SmtpClient::ServerError:
      qCritical()<<QObject::tr("Server Error");
      break;
    case SmtpClient::ClientError:
      qCritical()<<QObject::tr("Client Error");
      break;
    default:
      qCritical()<<QObject::tr("Unknow Error");
  }
}

void SmtpHardWorker::disconnect()
{
  try
  { // Этот метод библиотеки работы с SMTP кидает необрабатываемое исключение
    // в случае, если соединение уже было разорвано и вызван метод quit
    _smtp.quit();
  }
  catch(...)
  {}
  qDebug()<<QObject::tr("disconnected from SMTP server");
  //}
}

SmtpHardWorker::~SmtpHardWorker()
{}

QScopedPointer<SmtpHardWorker> SmtpHardWorker::_instance;
