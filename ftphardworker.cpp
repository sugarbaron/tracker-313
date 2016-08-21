#include "ftphardworker.h"
#include "exception.h"
#include "QMessageBox"
#include "General.h"
#include "exception.h"

FtpHardWorker::FtpHardWorker(QObject *parent)
  : QObject(parent),_ftp(new QFtp),_localTempFolder(new QDir),_file(new QFile),
    _timer(new QTimer), _folderList(new QList<QString>),
    _progressDialog(new QProgressDialog),_filesList(new QList<QString>),_localTempPath(new QString),
    _wasCancel(false),  _host(), _login(), _pswd(), _port(21)
{
  init();
}

void FtpHardWorker::init()
{
  // Подключаем сигнал завершения выполнения команды к обработчику
  connect(_ftp.data(),SIGNAL(commandFinished(int,bool)),this,SLOT(ftpCommandFinished(int,bool)));

  // Подключаем сигнал запроса содержимого каталога на сервере к обрабочику
  connect(_ftp.data(),SIGNAL(listInfo(QUrlInfo)),this,SLOT(listInformation(QUrlInfo)));

  // Настраиваем програсс-бар для передачи файлов
  progressDialogConfig();

  // Задаем путь к каталогу для временных файлов
  setLocalTempPath(qApp->applicationDirPath()+ "/temp/");
}

void FtpHardWorker::progressDialogConfig()
{
  // Устанавливаем заголовок прогресс-бара
  _progressDialog->setWindowTitle(tr("Передача файла"));

  // Делаем прогресс-бар модальным
  _progressDialog->setWindowModality(Qt::ApplicationModal);

  // Подключаем сигнал изменения количества скаченных данных к слоту отображения процесса скачки
  connect(_ftp.data(),SIGNAL(dataTransferProgress(qint64,qint64)),this,SLOT(updateDataTransferProgress(qint64,qint64)));

  // Подключаем обработчик кнопки "Отмена"
  connect(_progressDialog.data(), SIGNAL(canceled()), this, SLOT(cancelDownload()));

  // Обнуляем и скрываем прогресс-бар по окончанию загрузки файла
  connect(_ftp.data(),SIGNAL(done(bool)),_progressDialog.data(),SLOT(reset()));
  connect(_ftp.data(),SIGNAL(done(bool)),_progressDialog.data(),SLOT(hide()));
}

QString FtpHardWorker::getShortFileName(QString fullName)
{
  QString shortName;
  qint32 pos = fullName.lastIndexOf("/");
  if((pos > 0)&&(pos < fullName.size()))
  {
    shortName = fullName.remove(0,++pos);
  }
  else
  {
    shortName = fullName;
  }
  return shortName;
}

QStringList FtpHardWorker::getShortFileNames(QStringList fullFileNames)
{
  QStringList shortNames;
  foreach(QString name, fullFileNames)
  {
    shortNames.push_back(getShortFileName(name));
  }
  return shortNames;
}

void FtpHardWorker::connectToFtpServer(QString host, QString login, QString pswd, quint32 port)
{
  QUrl url(host);

  // Проверяем корректность адреса сервера
  if ((!url.isValid()) || (url.scheme().toLower() != "ftp"))
  {
    QString msg = "FTP URL is not valid";
    qCritical()<<msg;
    throw NeedFixCode(msg);
  }
  
  Clock* systemClock = General::getSystemClock();
  // Делаем несколько попыток подключения
  for(quint32 i = 1; i <= _trysQuantity; i++)
  {
    // Если не подключены к серверу, пытаемся подключиться
    if(_ftp->state()!= QFtp::LoggedIn)
    {
      disconnect();
      // Переинициализируем ресурс(костыль для корректной работы QFtp)
      _ftp.reset(new QFtp);
      init();
      _ftp->connectToHost(url.host(), url.port(port));
      _ftp->login(login,pswd);
      // Ставим основной поток на ожидание т.к. команды FTP выполняются ассинхронно
      systemClock->msThreadSleep(_connectionTimeout);
    }
    else
    {
      return;
    }
    // Проверяем удалось ли подулючиться
    if(_ftp->state()!= QFtp::LoggedIn)
    {
      _ftp->abort();
      QString msg = tr("Unable to connect to the FTP server, error string = ")
                  + _ftp->errorString();
      qCritical()<<msg;
      // Если ичерпаны все попытки парирования, кидаем исключение
      if(_trysQuantity == i)
      {
        throw FtpException(msg);
      }
    }
    else
    {
      qDebug()<<tr("Connected succ to FTP server");
      return;
    }
  }
}

void FtpHardWorker::setLocalTempPath(QString path)
{
  *(_localTempPath.data()) = path;
  _localTempFolder->setPath(*(_localTempPath.data()));
}

