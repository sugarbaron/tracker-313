#include "mainwindow.h"
#include "exception.h"
#include "general.h"
#include "ftphardworker.h"
#include "project.h"
#include "worker.h"
#include "alghoritm.h"
#include "qstringex.h"
#include "qdesktopservices.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),ui(new Ui::MainWindow), _timer(new QTimer), _mainWindowDateTime(new QDateTime),
    _localTasksCash(new QList<Task>),_localWorkersCash(new QList<Worker>),_localProjectsCash(new QList<Project>),
    _localOrdersCash(new QList<Order>),_localReportsCash(new QList<Report>)

{
  ui->setupUi(this);

  // Инициализируем контекстное меню виджетов с именами файлов поручений и отчетов
  initContexMenuForFilesTasks();

  // Выставляем свойства окна
  ui->isResponsibleLbl->setVisible(false);
  ui->splitter->setStretchFactor(0,1);

  // Настраиваем таблицы для отображения поручений
  initTaskTable(ui->myTasksTblWid);
  initTaskTable(ui->requestedByTasksTblWid);
  initTaskTable(ui->completeTasksTblWid);

  // Соединяем обработчик скачивания и открытия файлов вложения по двойном щелчку
  connect(ui->filesTaskLstWid,SIGNAL(itemDoubleClicked(QListWidgetItem*)),this,SLOT(loadFile()));

  // Соединяем обработчик скачивания и открытия файлов вложения отчета по двойном щелчку
  connect(ui->filesReportLstWid,SIGNAL(itemDoubleClicked(QListWidgetItem*)),this,SLOT(loadFile()));

  // Разворачиваем главное окно на весь экран
  this->setWindowState(Qt::WindowMaximized);
}

void MainWindow::initContexMenuForFilesTasks()
{
  // Добавляем действия в контекстное меню виджета с именами файлов поручения
  ui->filesTaskLstWid->addAction(ui->openFileAct);
  ui->filesTaskLstWid->addAction(ui->saveFileAct);

  // Соединяем обработчик загрузки файлов с событием нажатия на действие в меню
  connect(ui->openFileAct,SIGNAL(triggered()),this,SLOT(loadFile()));

  connect(ui->saveFileAct,SIGNAL(triggered()),this,SLOT(saveFile()));

  // Добавляем действия в контекстное меню виджета с именами файлов отчета
  ui->filesReportLstWid->addAction(ui->openFileAct);
  ui->filesReportLstWid->addAction(ui->saveFileAct);
}

void MainWindow::initTaskTable(QTableWidget* tableWidget)
{
  // Задаем цвет разделительной сетки для таблицы(черный)
  tableWidget->setStyleSheet( "QTableWidget { gridline-color: black; }" );

  // Настройка шиирны колонок виджета QTableWidget
  tableWidget->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Stretch);
  tableWidget->setColumnWidth(descriptionColumn,250);
  tableWidget->setColumnWidth(responsibleColumn,115);
  tableWidget->setColumnWidth(accompliceColumn,115);
  tableWidget->setColumnWidth(deadlineColumn,115);
  tableWidget->setColumnWidth(statusColumn,115);

  // Делаем скрытой колонку с Id поручения
  tableWidget->horizontalHeader()->hideSection(idColumn);

  // Настройка QTableWidget в части изменения размера ячейки
  // по размеру содержимого при изменении содержимого
  connect(tableWidget->horizontalHeader(), SIGNAL(sectionResized(int,int,int)),
          tableWidget, SLOT(resizeRowsToContents()));

  // изменение ширины строк таблиц при изменении размеров главного окна
  connect(this,SIGNAL(resizeSignal()),tableWidget,SLOT(resizeRowsToContents()));

  //dbg вставлено для получения диагностики и поимки бага с дерганием таблицы и
  // подвисанием трекера
  connect(this,SIGNAL(resizeSignal()),this,SLOT(resizeSignalEmited()));
  connect(tableWidget->horizontalHeader(), SIGNAL(sectionResized(int,int,int)),
          this, SLOT(sectionResizedSignalEmited()));
}

QTableWidget* MainWindow::getCurrentTableWidget()
{
  QTableWidget* tableWidget;
  // Выбираем таблицу в зависимости от активной вкладки
  switch(this->ui->tasksTbWid->currentIndex())
  {
    case myTasks:
      tableWidget = ui->myTasksTblWid;
      break;
    case requestedByTasks:
      tableWidget = ui->requestedByTasksTblWid;
      break;
    case completeTasks:
      tableWidget = ui->completeTasksTblWid;
      break;
    default:
      qCritical()<<"default branch";
      throw NeedFixCode("tableWidget* is NULL");
  }
  return tableWidget;
}

