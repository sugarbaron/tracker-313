#include "createtaskWindow.h"
#include "exception.h"
#include "general.h"
#include "qstringex.h"
#include <QMessageBox>

CreateTaskWindow::CreateTaskWindow(QWidget *parent) :
  QWidget(parent),ui(new Ui::CreateTaskWindow), _selectedFileNames(new QStringList),
  _currentUser(new Worker)
{
  ui->setupUi(this);
  setAcceptDrops(true);
  // устанавливаем связи между сигналами и слотам объектов в окне
  connect(ui->selectResponsibleCmBx,SIGNAL(activated(int)),SLOT(responsibleWasChanged(int)));
  connect(this,SIGNAL(keyPressedSignal(qint32)),SLOT(deleteSelectedFilesKeyPressed(qint32)));
  connect(ui->includeToOrderChBx,SIGNAL(toggled(bool)),ui->selectOrderCmBx,SLOT(setEnabled(bool)));
  connect(ui->includeToProjectChBx,SIGNAL(toggled(bool)),ui->selectProjectCmBx,SLOT(setEnabled(bool)));
}

CreateTaskWindow::~CreateTaskWindow()
{
  delete ui;
}

void CreateTaskWindow::closeEvent(QCloseEvent* event)
{
  emit(closeSignal());
  QWidget::closeEvent(event);
}

void CreateTaskWindow::responsibleWasChanged(qint32 index)
{
  // При изменении текущего ответственного находим и удаляем его из списка соисполнителей
  // если он был выбран как соисполнитель
  QListWidgetItem* wd;
  qint32 size = this->ui->selectedAccompliceLst->count();
  quint32 responsibleId = this->ui->selectResponsibleCmBx->itemData(index,Qt::UserRole).value<quint32>();
  quint32 currentAccompliceId;
  for(qint32 i = 0; i < size; ++i)
  {
    currentAccompliceId = this->ui->selectedAccompliceLst->item(i)->data(Qt::UserRole).value<quint32>();
    if(currentAccompliceId == responsibleId)
    {
      wd = this->ui->selectedAccompliceLst->item(i);
      delete wd;
      break;
    }
  }
}

void CreateTaskWindow::deleteSelectedFilesKeyPressed(qint32 key)
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
      _selectedFileNames->removeAt(indexesFileNamesForRemove[i]);
    }
  }
}

QStringList CreateTaskWindow::getSelectedFileNames()
{
  return *(_selectedFileNames);
}

void CreateTaskWindow::addSelectedFileName(QString name)
{
  if(!name.isEmpty())
  {
    _selectedFileNames->push_back(name);
  }
  else
  {
    qCritical()<<"empty fileName";
    throw NeedFixCode("wrong parametr");
  }
}

void CreateTaskWindow::infoRefresh()
{
  // очищаем элементы формы
  ui->selectResponsibleCmBx->clear();
  ui->selectOrderCmBx->clear();
  ui->selectProjectCmBx->clear();
  ui->selectedAccompliceLst->clear();
  ui->selectedFilesLst->clear();
  ui->selectImportanceCmBx->setCurrentIndex(defaultImportanceIndex);
  _selectedFileNames->clear();

  // актуализируем информацию о вошедшем в систему пользователе
  _currentUser->synhronizeWithDb(General::getRegistredUserId());

  // устанавливаем диапазон для выбора даты контроля поручения(1 год до текущей даты и 5 лет после)
  Clock* systemClock = General::getSystemClock();
  QDateTime currentDateTime = systemClock->getTime();
  this->ui->deadLineDatEd->setDateRange(currentDateTime.date().addYears(-1),currentDateTime.date().addYears(5));

  // актуализируем дату контроля поручения
  this->ui->deadLineDatEd->setDate(currentDateTime.date());

  // акутализируем перчень работников, которым текущий пользователь может давать поручения
  try
  {
    setResponsibleList();
  }catch(ExecutionAborted){}


  // актуализируем перечень заказов
  try
  {
    SqlHardWorker* sqlHardWorker = General::getSqlHardWorker();
    QList<Order> orders = sqlHardWorker->getAllOrders();
    foreach(Order order, orders)
    {
      this->ui->selectOrderCmBx->addItem(order.getName(),QVariant(order.getId()));
    }
  }catch(ExecutionAborted){}

  // актуализируем перечень проектов
  try
  { 
    SqlHardWorker* sqlHardWorker = General::getSqlHardWorker();
    QList<Project> projects = sqlHardWorker->getAllProjects();
    foreach(Project project, projects)
    {
      this->ui->selectProjectCmBx->addItem(project.getName(),QVariant(project.getId()));
    }
  }catch(ExecutionAborted){}
}

