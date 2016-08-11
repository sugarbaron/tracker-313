#include "selectaccomplicewindow.h"
#include "exception.h"
#include "general.h"

SelectAccompliceWindow::SelectAccompliceWindow(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::SelectAccompliceWindow)
{
  ui->setupUi(this);
}

SelectAccompliceWindow::~SelectAccompliceWindow()
{
  delete ui;
}

void SelectAccompliceWindow::infoRefresh(quint32 currentResponsibleId)
{
  // Достаем из БД список работников, которые могут быть соисполнителями поручения
  quint32 registredUserId = General::getRegistredUserId();
  SqlHardWorker* sqlHardWorker = General::getSqlHardWorker();
  QList<Worker> workerList = sqlHardWorker->getListOfResponsibleForWorker(registredUserId);

  // очищаем список работников
  ui->accompliceLst->clear();

  // Достаем из базы пользователя, котоырй выбран ответвенным за поручение
  Worker currentResponsibleWorker;
  currentResponsibleWorker.synhronizeWithDb(currentResponsibleId);
  // Удаляем ответственного работника из списка(т.к. он назначен ответственным и не может
  // быть соисполнителем поручения)
  workerList.removeOne(currentResponsibleWorker);

  // Добавляем соисполнителей в список на форме
  foreach (Worker w, workerList)
  {
    QListWidgetItem* item = new QListWidgetItem();
    item->setText(w.getFullName());
    item->setData(Qt::UserRole,QVariant(w.getId()));
    ui->accompliceLst->addItem(item);
  }
}

void SelectAccompliceWindow::closeEvent(QCloseEvent* event)
{
  emit(closeSignal());
  QDialog::closeEvent(event);
}