QString FtpHardWorker::getLocalTempPath()
{
  return *(_localTempPath.data());
}

void FtpHardWorker::uploadFile(const QString& localFilename,
                               const QString& filePathInServer)
{
  // Костыль для корректной работы QFtp в случае, если
  // предыдущая скачка была отменена
  if(_wasCancel)
  {
    Clock* systemClock = General::getSystemClock();
    _ftp->close();
    systemClock->msThreadSleep(50);
    _wasCancel = false;
  }
  connectToFtpServer(_host,  _login, _pswd, _port);
  // Открываем файл на локальной машине, который нужно загрузить на сервер
  _file.reset(new QFile);
  _file->setFileName(localFilename);
  if(!_file->open(QIODevice::ReadOnly))
  {
    QString msg = tr("Can't open file, filename = ") + localFilename;
    qCritical()<<msg;
    throw FtpException(msg);
  }
  else
  {
    // Создадим директорию назначения на сервере
    makeFtpFolder(filePathInServer);
    // Отрезаем имя файла от его полного имени
    QString shortFileName = getShortFileName(localFilename);
    // Отправляем команду на ассинхронную загрузку файла
    _ftp->put(_file.data(),filePathInServer + shortFileName);
    // Отображаем прогресс-бар закачки файла
    _progressDialog->setLabelText(tr("Загружаем %1...").arg(getShortFileName(shortFileName)));
    _progressDialog->exec();
    // Закрываем файл
    _file->close();
    // Если была отмена скачики файла
    if(_wasCancel)
    {
      throw ExecutionAborted();
    }
    // Если загрузить файл не удалось
    if(_ftp->error())
    {
      QString msg = tr("Can't upload file ") + _file->fileName() + tr(" to FTP server, error string = ")
                  + _ftp->errorString();
      qCritical()<<msg;
      throw FtpException(msg);
    }
  }
}

void FtpHardWorker::downloadFile(const QString& filename,
                                 const QString& dstPath)
{
  // Костыль для корректной работы QFtp в случае, если
  // предыдущая скачка была отменена
  if(_wasCancel)
  {
    Clock* systemClock = General::getSystemClock();
    _ftp->close();
    systemClock->msThreadSleep(50);
    _wasCancel = false;
  }

  connectToFtpServer(_host,  _login, _pswd, _port);
  QString shortFileName = getShortFileName(filename);
  // Создадим файл на локальной машине
  _file.reset(new QFile);
  _file->setFileName(dstPath + "/" + shortFileName);
  if(!_file->open(QIODevice::WriteOnly))
  {
    QString msg = tr("Can't create local file");
    qCritical()<<msg;
    throw FtpException(msg);
  }
  // Отправляем команду на ассинхронную загрузку файла
  _ftp->get(filename,_file.data());
  // Отображаем прогресс-бар закачки файла
  _progressDialog->setLabelText(tr("Скачиваем %1...").arg(getShortFileName(filename)));
  _progressDialog->exec();

  // Закрываем файл
  _file->close();

  // Если была отмена скачики файла
  if(_wasCancel)
  {
    _file->remove();
    throw ExecutionAborted();
  }

  // Если скачать файл не удалось
  if(_ftp->error())
  {
    QString msg = tr("Can't dowload file from FTP server, error string = ")
                + _ftp->errorString();
    qCritical()<<msg;
    _file->remove();
    throw FtpException();
  }

  qDebug()<<tr("Download")<<_file->fileName()<<tr("done");
}


void FtpHardWorker::downloadFileToTempDir(const QString& filename,
                                          const QString& dstPath)
{
  // Создадим каталог на локальной машине
  if(!_localTempFolder->mkpath(_localTempFolder->path() + "/" + dstPath + "/"))
  {
    QString msg = tr("Can't create tmp folder");
    qCritical()<<msg;
    throw FtpException(msg);
  }

  QString tmpPath = _localTempFolder->path() + "/" + dstPath + "/";
  downloadFile(filename, tmpPath);
}

void FtpHardWorker::ftpCommandFinished(int, bool)
{
  if (_ftp->currentCommand() == QFtp::Mkdir)
  {
    emit(makeFolderFinished());
  }

  if (_ftp->currentCommand() == QFtp::Rmdir)
  {
    emit(removeFolderFinished());
  }

  if (_ftp->currentCommand() == QFtp::Remove)
  {
    emit(removeFileFinished());
  }

  if(_ftp->currentCommand() == QFtp::List)
  {
    emit(getListFinished());
  }

  if(_ftp->currentCommand() == QFtp::Rename)
  {
    emit(renamefinished());
  }
}