void MainWindow::addItemInTable(quint32 rowNum, quint32 column,const QString& text, const QColor& color)
{
  QTableWidget* tableWidget = getCurrentTableWidget();
  QTableWidgetItem* item = new QTableWidgetItem;
  item->setText(text);
  item->setBackgroundColor(color);

  // Без блокировки сигналов операция добавления элемента в таблицу
  // Занимает неприлично много времени
  tableWidget->blockSignals(true);
  tableWidget->setItem(rowNum,column,item);
  tableWidget->blockSignals(false);
}

void MainWindow::refreshTasksInTable()
{
  QTableWidget* tableWidget = getCurrentTableWidget();
  quint32 selectedTaskId = 0;

  // запоминаем выделенное поручение
  try{selectedTaskId = getCurrentTaskId();}
  catch(ExecutionAborted){}

  // очищаем таблицу
  tableWidget->clearContents();
  tableWidget->setRowCount(0);

  quint32 rowNum = 0;
  QColor color;
  foreach (Task task, *(_localTasksCash.data()))
  {
    // Получим цвет, которым нужно подсветить поруочение
    color = getColorForTask(task);
    tableWidget->setRowCount(rowNum + 1);
    addItemInTable(rowNum,descriptionColumn,task.getDescription(),color);
    QList<Worker>::const_iterator responsible = qFindEx(*(_localWorkersCash.data()),task.getResponsibleId());
    addItemInTable(rowNum,responsibleColumn,responsible->getShortName(),color);
    addItemInTable(rowNum,idColumn,QString::number(task.getIdTask(),10),color);
    addItemInTable(rowNum,accompliceColumn,task.getStringAccomplicesDb(),color);
    addItemInTable(rowNum,deadlineColumn,task.getDeadlineDate().toString("dd.MM.yyyy"),color);
    addItemInTable(rowNum,statusColumn,task.getTextStatus(),color);
    // Выделяем то поручение, которое было выделено
    if(task.getIdTask() == selectedTaskId)
    {
      tableWidget->selectRow(rowNum);
    }
    ++rowNum;
  }
  tableWidget->resizeRowsToContents();
}

QColor MainWindow::getColorForTask(const Task& task)
{
  QColor color;
  color.setNamedColor("white");
  if(task.getStatus() == Task::complete)
  {
    color.setNamedColor("white");
  }
  else
  if(task.getStatus() == Task::inProgress)
  {
    color.setNamedColor("#FFFED4");
  }
  else
  if(task.getStatus() == Task::canceled)
  {
    color.setNamedColor("#E1E1E1");
  }
  else
  if(task.getStatus() == Task::isBidForReport)
  {
    color.setNamedColor("#E4FFd2");
  }
  else
  if(task.getStatus() == Task::updated)
  {
    color.setNamedColor("#DDCFFF");
  }
  else
  if(task.getStatus() == Task::newTask)
  {
    color.setNamedColor("#D5FFFD");
  }
  // Красим в красный цвет, если поручение было просрочено
  if(((task.getDeadlineDate() < _mainWindowDateTime->date())
     &&(task.getStatus() != Task::canceled)
     &&(task.getStatus() != Task::complete))
     ||(task.getIsOverdue()))
  {
    color.setNamedColor("#FFD0CB");
  }
  return color;
}


void MainWindow::refreshDateTime()
{
  Clock* systemClock = General::getSystemClock();
  *(_mainWindowDateTime) = systemClock->getTime();
  this->ui->dateLbl->setText(_mainWindowDateTime->toString("dd.MM.yyyy"));
  this->ui->timeLbl->setText(_mainWindowDateTime->toString("hh:mm"));
}

void MainWindow::infoRefresh()
{
  // актуализируем дату и время в главном окне
  refreshDateTime();
  quint32 registredUserId = General::getRegistredUserId();
  Worker registredWorker;
  registredWorker.synhronizeWithDb(registredUserId);
  this->ui->userNameLbl->setText(registredWorker.getFullName());

  // запускаем циклическое отображение даты и времени в главном окне
  startMainWindowDateTime();

  // обновляем данные из базы в локальном кэше
  try{refreshLocalCash();}
  // В эту ветку попадаем, если в БД нет ни одной задачи для текущего пользователя
  catch(ExecutionAborted){};

  // обновляем перечень поручений в таблице
  refreshTasksInTable();

  // получаем id выделенного поручения
  try
  {getCurrentTaskId();}
  // если поручение не выделенно, очищаем панель с детальной информацией о поручении
  catch(ExecutionAborted)
  {clearDetailSelectedTaskInfo();}
}

