/*! \file ftphardworker.h
 *  \brief Описание интерфейса класса FtpHardWorker
 *  \author Бубенщиков А.И.
 *  \date 23.12.2015
 */

#ifndef FTPHARDWORKER_H
#define FTPHARDWORKER_H

#include <QObject>
#include <QFile>
#include <QtFtp/qftp.h>
#include <QScopedPointer>
#include <QTimer>
#include <QEventLoop>
#include <QtCore>
#include <QProgressDialog>


/*! \class FtpHardWorker
 *  \brief Класс, обеспечивающий работу с FTP-сервером
 *
 *  Граничный класс, обеспечивающий взаимодействие с внешней системой
 *  FTP-сервером для хранения файлов вложений
 */
class FtpHardWorker : public QObject
{
  Q_OBJECT
  private:
    /// Конструктор по умолчанию
    explicit FtpHardWorker(QObject *parent = 0);

    /// Конструктор копирования
    explicit FtpHardWorker(const FtpHardWorker&);

    /// Оператор присваивания
    FtpHardWorker& operator = (FtpHardWorker&);

    /// Единственный экземпляр класса FtpHardWorker
    static QScopedPointer<FtpHardWorker> _instance;

    /// Таймаут соединения с FTP сервером
    static const quint32 _connectionTimeout = 500;

    ///количество попыток парирования в случае неуспешных попыток подключения к FTP серверу
    static const quint32 _trysQuantity = 4;

    /// Объект для работы с FTP сервером
    QScopedPointer<QFtp> _ftp;

    ///Папка с временными файлами на локальной машине
    QScopedPointer<QDir> _localTempFolder;

    /// Имя файла скачиваемого с сервера или загружаемого на сервер
    QScopedPointer<QFile> _file;

    /// Таймер для отсчета таймаута подключения к серверу
    QScopedPointer<QTimer> _timer;

    /// Список папок в текущем каталоге на FTP сервере
    QScopedPointer<QList<QString> > _folderList;

    /// Прогресс-бар для отображеня процесска скачки файла
    QScopedPointer<QProgressDialog> _progressDialog;

    /// Список файлов в текущем каталоге на FTP сервере
    QScopedPointer<QList<QString> > _filesList;

    /// Путь к локальному каталогу с временными файлами
    QScopedPointer<QString> _localTempPath;

    /// Флаг показывающий была ли отменена предыдущая загрузка
    bool _wasCancel;
    
    /// Реквизиты для соединения с FTP сервером
    QString _host;
    QString _login;
    QString _pswd;
    quint32 _port;

    /// Метод, настраивающий прогресс-бар для передачи файлов
    void progressDialogConfig();

    /*!
     * \brief removeLocalFolder Метод, удаляющий папку и все ее содержимое на
     *                          локальной машине
     * \param dir               Удаляемая папка
     */
    static void removeLocalFolder(const QDir &dir);

    /*!
     * \brief makeFtpFolder Метод создающий папку в текущей директории
     *                      FTP сервера
     * \param path          Имя создаваемой папки
     */
    void makeFtpFolder(const QString& path);

    /// Метод настройки и инициализации внутренних данных
    void init();

  public:

    /// Деструктор
    virtual ~FtpHardWorker();

    /// Метод, возвращающий указатель на экземпляр объекта
    static FtpHardWorker *getInstance();

    /*!
     * \brief getShortFileNames Метод преобразующий массив с полными именами
     *                          файлов в массив коротких имен
     * \param fullFileNames     Массив с полными именами файлов
     * \return                  Массив с короткими именами файлов
     */
    static QStringList getShortFileNames(QStringList fullFileNames);

    /*!
     * \brief getFile  Метод загружающий файл с FTP сервера
     * \param filename Путь к загружаемому файлу на сервере
     * \param dstPath  Путь к файлу на локальной машине, отностительно
     *                 каталога с временными файлами
     */
    //void getFile(const QString &filename, const QString &dstPath);

    /*!
     * \brief  downloadFileToTempDir Метод загружающий файл с FTP сервера
     *                               в каталог с временными файлами
     * \param  filename              Путь к загружаемому файлу на сервере
     * \param  dstPath               Путь к файлу на локальной машине, отностительно
     *                               каталога с временными файлами
     * \throws FtpException          В случае ошибки создания каталога с временными
     *                               файлами, ошибки создания файла на локальной
     *                               машине или ошибки загрузки файла
     * \throws ExecutionAborted      В случае, если закгрузка файла была отменена
     */
    void downloadFileToTempDir(const QString &filename, const QString &dstPath);

    /*!
     * \brief  downloadFile          Метод загружающий файл с FTP сервера
     * \param  filename              Путь к загружаемому файлу на сервере
     * \param  dstPath               Путь к файлу на локальной машине
     * \throws FtpException          В случае ошибки создания файла на локальной
     *                               машине или ошибки загрузки файла
     * \throws ExecutionAborted      В случае, если закгрузка файла была отменена
     */
    void downloadFile(const QString& filename, const QString& dstPath);

