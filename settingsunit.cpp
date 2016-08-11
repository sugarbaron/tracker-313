#include "settingsunit.h"
#include "exception.h"

SettingsUnit* SettingsUnit::create()
{
  checkUnitAbsence();
  _thisUnit.reset( new SettingsUnit() );
  _thisUnit->readSettingsFile();
  return _thisUnit.data();
}

void SettingsUnit::checkUnitAbsence()
{
  bool isAlreadyCreated = !( _thisUnit.isNull() );
  if(isAlreadyCreated)
  {
    QString errorText = "this method must be invoked only once";
    qCritical()<<errorText;
    throw NeedFixCode(errorText);
  }
  return;
}

SettingsUnit::SettingsUnit():
_settingsFile("settings.txt"),
_emailLogin(),
_emailIp(),
_emailPort(0),
_emailPassword(),
_ftpLogin(),
_ftpIp(),
_ftpPort(0),
_ftpPassword(),
_sqlLogin(),
_sqlIp(),
_sqlPort(0),
_sqlPassword(),
_defaultLogin()
{}

void SettingsUnit::readSettingsFile()
{
  _ftpIp = "ftp://" + readSetting(FTP_IP_TAG);
  _sqlIp = readSetting(SQL_IP_TAG);
  _emailIp  = readSetting(EMAIL_IP_TAG);
  _ftpLogin = readSetting(FTP_LOGIN_TAG);
  _sqlLogin = readSetting(SQL_LOGIN_TAG);
  _emailLogin  = readSetting(EMAIL_LOGIN_TAG);
  _ftpPassword = readSetting(FTP_PASSWORD_TAG);
  _sqlPassword = readSetting(SQL_PASSWORD_TAG);
  _emailPassword = readSetting(EMAIL_PASSWORD_TAG);
  QString ftpPortAsText = readSetting(FTP_PORT_TAG);
  QString sqlPortAsText = readSetting(SQL_PORT_TAG);
  QString emailPortAsText = readSetting(EMAIL_PORT_TAG);
  _ftpPort = ftpPortAsText.toUInt();
  _sqlPort = sqlPortAsText.toUInt();
  _emailPort = emailPortAsText.toUInt();
  
  try
  {
    _defaultLogin = readSetting(DEFAULT_LOGIN_TAG);
  }
  catch(SettingsFileException){/* допустимо отсутствие настройки "логин по умолчанию" */}
  return;
}

QString SettingsUnit::readSetting(const QString& settingTag)
{
  openSettingsFile();
  QString settingValue = tryToDetectSettingValueInFile(settingTag);
  closeSettingsFile();
  
  removeCarriageReturnCharacter(settingValue);
  removeSpaces(settingValue);
  return settingValue;
}

void SettingsUnit::openSettingsFile()
{
  bool isOk = false;
  isOk = _settingsFile.open(QIODevice::ReadOnly|QIODevice::Text);
  if(isOk == false)
  {
    QString errorText = "error opening file #settings.txt";
    qCritical()<<errorText;
    throw SettingsFileException(errorText);
  }
  return;
}

QString SettingsUnit::tryToDetectSettingValueInFile(const QString& settingTag)
{
  QString settingValue;
  try
  {
    settingValue = detectSettingValueInFile(settingTag);
  }
  catch(...)
  {
    closeSettingsFile();
    throw;
  }
  return settingValue;
}

QString SettingsUnit::detectSettingValueInFile(const QString& settingTag)
{
  QString settingValue;
  QString currentLine;
  bool isTagDetected = false;
  while( !(_settingsFile.atEnd()) )
  {
    currentLine  = _settingsFile.readLine();
    settingValue = detectSettingValueInString(settingTag, currentLine);
    isTagDetected = !( settingValue.isEmpty() );
    if(isTagDetected)
    {
      break;
    }
  }
  
  if(isTagDetected == false)
  {
    QString errorText = "setting tag #" + settingTag + " is not found in #settings.txt";
    qCritical()<<errorText;
    throw SettingsFileException(errorText);
  }
  return settingValue;
}

