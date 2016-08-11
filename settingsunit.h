/*! \file settingsunit.h
 *  \brief модуль извлечения параметров из настроечного файла
 *  \author sugarbaron ([bravery_and_stupidity] e-mail: sugarbaron1@mail.ru)
 *  \date 04.08.2016
 */
#ifndef SETTINGSUNIT
#define SETTINGSUNIT

#include "settings.h"
#include <QScopedPointer>
#include <QFile>

/*! \brief модуль извлечения параметров из настроечного файла */
class SettingsUnit : public Settings
{
  //секция_данных______________________________________________________________
  /////////////////////////////////////////////////////////////////////////////
  private: static const QString FTP_IP_TAG;
  private: static const QString SQL_IP_TAG;
  private: static const QString EMAIL_IP_TAG;
  private: static const QString SQL_PORT_TAG;
  private: static const QString FTP_PORT_TAG;
  private: static const QString EMAIL_PORT_TAG;
  private: static const QString FTP_LOGIN_TAG;
  private: static const QString SQL_LOGIN_TAG;
  private: static const QString EMAIL_LOGIN_TAG;
  private: static const QString FTP_PASSWORD_TAG;
  private: static const QString SQL_PASSWORD_TAG;
  private: static const QString EMAIL_PASSWORD_TAG;
  private: static const QString DEFAULT_LOGIN_TAG;
  
  private: static QScopedPointer<SettingsUnit> _thisUnit;
  
  private: QFile _settingsFile;
  private: QString _emailLogin;
  private: QString _emailIp;
  private: quint32 _emailPort;
  private: QString _emailPassword;
  private: QString _ftpLogin;
  private: QString _ftpIp;
  private: quint32 _ftpPort;
  private: QString _ftpPassword;
  private: QString _sqlLogin;
  private: QString _sqlIp;
  private: quint32 _sqlPort;
  private: QString _sqlPassword;
  private: QString _defaultLogin;
  
  //секция_методов_____________________________________________________________
  /////////////////////////////////////////////////////////////////////////////
  public:  static SettingsUnit* create();
  private: static void checkUnitAbsence();
  private: SettingsUnit();
  private: void readSettingsFile();
  private: QString readSetting(const QString& settingTag);
  private: void openSettingsFile();
  private: QString tryToDetectSettingValueInFile(const QString& settingTag);
  private: QString detectSettingValueInFile(const QString& settingTag);
  private: QString detectSettingValueInString(const QString& settingTag, const QString& line) const;
  private: void closeSettingsFile();
  private: void removeCarriageReturnCharacter(QString& line) const;
  private: void removeSpaces(QString& line) const;
  public:  virtual ~SettingsUnit();
  
  public: QString getEmailLogin() const;

  public: QString getEmailIp() const;

  public: quint32 getEmailPort() const;

  public: QString getEmailPassword() const;

  public: QString getFtpLogin() const;

  public: QString getFtpIp() const;

  public: quint32 getFtpPort() const;

  public: QString getFtpPassword() const;

  public: QString getSqlLogin() const;

  public: QString getSqlIp() const;

  public: quint32 getSqlPort() const;

  public: QString getSqlPassword() const;
  
  public: QString getDefaultLogin() const;
};
#endif // SETTINGSUNIT

