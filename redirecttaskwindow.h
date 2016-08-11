#ifndef REDIRECTTASKWINDOW_H
#define REDIRECTTASKWINDOW_H

#include <QWidget>
#include "task.h"
#include "worker.h"
#include "ui_redirecttaskwindow.h"

namespace Ui {
class RedirectTaskWindow;
}

class RedirectTaskWindow : public QWidget
{
    Q_OBJECT

public:
    explicit RedirectTaskWindow(QWidget *parent = 0);

    /// Графические элементы окна
    Ui::RedirectTaskWindow *ui;

    /*!
     *  \brief infoRefresh   Метод обновляющий информацию в окне
     *                       перенаправления поручения
     *                       Перед отображением данные обновляются из БД
     *  \param currentTaskId Id отображаемого поручения
     */
    void infoRefresh(quint32 currentTaskId);

    /*!
     * \brief isTaskReadyToSave Метод проверяющий все ли обязательные поля
     *                          заполнены в окне при создании поручения
     * \throws NeedFixCode      Если выбрана некорректная дата контроля поручения
     * \throws ExecutionAborted Если заполнены не все обязательные поля окна
     *                          создания поручения
     */
    void isTaskReadyToSave();

    /*!
     * \brief fillTask     Заполняет перенаправляемое поручение данными с формы
     * \param task         Поручение, которое заполняется данными
     * \param idParentTask Id родительского поручения
     * \throws NeedFixCode при ошибке преобразования данных
     */
    void fillTask(Task& task, quint32 idParentTask);

    ~RedirectTaskWindow();

private:
    /// Событие закрытия окна
    void closeEvent(QCloseEvent* event);

    /// Текущий ползователь, вошедший в систему
    Worker* _currentUser;

    /// Перепоручаемая задача
    Task* _redirectedTask;

private slots:
    /// Обработчик события изменения текущего отвественного в окне создания поручения
    void responsibleWasChanged(qint32 index);

signals:
    void closeSignal();
};

#endif // REDIRECTTASKWINDOW_H
