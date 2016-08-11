#ifndef SMTPHARDWORKER_H
#define SMTPHARDWORKER_H

#include <SmtpMime.h>
#include "email.h"

class SmtpHardWorker: public QObject
{
  Q_OBJECT
  private:

    /// Таймаут подключения к smtp серверу
    static const qint32 _connectionTimout = 5000;

    /// Конструктор по умолчанию
    explicit SmtpHardWorker();

    /// Конструктор копирования
    explicit SmtpHardWorker(const SmtpHardWorker&);

    /// Оператор присваивания
    SmtpHardWorker& operator = (SmtpHardWorker&);

    /// Объект для работы с SMTP сервером
    SmtpClient _smtp;

    /// Единственный экземпляр класса FtpHardWorker
    static QScopedPointer<SmtpHardWorker> _instance;

  private slots:

    /// Обработчик ошибок smtp сервера
    void SmtpErrorHandler(SmtpClient::SmtpError e);

  public:

    /// Метод, возвращающий указатель на экземпляр объекта
    static SmtpHardWorker *getInstance();

    /*!
     * \brief coonectToSmtpServer Метод для подключения к SMTP серверу
     * \param adress              Адрес почтового сервера
     * \param port                Порт для подключения
     * \param login               Логин для авторизации
     * \param pass                Пароль
     */
    void connectToSmtpServer(const QString &adress, quint32 port, const QString &login, const QString &pass);

    /*!
     * \brief disconnect Метод для отключения от SMTP сервера
     */
    void disconnect();

    /*!
     * \brief sendEmail Метод для отправки почтового сообщения
     * \param email Почтовое сообщение, которое необходимо отправить
     */
    void sendEmail(const Email& email);

    /// Деструктор
    virtual ~SmtpHardWorker();
};

#endif // SMTPHARDWORKER_H