void FtpHardWorker::listInformation(QUrlInfo url)
{
  if(url.isDir())
  {
    _folderList->push_back(url.name());
  }
  if(url.isFile())
  {
    _filesList->push_back(url.name());
  }
}

QStringList FtpHardWorker::getFoldersFromPath(const QString& path)
{
  QStringList folders;
  qint32 pos = 0;
  qint32 newPos = 0;
  if((!path.isEmpty())&&(path.at(0) == '/'))
  {
    ++pos;
  }
  while(pos < path.size())
  {
    newPos = path.indexOf("/",pos);
    if(newPos == -1)
    {
      folders.push_back(path.mid(pos,newPos));
      qDebug()<<path.mid(pos,newPos);
      return folders;
    }
    folders.push_back(path.mid(pos,(newPos-pos)));
    //qDebug()<<path.mid(pos,(newPos-pos));
    pos = newPos;
    ++pos;
  }
  return folders;
}

void FtpHardWorker::updateDataTransferProgress(qint64 readBytes,qint64 totalBytes)
{
  _progressDialog->setMaximum(totalBytes);
  _progressDialog->setValue(readBytes);
}

void FtpHardWorker::getEntryList(QString path)
{
  connectToFtpServer(_host,  _login, _pswd, _port);
  QEventLoop loop;
  connect(this,SIGNAL(getListFinished()),&loop,SLOT(quit()));
  _ftp->list(path);
  loop.exec();
  int errorCode = _ftp->error();
  if(errorCode)
  {
    qCritical()<<"for path #"<<path;
    QString msg = tr("Get folder list error: ")
                + _ftp->errorString();
    qCritical()<<msg;
    qCritical("error code #%i", errorCode);
    throw FtpException(msg);
  }
  return;
}

void FtpHardWorker::makeFtpFolder(const QString& path)
{
  connectToFtpServer(_host,  _login, _pswd, _port);
  QEventLoop loop;
  connect(this,SIGNAL(makeFolderFinished()),&loop,SLOT(quit()));
  QStringList folders = getFoldersFromPath(path);
  getEntryList();
  foreach(QString name, folders)
  {
    if(!_folderList->contains(name))
    {
      _ftp->mkdir(name);
      loop.exec();
      if(_ftp->error())
      {
        QString msg = tr("Create folder ")
                    + name
                    + tr(" error, errorstring = ")
                    + _ftp->errorString();
        qCritical()<<msg;
        // Возвращаемся в каталог самого верхнего уровня
        for(int i = 0; i < folders.size(); ++i)
        {_ftp->cd("../..");}
        _folderList->clear();
        _filesList->clear();
        throw FtpException(msg);
      }
    }
    _ftp->cd(name);
    _folderList->clear();
    _filesList->clear();
    getEntryList();
  }

  // Возвращаемся в каталог самого верхнего уровня
  for(int i = 0; i < folders.size(); ++i)
  {_ftp->cd("../..");}
  _folderList->clear();
  _filesList->clear();
}

bool FtpHardWorker::existFtpFolder(const QString& path)
{
  if(path.isEmpty())
  {
    QString msg = tr("wrong param, path is empty");
    qCritical()<<msg;
    throw NeedFixCode(msg);
  }
  try{getEntryList(path);}
  catch(FtpException){return false;}
  return true;
}

void FtpHardWorker::rename(const QString& oldName,const QString& newName)
{
  connectToFtpServer(_host,  _login, _pswd, _port);
  if(oldName.isEmpty()||newName.isEmpty())
  {
    QString msg = tr("wrong param, name is empty");
    qCritical()<<msg;
    throw NeedFixCode(msg);
  }
  QEventLoop loop;
  connect(this,SIGNAL(renamefinished()),&loop,SLOT(quit()));
  _ftp->rename(oldName,newName);
  loop.exec();
  if(_ftp->error())
  {
    QString msg = tr("Can't rename, error string = ")
                + _ftp->errorString();
    qCritical()<<msg;
    throw FtpException(msg);
  }
}

void FtpHardWorker::copy(const QString& srcPath, const QString& dstPath)
{
  qDebug()<<"srcPath = "<<srcPath<<", dstPath = "<<dstPath;
  if(srcPath.isEmpty()||dstPath.isEmpty())
  {
    QString msg = tr("wrong param, path is empty, srcPath = %1, dstPath = %2")
                  .arg(srcPath,dstPath);
    qCritical()<<msg;
    throw NeedFixCode(msg);
  }
  _folderList->clear();
  _filesList->clear();
  // Получаем список файлов в исходной директории
  getEntryList(srcPath);

  // Скачиваем файлы и загружаем на сервер в другую папку
  foreach(QString file, *(_filesList.data()))
  {
    downloadFileToTempDir(srcPath + file,"tmp/");
    uploadFile(_localTempFolder->absolutePath() + "/tmp/" + file,dstPath);
  }
  // Удаляем временную папку на локальной машине
  QDir dir;
  dir.setPath(_localTempFolder->absolutePath() + "/tmp/");
  removeLocalFolder(dir);
  _folderList->clear();
  _filesList->clear();
}

