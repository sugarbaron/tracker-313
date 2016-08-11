/*! \file email.h
 *  \brief Интерфейс класса Email
 *  \author Бубенщиков А.И.
 *  \date 19.05.2016
 */

#ifndef EMAIL_H
#define EMAIL_H

#include "SmtpMime.h"

/*!
 * \brief Класс, описывающий сущность почтового сообщения
 */
class Email
{
  private:
    /// Отправитель письма
    QString _sender;

    /// Получатель письма
    QString _recipient;

    /// Заголовок письма
    QString _subject;

    /// Основной текст письма
    QString _text;

    /// Конструктор приватный
    Email();

  public:

    /// Конструктор с параметрами
    Email(QString recipient, QString subject, QString text);

    /// Интерфейсные методы доступа к полям

    const QString& getSender() const;
    void setSender(const QString& sender);

    const QString& getRecipient() const;
    void setRecipient(const QString& recipient);

    const QString& getSubject() const;
    void setSubject(const QString& subject);

    const QString& getText() const;
    void setText(const QString& text);

    /// Методы проверки объекта на валидность
    bool isValid() const;
};

// Тестовый код для проверки работы отправки почты через smtp
/*SmtpClient smtp("mx.npoa.org", 25, SmtpClient::TcpConnection);
// We need to set the username (your email address) and password
// for smtp authentification.
smtp.setUser("BubenshikovAI@npoa.org");
smtp.setPassword("15320666a");
smtp.setAuthMethod(SmtpClient::AuthLogin);
// Now we create a MimeMessage object. This is the email.
MimeMessage message;
EmailAddress sender("BubenshikovAI@npoa.org", "BubenshikovAI");
message.setSender(&sender);
EmailAddress to("SchepochkinIN@npoa.org", "Щепыч!");
message.addRecipient(&to);
message.setSubject("Тестовое письмо");
// Now add some text to the email.
// First we create a MimeText object.
MimeText text;
text.setText("Вот я прислал тебе письмо с трекера!\n");
// Now add it to the mail
message.addPart(&text);
// Now we can send the mail
if (!smtp.connectToHost()) {
    qDebug() << "Failed to connect to host!" << endl;
    return -1;
}
if (!smtp.login()) {
  qDebug() << "Failed to login!" << endl;
  return -2;
}
if (!smtp.sendMail(message)) {
    qDebug() << "Failed to send mail!" << endl;
    return -3;
}
smtp.quit();*/
#endif // EMAIL_H