/* найти в строке параметр по тегу и вернуть значение этого параметра. 
 * (под значением понимается часть строки между тегом и концом строки) */
QString SettingsUnit::detectSettingValueInString(const QString& settingTag, const QString& string) const
{
  QString settingValue;
  QStringList stringParts = string.split(settingTag);
  bool isTagDetected = (stringParts.size() > 1);
  if(isTagDetected)
  {
    settingValue = stringParts.takeLast();
  }
  return settingValue;
}

void SettingsUnit::closeSettingsFile()
{
  _settingsFile.close();
  return;
}

void SettingsUnit::removeCarriageReturnCharacter(QString& line) const
{
  const quint32 LAST_CHARACTER_INDEX = line.size() - 1;
  line.remove(LAST_CHARACTER_INDEX, 1);
  return;
}

void SettingsUnit::removeSpaces(QString& line) const
{
  QStringList lineParts = line.split(" ", QString::SkipEmptyParts);
  bool thereWereOnlySpaces = lineParts.isEmpty();
  if(thereWereOnlySpaces)
  {
    QString errorText = "setting value is not found in #settings.txt";
    qCritical()<<errorText;
    throw SettingsFileException(errorText);
  }
  line = lineParts.takeLast();
  return;
}

SettingsUnit::~SettingsUnit()
{}

QString SettingsUnit::getEmailLogin() const
{
  return _emailLogin;
}

QString SettingsUnit::getEmailIp() const
{
  return _emailIp;
}

quint32 SettingsUnit::getEmailPort() const
{
  return _emailPort;
}

QString SettingsUnit::getEmailPassword() const
{
  return _emailPassword;
}

QString SettingsUnit::getFtpLogin() const
{
  return _ftpLogin;
}

QString SettingsUnit::getFtpIp() const
{
  return _ftpIp;
}

quint32 SettingsUnit::getFtpPort() const
{
  return _ftpPort;
}

QString SettingsUnit::getFtpPassword() const
{
  return _ftpPassword;
}

QString SettingsUnit::getSqlLogin() const
{
  return _sqlLogin;
}

QString SettingsUnit::getSqlIp() const
{
  return _sqlIp;
}

quint32 SettingsUnit::getSqlPort() const
{
  return _sqlPort;
}

QString SettingsUnit::getSqlPassword() const
{
  return _sqlPassword;
}

QString SettingsUnit::getDefaultLogin() const
{
  return _defaultLogin;
}

//секция_определения_статических_членов_класса_________________________________
///////////////////////////////////////////////////////////////////////////////
QScopedPointer<SettingsUnit> SettingsUnit::_thisUnit;
const QString SettingsUnit::FTP_IP_TAG = "ftp_ip:";
const QString SettingsUnit::SQL_IP_TAG = "sql_ip:";
const QString SettingsUnit::EMAIL_IP_TAG = "email_ip:";
const QString SettingsUnit::SQL_PORT_TAG = "sql_port:";
const QString SettingsUnit::FTP_PORT_TAG = "ftp_port:";
const QString SettingsUnit::EMAIL_PORT_TAG = "email_port:";
const QString SettingsUnit::FTP_LOGIN_TAG  = "ftp_login:";
const QString SettingsUnit::SQL_LOGIN_TAG  = "sql_login:";
const QString SettingsUnit::EMAIL_LOGIN_TAG  = "email_login:";
const QString SettingsUnit::FTP_PASSWORD_TAG = "ftp_password:";
const QString SettingsUnit::SQL_PASSWORD_TAG = "sql_password:";
const QString SettingsUnit::EMAIL_PASSWORD_TAG = "email_password:";
const QString SettingsUnit::DEFAULT_LOGIN_TAG  = "default_login:";