void FtpHardWorker::removeFtpFolder(const QString& path)
{
  try
  {
    connectToFtpServer(_host,  _login, _pswd, _port);
  }
  catch(FtpException){return;}

  if(path.isEmpty())
  {
    QString msg = tr("wrong param, path is empty");
    qCritical()<<msg;
    throw NeedFixCode(msg);
  }
  // Получаем список файлов и папок в каталоге, который нужно удалить
  try{getEntryList(path);}
  catch(FtpException){return;}

  // Копируем данные в локальные переменные(иначе рекурсия не будет работать)
  QList<QString> localFolderList = *(_folderList.data());
  QList<QString> localFilesList = *(_filesList.data());

  // Очищаем список файлов и папок
  _filesList->clear();
  _folderList->clear();

  // Удаляем файлы
  QEventLoop loop;
  connect(this,SIGNAL(removeFileFinished()),&loop,SLOT(quit()));
  foreach(QString file,localFilesList)
  {
    _ftp->remove(path + file);
    loop.exec();
    int errorCode = _ftp->error();
    if(errorCode)
    {
      QString msg = tr("Can't remove file from FTP server, error string = ")
                  + _ftp->errorString();
      qCritical()<<msg;
      qCritical()<<"error code #"<<QString::number(errorCode);
    }
  }

  // Удаляем папки
  foreach(QString dir,localFolderList)
  {
    removeFtpFolder(path + "/" + dir + "/");
  }

  // Удаляем папку верхнего уровня
  QObject::disconnect(this,SIGNAL(removeFileFinished()),&loop,SLOT(quit()));
  connect(this,SIGNAL(removeFolderFinished()),&loop,SLOT(quit()));
  _ftp->rmdir(path);
  loop.exec();
  int errorCode = _ftp->error();
  if(errorCode)
  {
    QString msg = tr("Can't remove folder from FTP server, error string = ")
                + _ftp->errorString();
    qCritical()<<msg;
    qCritical()<<"error code #"<<QString::number(errorCode);
  }
}

void FtpHardWorker::removeLocalFolder(const QDir& dir)
{
  // Получаем список каталогов
  QStringList lstDirs = dir.entryList(QDir::Dirs
                                     |QDir::AllDirs
                                     |QDir::NoDotAndDotDot);

  // Получаем список файлов
  QStringList lstFiles = dir.entryList(QDir::Files);

  // Удаляем файлы
  foreach (QString entry, lstFiles)
  {
    QString entryAbsPath = dir.absolutePath()
                         + "/" + entry;
    QFile::setPermissions(entryAbsPath,QFile::ReadOwner | QFile::WriteOwner);
    QFile::remove(entryAbsPath);
  }

  // Удаляем папки
  foreach (QString entry, lstDirs)
  {
    QString entryAbsPath = dir.absolutePath()
                         + "/" + entry;
    QDir dr(entryAbsPath);
    removeLocalFolder(dr);
  }

  // Удаляем папку верхнего уровня
  QDir().rmdir(dir.absolutePath());
}

FtpHardWorker* FtpHardWorker::getInstance()
{
  if(_instance.isNull())
  {
    _instance.reset(new FtpHardWorker);
  }
  return _instance.data();
}

void FtpHardWorker::disconnect()
{
  _folderList->clear();
  _filesList->clear();

  if(_ftp->state() == QFtp::LoggedIn)
  {
    // Прерываем текущую загрузку
    _ftp->abort();
    _ftp->close();
    qDebug()<<QObject::tr("disconnected from FTP server");
  }

  // Закрываем файл
  _file->close();
}

void FtpHardWorker::removeTmpFolder()
{
  // Закрываем файл
  _file->close();

  // Удаляем каталог с временными файлами
  if((!(_localTempPath->isEmpty()))&&(_localTempFolder->exists()))
  {
    removeLocalFolder(*(_localTempFolder.data()));
  }
}

void FtpHardWorker::cancelDownload()
{
  // Стопэ, стопэ, отмэна отмэна
  _ftp->abort();
  _wasCancel = true;
  qDebug()<<"load aborted";
}

void FtpHardWorker::rememberConnectionSettings(QString& host, QString& login, QString& pswd, quint32& port)
{
  _host  = host;
  _login = login;
  _pswd  = pswd;
  _port  = port;
  return;
}

FtpHardWorker::~FtpHardWorker()
{}

QScopedPointer<FtpHardWorker> FtpHardWorker::_instance;