void MainWindow::clearDetailSelectedTaskInfo()
{
  ui->filesTaskLstWid->clear();
  ui->filesReportLstWid->clear();
  ui->ordersLbl->clear();
  ui->projectLbl->clear();
  ui->commentTxtEd->clear();
  ui->isOverdueLbl->clear();
  ui->isResponsibleLbl->hide();
  ui->authorLbl->clear();
  ui->importanceLbl->clear();
}

void MainWindow::refreshLocalCash()
{
  quint32 registredUserId = General::getRegistredUserId();
  SqlHardWorker* sqlHardWorker = General::getSqlHardWorker();
  // Обновляем локальный кэш с поручениями
  _localTasksCash->clear();
  switch(ui->tasksTbWid->currentIndex())
  {
    case myTasks:
      *(_localTasksCash.data()) = sqlHardWorker->getTasksForResponsible(registredUserId);
      break;
    case requestedByTasks:
      *(_localTasksCash.data()) = sqlHardWorker->getTasksRequestedBy(registredUserId);
      break;
    case completeTasks:
      *(_localTasksCash.data()) = sqlHardWorker->getCompleteTasksForWorker(registredUserId);
      break;
    default:
      qCritical()<<"wrong taskTbWid current index";
      QMessageBox::critical(0, tr("ОШБИКА"),tr("Критическая ошибка"), QMessageBox::Ok);
      throw NeedFixCode("default branch");
  }

  // Убираем поручения со статусом "выполнено"
  // Со всех вкладок окна, кроме вкладки "выполненные"
  if(ui->tasksTbWid->currentIndex()!= completeTasks)
  {
    foreach (Task task, *(_localTasksCash.data()))
    {
      if((task.getStatus() == Task::complete)||
         (task.getStatus() == Task::canceled))
      {
        _localTasksCash->removeOne(task);
      }
    }
  }

  // Выполняем сортировку поручений
  qSort(_localTasksCash->begin(),_localTasksCash->end());

  // Обновляем локальный кэш с пользователями
  QList<quint32> idListWorkers;
  QList<quint32> idListOrders;
  QList<quint32> idListProjects;
  QList<quint32> idListReports;
  // Сложим в idList все айдишники
  // имеющиеся во всех поручениях
  foreach (Task task, *(_localTasksCash.data()))
  {
    // для воркеров
    idListWorkers += task.getAccomplicesId();
    idListWorkers.push_back(task.getAuthorId());
    idListWorkers.push_back(task.getResponsibleId());

    // для заказов
    if(task.getOrderId() > 0)
    {
      idListOrders.push_back(task.getOrderId());
    }

    // для проектов
    if(task.getProjectId() > 0)
    {
      idListProjects.push_back(task.getProjectId());
    }

    // для отчетов по поручениям
    if(task.getReportId() > 0)
    {
      idListReports.push_back(task.getReportId());
    }
  }

  // Удаляем повторяющиеся айдишники
  qRemoveEquals(idListWorkers);
  qRemoveEquals(idListOrders);
  qRemoveEquals(idListProjects);
  qRemoveEquals(idListReports);
  
  // Обновим локальный кэш воркеров из БД
  _localWorkersCash->clear();
  try{*(_localWorkersCash.data()) = sqlHardWorker->getWorkers(idListWorkers);}
  catch(ExecutionAborted){}

  // Обновим локальный кэш проектов
  _localProjectsCash->clear();
  try{*(_localProjectsCash.data()) = sqlHardWorker->getProjects(idListProjects);}
  catch(ExecutionAborted){}

  // Обновим локальный кэш заказов
  _localOrdersCash->clear();
  try{*(_localOrdersCash.data()) = sqlHardWorker->getOrders(idListOrders);}
  catch(ExecutionAborted){}

  // Обновим локальный кэш отчетов
  _localReportsCash->clear();

  // Обновим локальный кэш отчетов по поручениям
  try{*(_localReportsCash.data()) = sqlHardWorker->getReports(idListReports);}
  catch(ExecutionAborted){}
}

