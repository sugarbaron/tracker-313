/*! \file settings.h
 *  \brief интерфейс модуля извлечения параметров из настроечного файла
 *  \author sugarbaron ([bravery_and_stupidity] e-mail: sugarbaron1@mail.ru)
 *  \date 04.08.2016
 */
#ifndef SETTINGS
#define SETTINGS

#include <QString>

/*! \brief интерфейс модуля извлечения параметров из настроечного файла */
class Settings
{
  public: virtual QString getDefaultLogin() const = 0;
  
  public: virtual QString getEmailLogin() const = 0;

  public: virtual QString getEmailIp() const = 0;

  public: virtual quint32 getEmailPort() const = 0;

  public: virtual QString getEmailPassword() const = 0;

  public: virtual QString getFtpLogin() const = 0;

  public: virtual QString getFtpIp() const = 0;

  public: virtual quint32 getFtpPort() const = 0;

  public: virtual QString getFtpPassword() const = 0;

  public: virtual QString getSqlLogin() const = 0;

  public: virtual QString getSqlIp() const = 0;

  public: virtual quint32 getSqlPort() const = 0;

  public: virtual QString getSqlPassword() const = 0;
};
#endif // SETTINGS

