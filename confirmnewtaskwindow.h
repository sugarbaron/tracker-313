#ifndef CONFIRMNEWTASKWINDOW_H
#define CONFIRMNEWTASKWINDOW_H

#include <QWidget>
#include "task.h"
#include "ui_confirmnewtaskwindow.h"

namespace Ui {
  class ConfirmNewTaskWindow;
}

/*!
 * \brief ConfirmNewTaskWindow Класс, описывающий окно подтверждения
 *                             создания новго поручения
 */
class ConfirmNewTaskWindow : public QWidget
{
  Q_OBJECT

  public:
    explicit ConfirmNewTaskWindow(QWidget *parent = 0);

    /// Графические элементы окна
    Ui::ConfirmNewTaskWindow *ui;

    /*!
     * \brief infoRefreshFromTask Метод, отображающий содержимое
     *                            поручения в окне
     * \param task                Поручение
     * \param respEmailSend       Нужно ли отправлять уведомление
     *                            ответственному
     * \param accEmailSend        Нужно ли отправлять уведомления
     *                            соисполнителям
     * \throws NeedFixCode        В случае возниконовения ошибки
     * \throws SqlQueryException  В случае ошибки взаимодействия с БД
     */
    void infoRefreshFromTask(const Task& task);

    ~ConfirmNewTaskWindow();

  signals:
    /// Сигнал о том, что окно было закрыто
    void closeSignal();

  private:
    /// Событие закрытия окна
    void closeEvent(QCloseEvent* event);
};

#endif // CONFIRMNEWTASKWINDOW_H
