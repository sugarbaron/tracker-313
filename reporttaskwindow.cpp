#include "reporttaskwindow.h"
#include "worker.h"
#include "order.h"
#include "project.h"
#include "exception.h"
#include "general.h"
#include "QDesktopServices"
#include "email.h"
#include "smtphardworker.h"
#include <QMessageBox>

ReportTaskWindow::ReportTaskWindow(QWidget *parent) :
  QWidget(parent),ui(new Ui::ReportTaskWindow),_reportFiles(new QList<QString>),
  _idReport(0),_windowState(createReport)
{
  ui->setupUi(this);

  // Подключаем сигнал изменения состояния окна к обработчику
  connect(this,SIGNAL(windowStateChanged()),this,SLOT(windowStateChangedHnd()));
}

ReportTaskWindow::~ReportTaskWindow()
{
  delete ui;
}

void ReportTaskWindow::closeEvent(QCloseEvent* event)
{
  emit(closeSignal());
  QWidget::closeEvent(event);
}

void ReportTaskWindow::infoRefreshFromTask(const Task& task)
{
  if(!task.isValid())
  {
    QString msg = "task is not valid";
    qCritical()<<msg;
    throw NeedFixCode(msg);
  }

  // Очищаем поля окна
  ui->reportTxtEd->clear();
  ui->selectedFilesLst->clear();

  // Очищаем поле с файлами вложения
  _reportFiles->clear();

  // Отображаем суть поручения
  ui->descriptionTxtEd->setPlainText(task.getDescription());

  // Отображаем комментарий к поручению
  ui->commentTxtEd->setPlainText(task.getComment());

  // Отображаем дату контроля поручения
  ui->deadLineDatEd->setDate(task.getDeadlineDate());

  // Отображаем важность поручения
  ui->importanceLbl->setText(QString::number(task.getImportance()));

  // Отображаем автора поручения
  ui->authorLbl->setText(task.getAuthorDb().getFullName());

  // Проект
  try{ui->projectLbl->setText(task.getProjectDb().getName());}
  catch(ExecutionAborted)
  {
    ui->projectLbl->clear();
  }

  // Заказ
  try{ui->ordersLbl->setText(task.getOrderDb().getName());}
  catch(ExecutionAborted)
  {
    ui->ordersLbl->clear();
  }
}

void ReportTaskWindow::infoRefreshFromTaskAndReport(const Task& task,
                                                    const Report& report)
{
  // Проверяем валидность переданного отчета
  if(!report.isValid())
  {
    QString msg = "report is not valid";
    qCritical()<<msg;
    throw NeedFixCode(msg);
  }

  // Запоминаем id отображаемого отчета
  _idReport = report.getId();

  // Отображаем информацию о поручении
  infoRefreshFromTask(task);

  // Очищаем поле с комментарием к поручению
  ui->commentTxtEd->clear();

  // Отображаем текст отчета
  ui->reportTxtEd->setPlainText(report.getText());

  // Отображаем файлы отчета
  *(_reportFiles.data()) = report.getFiles();
  ui->selectedFilesLst->clear();
  ui->selectedFilesLst->addItems(FtpHardWorker::getShortFileNames(*(_reportFiles.data())));
}

void ReportTaskWindow::fillReport(Report& report)
{
  // дата создания
  Clock* systemClock = General::getSystemClock();
  QDateTime now = systemClock->getTime();
  QDate today = now.date();
  report.setCreationDate(today);
  // текст отчета
  report.setText(ui->reportTxtEd->toPlainText());
  // файлы вложения
  report.setFiles(*(_reportFiles.data()));
}