void CreateTaskWindow::setResponsibleList()
{
  quint32 registredUserId = General::getRegistredUserId();
  SqlHardWorker* sqlHardWorker = General::getSqlHardWorker();
  QList<Worker> listOfResponsible = sqlHardWorker->getListOfResponsibleForWorker(registredUserId);
  foreach (Worker w, listOfResponsible)
  {
    this->ui->selectResponsibleCmBx->addItem(w.getFullName(),QVariant(w.getId()));
  }

}

void CreateTaskWindow::keyPressEvent(QKeyEvent* event)
{
  emit(keyPressedSignal(event->key()));
}

void CreateTaskWindow::isCreatedTaskReadyToSave()
{
  // Проверяем заполнено ли поле "Суть поручения"
  if(ui->descriptionTxtEd->toPlainText().isEmpty())
  {
    QMessageBox::information(0, tr("ВНИМАНИЕ"),tr("Поле \"Суть поручения\" не может быть пустым"), QMessageBox::Ok);
    throw ExecutionAborted("description task is empty");
  }

  // Проверяем выбран ли ответственный
  if((ui->selectResponsibleCmBx->currentData().isNull()))
  {
    QMessageBox::information(0, tr("ВНИМАНИЕ"),tr("Не выбран ответственный за поручение"), QMessageBox::Ok);
    throw ExecutionAborted("responsible not selected");
  }

  // Проверяем корректна ли дата контроля
  // Такой ситуации при нормальном функционировании программы быть не должно
  if(!(ui->deadLineDatEd->date().isValid()))
    {
    QMessageBox::information(0, tr("ВНИМАНИЕ"),tr("Выбрана некорректная дата контроля"), QMessageBox::Ok);
    // Поэтому тут кидаем NeedFixCode
    throw NeedFixCode("need fix code");
  }

  // Проверяем активна ли галочка включить в заказ и выбран ли заказ
  if((ui->includeToOrderChBx->isChecked())
     &&(ui->selectOrderCmBx->currentData().isNull()))
  {
    QMessageBox::information(0, tr("ВНИМАНИЕ"),tr("Не выбран заказ"), QMessageBox::Ok);
    throw ExecutionAborted("order is not selected");
  }

  // Проверяем активна ли галочка включить в проект и выбран ли проект
  if((ui->includeToProjectChBx->isChecked())
     &&(ui->selectProjectCmBx->currentData().isNull()))
  {
    QMessageBox::information(0, tr("ВНИМАНИЕ"),tr("Не выбран проект"), QMessageBox::Ok);
    throw ExecutionAborted("project is not selected");
  }
}