void MainWindow::startMainWindowDateTime()
{
  // если таймер еще не запущен
  // запускаем!
  if(!_timer->isActive())
  {
    // выставляем интервал срабатываения таймера
    _timer->setInterval(cTimerInterval);
    connect(_timer,SIGNAL(timeout()), SLOT(refreshDateTime()));
    _timer->start();
  }
}

void MainWindow::showDetailSelectedTaskInfo()
{
  quint32 taskId = getCurrentTaskId();
  // Достаем задачу из локального кэша поручений
  QList<Task>::const_iterator task = qFindEx(*(_localTasksCash.data()),taskId);
  // Отображаем важность задачи
  this->ui->importanceLbl->setText(QString::number((int)task->getImportance()));

  try
  {
    // Достаем проект из локального кэша
    QList<Project>::const_iterator project = qFindEx(*(_localProjectsCash.data()), task->getProjectId());
    // Отображаем название проекта
    this->ui->projectLbl->setText(project->getName());
  }
  catch(ExecutionAborted)
  {
    this->ui->projectLbl->clear();
  }

  try
  {
    // Достаем проект из локального кэша
    QList<Order>::const_iterator order = qFindEx(*(_localOrdersCash.data()),task->getOrderId());
    // Отображаем название заказа
    this->ui->ordersLbl->setText(order->getName());
  }
  catch(ExecutionAborted)
  {
    this->ui->ordersLbl->clear();
  }


  QList<Worker>::const_iterator author = qFindEx(*(_localWorkersCash.data()),task->getAuthorId());

  this->ui->authorLbl->setText(author->getShortName());
  // Для задач которые не имеют статус "снято" или "выполнено
  // анализируем текущую дату и сравниваем с датой контроля
  // поручения, для остальных анализируем флаг isOverdue,
  // хрянящийся в БД
  if(((task->getDeadlineDate() < _mainWindowDateTime->date())
     &&(task->getStatus() != Task::canceled)
     &&(task->getStatus() != Task::complete))
     ||(task->getIsOverdue()))
  {
    this->ui->isOverdueLbl->setText(tr("Просрочено"));
  }
  else
  {
    this->ui->isOverdueLbl->clear();
  }
  this->ui->commentTxtEd->setPlainText(task->getComment());

  // Является ли текущий пользователь ответственным за данное поручение
  quint32 registredUserId = General::getRegistredUserId();
  if(task->getResponsibleId() == registredUserId)
  {
    this->ui->isResponsibleLbl->show();
  }
  else
  {
    this->ui->isResponsibleLbl->hide();
  }

  QStringList filesTaskShortNames = FtpHardWorker::getShortFileNames(task->getFilesTask());

  this->ui->filesTaskLstWid->clear();
  this->ui->filesTaskLstWid->addItems(filesTaskShortNames);
  QStringList filesReportShortNames;

  this->ui->filesReportLstWid->clear();
  // Достаем отчет из локального кэша
  QList<Report>::const_iterator report = qFindEx(*(_localReportsCash.data()),task->getReportId());
  filesReportShortNames = FtpHardWorker::getShortFileNames(report->getFiles());
  this->ui->filesReportLstWid->addItems(filesReportShortNames);
}

void MainWindow::refreshStatusTask()
{
  qint32 taskId = getCurrentTaskId();
  QList<Task>::const_iterator task = qFindEx(*(_localTasksCash.data()),taskId);
  quint32 registerUserId = General::getRegistredUserId();
  // если текущий пользователь отвественный за поручение
  // и поручение еще не было просмотрено раннее
  // или есть обновления в поручении
  if((task->getResponsibleId() == registerUserId)
     &&((task->getStatus() == Task::newTask)||(task->getStatus() == Task::updated)))
  {
    SqlHardWorker* sqlHardWorker = General::getSqlHardWorker();
    Task refreshedTask;
    refreshedTask.synhronizeWithDb(task->getIdTask());
    refreshedTask.setStatus(Task::inProgress);
    sqlHardWorker->updateTask(refreshedTask, false);
  }
  else
  {throw ExecutionAborted();}
}