    /*!
     * \brief connectToFtpServer Метод, устанавливающий соединение с FTP сервером
     * \param host               Адрес FTP сервера
     * \param login              Имя пользователя для входа на FTP сервер
     * \param pswd               Пароль для входа на FTP сервер
     * \param port               Порт для подключения
     * \throws NeedFixCode       Если методу передан некорректный адрес сервера
     */
    void connectToFtpServer(QString host, QString login,
                            QString pswd, quint32 port = 21);

    /*!
     * \brief setLocalTempPath Метод установки пути к папке с
     *                         временными файлами на локальной машине
     * \param path             Путь к папке с временными файлами
     */
    void setLocalTempPath(QString path);

    /*!
     * \brief rename  Метод переименования файла или папки
     *                на сервере
     * \param oldName Старое имя файла или папки
     * \param newName Новое имя файла или папки
     * \throws FtpException в случае ошибки переименования
     */
    void rename(const QString& oldName,const QString& newName);

    /// Метод возвращающий путь к папке с временными файлами на локальной машине
    QString getLocalTempPath();

    /*!
     * \brief uploadFile       Метод загружающий файл на FTP сервер
     * \param localFilename    Путь к загружаемому файлу на локальной машине
     * \param filePathInServer Путь к файлу на локальной машине, отностительно
     *                         каталога с временными файлами
     */
    void uploadFile(const QString& localFilename,
                    const QString& filePathInServer);

    /*!
     * \brief getFoldersFromPath Метод для парсинга абсолютного пути
     * \param path               Абсолютный путь
     * \return                   Массив строк, в котором содержатся все
     *                           папки, содержищиеся в абсолютном пути
     */
    QStringList getFoldersFromPath(const QString& path);

    /*!
     * \brief removeFtpFolder Метод удаляющий папку со всем ее содержимым
     *                        с FTP сервера
     * \param path            Путь к удаляемой папке на сервере
     * \throws NeedFixCode    В случае если передана пустая строка в
     *                        качестве пути к папке на сервере
     */
    void removeFtpFolder(const QString& path);

    /*!
     * \brief removeTmpFolder Метод удаляющий папку с временными файлами
     *                        на локальной машине
     */
    void removeTmpFolder();

    /*!
     * \brief getEntryList Метод, заполняющий поля класса _filesList
     *                     и _folderList
     * \param path         Путь к папке
     */
    void getEntryList(QString path = QString());

    /*!
     * \brief existFtpFolder Метод, проверяющий создана ли на сервере
     *                       папка
     * \param path           Путь к проверяемой папке
     * \return               Истина, если папка существут, иначе ложь
     * \throws NeedFixCode   Если передана пустая строка в параметре
     */
    bool existFtpFolder(const QString& path);

    /*!
     * \brief copy         Метод копирующий содержимое папки на FTP сервере
     *                     в новую папку на FTP сервере
     *
     *                     Метод копирует только файлы содержащиеся в
     *                     папке и не копирует имеющиеся подпапки
     * \param srcPath      Путь к копируемой папке на FTP сервере
     * \param dstPath      Путь к рузультирующей папке на FTP сервере
     * \throws NeedFixCode В случае возникновении ошибки при копировании
     */
    void copy(const QString& srcPath, const QString& dstPath);

    /*!
     * \brief getShortFileName Метод, возращающий короткое имя файла
     * \param fullName         Полное имя файла
     * \return                 Короткое имя файла
     */
    static QString getShortFileName(QString fullName);

    /*!
     * \brief disconnect Метод закрывающий соединение с FTP сервером
     */
    void disconnect();
    
    /*!
     * \brief rememberConnectionSettings Метод, запоминающий реквизиты для соединения с FTP сервером
     * \param host               Адрес FTP сервера
     * \param login              Имя пользователя для входа на FTP сервер
     * \param pswd               Пароль для входа на FTP сервер
     * \param port               Порт для подключения
     */
    void rememberConnectionSettings(QString& host, QString& login, QString& pswd, quint32& port);

  signals:
    /// Сигнал, высылаемый по окончанию загрузки файла
    void downloadFileFinished();
    /// Сигнал, высылаемый по окончанию передачи файла на сервер
    void uploadFileFinished();
    /// Сигнал о том, что получение списка элементов в папке завершено
    void getListFinished();
    /// Сигнал, высылаемый по окончанию операции удаления папки с сервера
    void removeFolderFinished();
    /// Сигнал, высылаемый по окончанию операции удаления файла с сервера
    void removeFileFinished();
    /// Сигнал, высылаемый по окончанию создания каталога на сервере
    void makeFolderFinished();
    /// Сигнал, высылаемый по окончанию переименования папки или файла на сервере
    void renamefinished();

  private slots:
    /// Обработчик окончания выполнения команды FTP сервером
    void ftpCommandFinished(int, bool);
    /// Обработчик операции list()
    void listInformation(QUrlInfo url);
    /// Обработчик изменяющий состояние прогресс-бара передачи файла
    void updateDataTransferProgress(qint64 readBytes,qint64 totalBytes);

    void cancelDownload();
};

#endif // FILESYSTEMHARDWORKER_H
