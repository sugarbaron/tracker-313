#include "redirecttaskwindow.h"
#include "general.h"
#include "task.h"
#include "exception.h"
#include <QMessageBox>

RedirectTaskWindow::RedirectTaskWindow(QWidget *parent) :
  QWidget(parent), ui(new Ui::RedirectTaskWindow),
  _currentUser(new Worker), _redirectedTask(new Task)
{
  ui->setupUi(this);
  connect(this->ui->selectResponsibleCmBx,SIGNAL(activated(int)),SLOT(responsibleWasChanged(int)));
}

RedirectTaskWindow::~RedirectTaskWindow()
{
  delete ui;
}

void RedirectTaskWindow::infoRefresh(quint32 currentTaskId)
{
  // Очищаем поля окна
  ui->commentTxtEd->clear();
  ui->selectResponsibleCmBx->clear();
  ui->sendEmailCoautChBx->setChecked(false);
  ui->sendEmailRespChBx->setChecked(false);
  ui->selectedAccompliceLst->clear();

  // Синхронизируем поручение с БД
  _redirectedTask->synhronizeWithDb(currentTaskId);

  // Синхронизируем залогиневшегося пользователя с БД
  _currentUser->synhronizeWithDb(General::getRegistredUserId());

  // Отображаем данные поручения в окне
  ui->descTxtEd->setPlainText(_redirectedTask->getDescription());
  ui->authCommentTxtEd->setPlainText(_redirectedTask->getComment());
  ui->importanceLbl->setText(QString::number(_redirectedTask->getImportance()));

  // Отображаем название проекта
  try
  {ui->projectLbl->setText(_redirectedTask->getProjectDb().getName());}
  catch(ExecutionAborted){}

  // Отображаем название заказа
  try
  {ui->orderLbl->setText(_redirectedTask->getOrderDb().getName());}
  catch(ExecutionAborted){}

  // устанавливаем диапазон для выбора даты контроля поручения
  ui->deadLineDatEd->setDateRange(_redirectedTask->getCreationDate(),_redirectedTask->getDeadlineDate());
  ui->deadLineDatEd->setDate(_redirectedTask->getDeadlineDate());

  // Отображаем список выбора ответственного за поручение
  quint32 registredUserId = General::getRegistredUserId();
  try
  {
    SqlHardWorker* sqlHardWorker = General::getSqlHardWorker();
    QList<Worker> responsibles = sqlHardWorker->getListOfResponsibleForWorker(registredUserId);
    foreach (Worker w, responsibles)
    {
      ui->selectResponsibleCmBx->addItem(w.getFullName(),QVariant(w.getId()));
    }
  }catch(ExecutionAborted){}// если список ответственных пуст
}

void RedirectTaskWindow::responsibleWasChanged(qint32 index)
{
  // При изменении текущего ответственного находим и удаляем его из списка соисполнителей
  // если он был выбран как соисполнитель
  QListWidgetItem* wd;
  qint32 size =this->ui->selectedAccompliceLst->count();
  for(qint32 i =0; i < size; ++i)
  {
    if(this->ui->selectedAccompliceLst->item(i)->data(Qt::UserRole).value<Worker>()
       == this->ui->selectResponsibleCmBx->itemData(index,Qt::UserRole).value<Worker>())
    {
      wd = this->ui->selectedAccompliceLst->item(i);
      delete wd;
      break;
    }
  }
}

void RedirectTaskWindow::isTaskReadyToSave()
{
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
}

void RedirectTaskWindow::fillTask(Task& task, quint32 idParentTask)
{
  // Обновим данные перепаручаемой задачи из БД
  _redirectedTask->synhronizeWithDb(_redirectedTask->getIdTask());
  task = *(_redirectedTask);

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
    if(!(ui->authCommentTxtEd->toPlainText()).isEmpty())
    {
      task.setComment(ui->authCommentTxtEd->toPlainText() + "\n" + comment);
    }
    else
    {
      task.setComment(comment);
    }
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

  // Статус поручения - новое
  task.setStatus(Task::newTask);

  // Id родительского поручения
  task.setParent(idParentTask);
}

void RedirectTaskWindow::closeEvent(QCloseEvent* event)
{
  emit(closeSignal());
  QWidget::closeEvent(event);
}