void MainWindow::changeTaskActionBar()
{
  ui->redirectTaskBt->setDisabled(true);
  ui->reportTaskBt->setDisabled(true);
  ui->cancelTaskBt->setDisabled(true);
  ui->editTaskBt->setDisabled(true);
  ui->applyDeclineReportBt->setDisabled(true);

  // Достаем id выделенного поручения
  quint32 selectedTaskId = getCurrentTaskId();

  // Достаем поручение из локального кэша
  QList<Task>::const_iterator task = qFindEx(*(_localTasksCash.data()),selectedTaskId);
  // Если задача снята или выполнена, то с ней сделать ничего нельзя - выходим
  if((task->getStatus() == Task::canceled)
     ||(task->getStatus() == Task::complete))
  {return;}

  // Получаем id пользователя, вошедшего в систему
  quint32 registredUserId = General::getRegistredUserId();

  // Если есть заявка на отчет и текущий пользователь совпадает с автором и ответственным
  if((task->getStatus() == Task::isBidForReport)&&(task->getResponsibleId() == registredUserId)
                                                &&(task->getAuthorId() == registredUserId))
  { // делаем активной кнопку "Отчитаться" и кнопку "Принять/отклонить отчет"
    // т.к. это задача, которая поручена самому себе
    ui->reportTaskBt->setEnabled(true);
    ui->applyDeclineReportBt->setEnabled(true);
    return;
  }
  // Если есть заявка на отчет и текущий пользователь ответсвенный за поручение
  if((task->getStatus() == Task::isBidForReport)&&(task->getResponsibleId() == registredUserId))
  { // делаем активной кнопку "Отчитаться"
    ui->reportTaskBt->setEnabled(true);
    return;
  }
  // Если есть заявка на отчет и текущий пользователь автор поручения
  if((task->getStatus() == Task::isBidForReport)&&(task->getAuthorId() == registredUserId))
  { // делаем активной кнопку "Принять/отклонить отчет"
    ui->applyDeclineReportBt->setEnabled(true);
    return;
  }

  // если залогинившийся пользователь
  // ответственный за поручение и у поручения нет статуса "Снято"
  if(task->getAuthorId() == registredUserId)
  {
    // делаем активной кнопку "Снять"
    ui->cancelTaskBt->setEnabled(true);
  }
  else
  {
    // делаем не активной кнопку "Снять"
    ui->cancelTaskBt->setDisabled(true);
  }

  // Делаем активной кнопку "Редактировать"
  ui->editTaskBt->setEnabled(true);

  // если залогинившийся пользователь
  // ответственный за поручение
  if(task->getResponsibleId() == registredUserId)
  {
    // делаем активными кнопки "Перепоручить", "Отчитаться",
    ui->redirectTaskBt->setEnabled(true);
    ui->reportTaskBt->setEnabled(true);
  }
  else
  {
    // делаем  не активныой кнопку "Отчитаться",
    ui->reportTaskBt->setDisabled(true);

    // Ищем id пользователя, вошедшего в систему в списке соисполнителей поручения
    try
    {
      qFindEx(task->getAccomplicesId(),registredUserId);
      // Нашли, делаем активной кнопку "Перепоручить"
      ui->redirectTaskBt->setEnabled(true);
    }
    catch(ExecutionAborted)
    // Не нашли, делаем не активной кнопку "Перепоручить"
    {ui->redirectTaskBt->setDisabled(true);}
  }
}

quint32 MainWindow::getCurrentTaskId()
{
  // Получаем указатель на активную вкладку с поручениями
  QTableWidget* tableWidget = getCurrentTableWidget();
  // Достаем id выделенного поручения
  if(tableWidget->currentRow() < 0)
  {throw ExecutionAborted();}// поручение не выделено - выходим

  QStringEx strTaskId = (tableWidget->item(tableWidget->currentRow(),idColumn))->text();
  quint32 taskId = strTaskId.toUIntEx();
  return taskId;
}

quint32 MainWindow::getCurrentReportId()
{
  // Достаем поручение из кэша
  QList<Task>::const_iterator task;
  task = qFindEx(*(_localTasksCash.data()),getCurrentTaskId());
  return task->getReportId();
}