void ReportTaskWindow::updateReportedTask(quint32 statusTask, Task& task)
{
  // Флаг, показывающий нужно ли отчитаться в головном поручении
  bool needReportToParentTask = false;

  // Если пытаются отклонить отчет, не указав прчину в комментарии
  if((ui->commentTxtEd->toPlainText().isEmpty()&&(Task::updated == statusTask)))
  {
    QMessageBox::information(0, tr("ВНИМАНИЕ"),tr("Необходимо указать причину отклонения отчета "
                                                  "в поле \"Комментарий\""), QMessageBox::Ok);
    throw ExecutionAborted("report is decline, but comment is empty");
  }

  Task parentTask;
  // Если отчитываемся по перепорученному поручению
  if((Task::complete == statusTask)&&(task.getParent()) > 0)
  {
    QMessageBox msgBox;
    msgBox.setWindowTitle(tr("Отчитаться"));
    msgBox.setText(tr("Это перепорученное поручение. Отчитаться в головном?"));
    QPushButton* yesButton = msgBox.addButton(tr("Да"),QMessageBox::ActionRole);
    msgBox.addButton(tr("Нет"),QMessageBox::ActionRole);
    msgBox.exec();
    if(msgBox.clickedButton() == yesButton)
    {
      needReportToParentTask = true;
      parentTask.synhronizeWithDb(task.getParent());
    }
  }

  task.setStatus(statusTask);
  parentTask.setStatus(Task::isBidForReport);
  parentTask.setReportId(task.getReportId());

  // Коментарий
  if(!ui->commentTxtEd->toPlainText().isEmpty())
  {
    QString comment;
    Clock* systemClock = General::getSystemClock();
    QDateTime date = systemClock->getTime();

    // Достаем из БД текущего пользователя
    Worker currentUser;
    currentUser.synhronizeWithDb(General::getRegistredUserId());
    // Достаем из БД ответственного за поручение
    Worker responsible = task.getResponsibleDb();
    // Достаем из БД отчет по поручению
    Report report = task.getReportDb();

    if(!task.getComment().isEmpty())
    {
      comment = task.getComment() + "\n";
    }
    // Вставляем комментарий принимающего поручение
    comment += "[" + date.toString("dd.MM.yy hh:mm:ss")+ " "
                                + currentUser.getShortName() + "]: ";
    comment += ui->commentTxtEd->toPlainText();
    task.setComment(comment);

    if(!parentTask.getComment().isEmpty())
    {
      comment = parentTask.getComment() + "\n";
    }
    // Вставляем в комментарий текст отчета по поручению в родительское поручение
    comment += "[" + date.toString("dd.MM.yy hh:mm:ss")+ " "
                                + responsible.getShortName() + "]отчет: ";
    comment += report.getText() + "\n";

    // Вставляем комментарий принимающего поручение в родительское поручение
    comment += "[" + date.toString("dd.MM.yy hh:mm:ss")+ " "
                                + currentUser.getShortName() + "]: ";
    comment += ui->commentTxtEd->toPlainText();
    parentTask.setComment(comment);
  }
  SqlHardWorker* sqlHardWorker = General::getSqlHardWorker();
  sqlHardWorker->updateSqlTaskData(task);
  if(needReportToParentTask)
  {
    sqlHardWorker->updateSqlTaskData(parentTask);
    // Отправим уведомление о создании отчета по поручению автору
    QString subjectMsg = "Получен отчет по поручению";
    QString textMsg = "Суть поручения: " + parentTask.getDescription();
    Email email(parentTask.getAuthorDb().getEmail(),subjectMsg,textMsg);
    SmtpHardWorker* smtp = SmtpHardWorker::getInstance();
    smtp->sendEmail(email);
  }
}

QList<QString> ReportTaskWindow::getReportFiles()
{
  return *(_reportFiles.data());
}

void ReportTaskWindow::addReportFile(const QString& fileName)
{
  _reportFiles->push_back(fileName);
}