void CreateTaskWindow::fillCreatedTask(Task& task)
{
  // Описание поручения
  task.setDescription(ui->descriptionTxtEd->toPlainText());

  // Автор поручения
  quint32 registredUserId = General::getRegistredUserId();
  task.setAuthor(registredUserId);

  // Дата создания
  Clock* systemClock = General::getSystemClock();
  QDateTime creationDate = systemClock->getTime();
  task.setCreationDate(creationDate.date());

  // Коментарий
  if(!ui->commentTxtEd->toPlainText().isEmpty())
  {
    QString comment = "[" + creationDate.toString("dd.MM.yy hh:mm:ss")+ " "
                                     + _currentUser->getShortName() + "]: ";
    comment += ui->commentTxtEd->toPlainText();
    task.setComment(comment);
  }

  // Ответственный
  bool ok = false;
  task.setResponsible(ui->selectResponsibleCmBx->currentData().toUInt(&ok));
  if(!ok)
  {
    qCritical()<<"responsible to int convert error";
    throw NeedFixCode("need fix code");
  }

  // Список соисполнителей
  QList<quint32> accompliceList;
  qint32 listSize = ui->selectedAccompliceLst->count();
  for(qint32 i = 0; i < listSize; i++)
  {
    accompliceList.push_back(ui->selectedAccompliceLst->item(i)->data(Qt::UserRole).toUInt(&ok));
    if(!ok)
    {
       qCritical()<<"accompliceList to int convert error";
       throw NeedFixCode("need fix code");
    }
  }
  task.setAccomplices(accompliceList);

  // Дата контроля
  task.setDeadlineDate(ui->deadLineDatEd->date());

  // Заказ
  if(ui->includeToOrderChBx->isChecked())
  {
    task.setOrder(ui->selectOrderCmBx->currentData().toUInt(&ok));
    if(!ok)
    {
       qCritical()<<"order to int convert error";
       throw NeedFixCode("need fix code");
    }
  }

  // Проект
  if(ui->includeToProjectChBx->isChecked())
  {
    task.setProject(ui->selectProjectCmBx->currentData().toUInt(&ok));
    if(!ok)
    {
       qCritical()<<"order to int convert error";
       throw NeedFixCode("need fix code");
    }
  }

  // Файлы вложения поручения
  task.setFilesTask(getSelectedFileNames());

  // Статус поручения - новое
  task.setStatus(Task::newTask);

  // Важность
  QStringEx importance = ui->selectImportanceCmBx->currentText();
  task.setImportance(importance.toUIntEx());
}

void CreateTaskWindow::fillEditedTask(Task& task)
{
  // Коментарий
  if(!ui->commentTxtEd->toPlainText().isEmpty())
  {
    QString comment;
    Clock* systemClock = General::getSystemClock();
    QDateTime date = systemClock->getTime();
    if(!task.getComment().isEmpty())
    {
      comment = task.getComment() + "\n";
    }
    comment += "[" + date.toString("dd.MM.yy hh:mm:ss")+ " "
                              + _currentUser->getShortName() + "]: ";
    comment += ui->commentTxtEd->toPlainText();
    task.setComment(comment);
  }

  // Ответственный
  bool ok = false;
  task.setResponsible(ui->selectResponsibleCmBx->currentData().toUInt(&ok));
  if(!ok)
  {
    qCritical()<<"responsible to int convert error";
    throw NeedFixCode("need fix code");
  }

  // Список соисполнителей
  QList<quint32> accompliceList;
  qint32 listSize = ui->selectedAccompliceLst->count();
  for(qint32 i = 0; i < listSize; i++)
  {
    accompliceList.push_back(ui->selectedAccompliceLst->item(i)->data(Qt::UserRole).toUInt(&ok));
    if(!ok)
    {
       qCritical()<<"accompliceList to int convert error";
       throw NeedFixCode("need fix code");
    }
  }
  task.setAccomplices(accompliceList);

  // Дата контроля
  task.setDeadlineDate(ui->deadLineDatEd->date());

  // Заказ
  if(ui->includeToOrderChBx->isChecked())
  {
    task.setOrder(ui->selectOrderCmBx->currentData().toUInt(&ok));
    if(!ok)
    {
       qCritical()<<"order to int convert error";
       throw NeedFixCode("need fix code");
    }
  }
  else
  {
    task.setOrder(0);
  }

  // Проект
  if(ui->includeToProjectChBx->isChecked())
  {
    task.setProject(ui->selectProjectCmBx->currentData().toUInt(&ok));
    if(!ok)
    {
       qCritical()<<"order to int convert error";
       throw NeedFixCode("need fix code");
    }
  }
  else
  {
    task.setProject(0);
  }

  // Если задача еще не была просмотрена пользователем
  // то менять статус не нужно
  if(task.getStatus() != Task::newTask)
  { // Меняем статус поручения(было обновлено)
    task.setStatus(Task::updated);
  }

  // Файлы вложения поручения
  task.setFilesTask(getSelectedFileNames());

  // Важность
  QStringEx importance = ui->selectImportanceCmBx->currentText();
  task.setImportance(importance.toUIntEx());
}