QString MainWindow::makePathToFileOnFtp()
{
  QString path;
  QString itemName;

  QList<QListWidgetItem*> selectedItems;
  if(this->focusWidget() == ui->filesTaskLstWid)
  {
    selectedItems = ui->filesTaskLstWid->selectedItems();
    // Конструируем путь к файлу на сервере
    path = "Tasks/id_" + QString::number(getCurrentTaskId());
  }
  else
  if(this->focusWidget() == ui->filesReportLstWid)
  {
    selectedItems = ui->filesReportLstWid->selectedItems();
    // Конструируем путь к файлу на сервере

    path = "Reports/id_" + QString::number(getCurrentReportId());
  }

  // Одновременно может быть выделен только один элемент
  if((selectedItems.size() == 1))
  {
    itemName = selectedItems.takeFirst()->text();
    path += "/" + itemName;
  }
  else // если файл не выделен
  if(selectedItems.isEmpty())
  {
    throw ExecutionAborted();
  }
  else
  {
    QString msg = tr("Wrong selection model");
    qCritical()<<msg;
    throw NeedFixCode(msg);
  }
  return path;
}

void MainWindow::saveFile()
{
  QString path;
  try
  {
    path = makePathToFileOnFtp();
  }
  catch(ExecutionAborted)
  {
    return;
  }
  catch(NeedFixCode& ex)
  {
    qCritical()<<ex.getInfo();
    QMessageBox::critical(0, tr("ОШБИКА"),tr("Критическая ошибка"), QMessageBox::Ok);
    emit(jumpToExitState());
  }
  QString localPath = QFileDialog::getExistingDirectory(this, tr("Сохранить файл"));
  // Если закрыли окно выбора файла или нажали кнопку "отмена"
  if(localPath.isEmpty())
  {
    return;
  }

  // Скачиваем файл с FTP сервера
  try
  {
    FtpHardWorker* ftp = General::getFtpHardWorker();
    ftp->downloadFile(path,localPath);
  }
  catch(FtpException& ex)
  {
    qCritical()<<ex.getInfo();
    QMessageBox::critical(0, tr("ОШБИКА"),tr("Не удалось скачать файл"), QMessageBox::Ok);
  }
  catch(ExecutionAborted){}
}

void MainWindow::loadFile()
{
  QString path;
  QString localPath;

  // Достаем id выделенного поручения
  quint32 selectedTaskId = getCurrentTaskId();

  // Достаем поручение из кэша
  QList<Task>::const_iterator task;
  try
  {task = qFindEx(*(_localTasksCash.data()),selectedTaskId);}
  catch(ExecutionAborted)
  {
    qCritical()<<"task not found in loacal cash";
    QMessageBox::critical(0, tr("ОШБИКА"),tr("Критическая ошибка"), QMessageBox::Ok);
    emit(exit(General::critErrorExitCode));
  }

  try
  {
    path = makePathToFileOnFtp();
    // Конструируем путь на локальной машине
    localPath = "Reports/id_" + QString::number(getCurrentReportId()) + "/";
  }
  catch(ExecutionAborted)
  {
    return;
  }
  catch(NeedFixCode& ex)
  {
    qCritical()<<ex.getInfo();
    QMessageBox::critical(0, tr("ОШБИКА"),tr("Критическая ошибка"), QMessageBox::Ok);
    emit(jumpToExitState());
  }

  // Скачиваем файл с FTP сервера
  try
  {
    FtpHardWorker* ftp = General::getFtpHardWorker();
    ftp->downloadFileToTempDir(path,localPath);
    // Открываем файл с помощью оболочки ОС
    QString itemName = FtpHardWorker::getShortFileName(path);
    if(!QDesktopServices::openUrl(QUrl::fromLocalFile(ftp->getLocalTempPath()
                                                      + "/"
                                                      + localPath
                                                      + itemName)),QUrl::TolerantMode)
    {
      qCritical()<<tr("Can't open local file");
      QMessageBox::critical(this, tr("ОШБИКА"),tr("Ошибка открытия файла"), QMessageBox::Ok);
    }
    else
    {
      qDebug()<<"Open file " + localPath + itemName<<" done";
    }
  }
  catch(FtpException& ex)
  {
    qCritical()<<ex.getInfo();
    QMessageBox::critical(0, tr("ОШБИКА"),tr("Не удалось скачать файл"), QMessageBox::Ok);
  }
  catch(ExecutionAborted){}
}

void MainWindow::resizeSignalEmited()
{
  qDebug()<<tr("resize signal emited");
}

void MainWindow::sectionResizedSignalEmited()
{
  qDebug()<<tr("section resized signal emited");
}

void MainWindow::closeEvent(QCloseEvent* event)
{
  emit(closeSignal());
  QWidget::closeEvent(event);
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
  emit(resizeSignal());
  QWidget::resizeEvent(event);
}


MainWindow::~MainWindow()
{
  delete ui;
  delete _timer;
  delete _mainWindowDateTime;
}

