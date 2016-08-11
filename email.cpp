#include "email.h"
#include "general.h"

Email::Email()
{}

Email::Email(QString recipient, QString subject, QString text):_recipient(recipient),
                                                               _subject(subject), _text(text)
{
  Settings* settings = General::getSettings();
  _sender = settings->getEmailLogin();
}

const QString& Email::getSender() const
{
  return _sender;
}

void Email::setSender(const QString& sender)
{
  _sender = sender;
}

const QString& Email::getRecipient() const
{
  return _recipient;
}

void Email::setRecipient(const QString& recipient)
{
  _recipient = recipient;
}

const QString& Email::getSubject() const
{
  return _subject;
}

void Email::setSubject(const QString& subject)
{
  _subject = subject;
}

const QString& Email::getText() const
{
  return _text;
}

void Email::setText(const QString& text)
{
  _text = text;
}

bool Email::isValid() const
{
  if((!(_sender.isEmpty()))&&
     (!(_recipient.isEmpty()))&&
     (!(_text.isEmpty())))
  {
    return true;
  }
  else
  {
    return false;
  }
}