void CreateTaskWindow::infoRefreshFromTask(const Task& task)
{
  // Загружаем на форму информацию из БД
  infoRefresh();

  // Загружаем файлы вложения на локальную машину
  FtpHardWorker* ftp = General::getFtpHardWorker();
  QList<QString> shortNames = FtpHardWorker::getShortFileNames(task.getFilesTask());
  foreach (QString file, shortNames)
  {
    // Конструируем путь к файлу
    QString path = "Tasks/id_" + QString::number(task.getIdTask()) + "/" + file;

    // Конструируем путь на локальной машине
    QString localPath = "Tasks/tmp/id_"
                      + QString::number(task.getIdTask())
                      + "/";
    // Скачиваем файл с FTP сервера
    ftp->downloadFileToTempDir(path,localPath);
    _selectedFileNames->push_back(ftp->getLocalTempPath() + "/" + localPath + "/" + file);
  }

  // Отображаем файлы вложения
  ui->selectedFilesLst->addItems(FtpHardWorker::getShortFileNames(*(_selectedFileNames.data())));

  // Отображаем суть поручения
  ui->descriptionTxtEd->setPlainText(task.getDescription());

  // Отображаем дату контроля
  ui->deadLineDatEd->setDate(task.getDeadlineDate());

  // Отображаем ответственного
  qint32 idx = ui->selectResponsibleCmBx->findData(QVariant(task.getResponsibleId()));
  if(idxNotDef == idx)
  {
    Worker worker = task.getResponsibleDb();
    ui->selectResponsibleCmBx->clear();
    ui->selectResponsibleCmBx->addItem(worker.getFullName(),worker.getId());
  }
  else
  {ui->selectResponsibleCmBx->setCurrentIndex(idx);}

  // Отображаем проект
  ui->includeToProjectChBx->setChecked(false);
  ui->selectProjectCmBx->setEnabled(false);
  // Если поручение включено в проект
  if(task.getProjectId()!= 0)
  {
    Project project = task.getProjectDb();
    idx = ui->selectProjectCmBx->findData(project.getId());
    if(idxNotDef == idx)
    {
      QString msg = "Project with id = " + QString::number(project.getId()) + " not found";
      qCritical()<<msg;
      throw NeedFixCode(msg);
    }
    ui->selectProjectCmBx->setCurrentIndex(idx);
    ui->includeToProjectChBx->setChecked(true);
    ui->selectProjectCmBx->setEnabled(true);
  }

  // Отображаем заказ
  ui->includeToOrderChBx->setChecked(false);
  ui->selectOrderCmBx->setEnabled(false);
  // Если поручение включено в проект
  if(task.getOrderId()!= 0)
  {
    Order order = task.getOrderDb();
    idx = ui->selectOrderCmBx->findData(order.getId());
    if(idxNotDef == idx)
    {
      QString msg = "Order with id = " + QString::number(order.getId()) + " not found";
      qCritical()<<msg;
      throw NeedFixCode(msg);
    }
    ui->selectOrderCmBx->setCurrentIndex(idx);
    ui->includeToOrderChBx->setChecked(true);
    ui->selectOrderCmBx->setEnabled(true);
  }

  // Отображаем важность поручения
  idx = ui->selectImportanceCmBx->findText(QString::number(task.getImportance()));
  if(idx == idxNotDef)
  {
    QString msg = "Importance " + QString::number(task.getImportance()) + " not found";
    qCritical()<<msg;
    throw NeedFixCode(msg);
  }
  ui->selectImportanceCmBx->setCurrentIndex(idx);

  // Отображаем соисполнителей
  ui->selectedAccompliceLst->clear();
  try
  {
    QList<Worker> accomplices = task.getAccomplicesDb();
    QListIterator<Worker> accomplice(accomplices);
    Worker worker;
    while(accomplice.hasNext())
    {
      worker = accomplice.next();
      QListWidgetItem* item = new QListWidgetItem();
      item->setText(worker.getFullName());
      item->setData(Qt::UserRole,QVariant(worker.getId()));
      ui->selectedAccompliceLst->addItem(item);
    }
  }
  catch(ExecutionAborted){}
}