void ReportTaskWindow::loadSeletedFile(QListWidgetItem* item)
{
  // Конструируем путь к файлу
  QString path = "Reports/id_" + QString::number(_idReport) + "/" + item->text();

  // Конструируем путь на локальной машине
  QString localPath = "Reports/id_"
                    + QString::number(_idReport)
                    + "/";

  // Скачиваем файл с FTP сервера
  try
  {
    FtpHardWorker* ftp = General::getFtpHardWorker();
    ftp->downloadFileToTempDir(path,localPath);
    // Открываем файл с помощью оболочки ОС
    if(!QDesktopServices::openUrl(QUrl::fromLocalFile(ftp->getLocalTempPath()
                                                      + "/"
                                                      + localPath
                                                      + item->text())))
    {
      qCritical()<<tr("Can't open local file");
      QMessageBox::critical(this, tr("ОШБИКА"),tr("Ошибка открытия файла"), QMessageBox::Ok);
    }
  }
  catch(FtpException& ex)
  {
    qCritical()<<ex.getInfo();
    QMessageBox::critical(0, tr("ОШБИКА"),tr("Не удалось скачать файл"), QMessageBox::Ok);
  }
  catch(ExecutionAborted){}
}

void ReportTaskWindow::setWindowState(quint32 state)
{
  if(state > viewReport)
  {
    QString msg = "invalid argument, state = "
                + QString::number(state);
    qCritical()<<msg;
    throw NeedFixCode(msg);
  }
  _windowState = state;
  emit(windowStateChanged());
}

quint32 ReportTaskWindow::getWindowState() const
{
  return _windowState;
}

void ReportTaskWindow::windowStateChangedHnd()
{
  if(viewReport == _windowState)
  {
    // Подключаем обработчик скачивания и открытия файлов вложения по двойном щелчку
    connect(ui->selectedFilesLst,SIGNAL(itemDoubleClicked(QListWidgetItem*)),this,SLOT(loadSeletedFile(QListWidgetItem*)));
    // Отключаем обработчик удаления файлов вложения по нажатию кнопок del и backspace
    disconnect(this,SIGNAL(keyPressedSignal(qint32)),this,SLOT(deleteSelectedFilesHnd(qint32)));
  }
  else
  {
    // Отключаем обработчик скачивания и открытия файлов вложения по двойном щелчку
    disconnect(ui->selectedFilesLst,SIGNAL(itemDoubleClicked(QListWidgetItem*)),this,SLOT(loadSeletedFile(QListWidgetItem*)));
    // Подключаем обработчик удаления файлов вложения по нажатию кнопок del и backspace
    connect(this,SIGNAL(keyPressedSignal(qint32)),SLOT(deleteSelectedFilesHnd(qint32)));
  }
}

void ReportTaskWindow::deleteSelectedFilesHnd(qint32 key)
{
  // Если в фокусе список файлов вложения и нажата кнопка Backspace или Delete
  if((this->focusWidget() == this->ui->selectedFilesLst)&&
    ((Qt::Key_Backspace == key)||(Qt::Key_Delete == key)))
  {
    QList<QListWidgetItem*> itemsForRemove;
    QList<qint32> indexesFileNamesForRemove;
    qint32 itemsCount = this->ui->selectedFilesLst->count();
    // Определяем какие элементы выделены и запоминаем их
    for(qint32 i = 0; i < itemsCount; i++)
    {
        QListWidgetItem* wd = this->ui->selectedFilesLst->item(i);
        if(wd->isSelected())
        {
          itemsForRemove.push_back(wd);
          indexesFileNamesForRemove.push_back(i);
        }
    }
    // Удаляем виджеты из listWidget
    foreach (QListWidgetItem* wd, itemsForRemove)
    {
      delete wd;
    }
    // Удаляем имена файлов из массива с полными именами выбранных файлов вложения поручения
    qint32 size = (indexesFileNamesForRemove.size() - 1);
    for(qint32 i = size; i >= 0; i--)
    {
      _reportFiles->removeAt(indexesFileNamesForRemove[i]);
    }
  }
}

void ReportTaskWindow::keyPressEvent(QKeyEvent* event)
{
  emit(keyPressedSignal(event->key()));
}
