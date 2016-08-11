#ifndef SELECTACCOMPLICEWINDOW_H
#define SELECTACCOMPLICEWINDOW_H

#include <QDialog>
#include "worker.h"
#include "ui_selectaccomplicewindow.h"


namespace Ui {
  class SelectAccompliceWindow;
}

class Worker;

class SelectAccompliceWindow : public QDialog
{
  Q_OBJECT

public:
  explicit SelectAccompliceWindow(QWidget *parent = 0);

  /// Графические элементы окна
  Ui::SelectAccompliceWindow *ui;

  /*!
   * \brief Метод обновляющий список соисполнителей в окне
   * \param currentResponsibleId Id работника, выбранного ответственнным за поручение
   * \param workerList Список работников, которым может быть дано поручение
   * \throws NeedFixCode в случае ошибки преобразования id текущего ответственного
   */
  void infoRefresh(quint32 currentResponsibleId);

  Ui::SelectAccompliceWindow* getUi();
  ~SelectAccompliceWindow();

signals:
  void closeSignal();

private:
  void closeEvent(QCloseEvent* event);
};



#endif // SELECTACCOMPLICEWINDOW_H
