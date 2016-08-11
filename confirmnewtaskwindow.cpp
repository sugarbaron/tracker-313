#include "confirmnewtaskwindow.h"
#include "exception.h"
#include "worker.h"
#include "order.h"
#include "project.h"


ConfirmNewTaskWindow::ConfirmNewTaskWindow(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::ConfirmNewTaskWindow)
{
  ui->setupUi(this);
}

ConfirmNewTaskWindow::~ConfirmNewTaskWindow()
{
  delete ui;
}

void ConfirmNewTaskWindow::infoRefreshFromTask(const Task& task)
{
  // Проеряем полученный таск на валидность
  if(!task.isValid())
  {
    QString msg = "task is not valid";
    qCritical()<<msg;
    throw NeedFixCode(msg);
  }

  // Отображаем суть поручения
  ui->descriptionTxtEd->setPlainText(task.getDescription());

  // Отображаем ответственного
  ui->responsibleLbl->setText(task.getResponsibleDb().getFullName());

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

  // Отображаем срок контроля поручения
  ui->deadLineDatEd->setDate(task.getDeadlineDate());

  // Отображаем заказ
  try{ui->ordersLbl->setText(task.getOrderDb().getName());}
  catch(ExecutionAborted)
  {
    ui->ordersLbl->clear();
  }


  // Отображаем проект
  try{ui->projectLbl->setText(task.getProjectDb().getName());}
  catch(ExecutionAborted)
  {
    ui->projectLbl->clear();
  }

  // Отображаем важность поручения
  ui->importanceLbl->setText(QString::number(task.getImportance()));

  // Отображаем список файлов приложенных к поручению
  ui->selectedFilesLst->clear();
  ui->selectedFilesLst->addItems(task.getFilesTask());
}

void ConfirmNewTaskWindow::closeEvent(QCloseEvent* event)
{
  emit(closeSignal());
  QWidget::closeEvent(event);
}
