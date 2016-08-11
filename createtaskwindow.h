#ifndef CreateTaskWindow_H
#define CreateTaskWindow_H

#include <QWidget>
#include <QtDebug>
#include <QDropEvent>
#include <QDrag>
#include <QUrl>
#include <QMimeData>
#include "task.h"
#include "worker.h"
#include "ui_createtaskwindow.h"

namespace Ui {
  class CreateTaskWindow;
}

/*!
 * \brief Интерфейс класса, описывающего окно создания поручения
 */
class CreateTaskWindow : public QWidget
{
  Q_OBJECT

  public:
    explicit CreateTaskWindow(QWidget *parent = 0);
    ~CreateTaskWindow();

    /// Графические элементы окна
    Ui::CreateTaskWindow *ui;

    /// Метод для доступа к защищенному полю _selectedFileNames
    QStringList getSelectedFileNames();

    /// Добавляет имя файлов в поле _selectedFileNames
    void addSelectedFileName(QString name);

    /*!
     * \brief Метод, обновляющий информацию в окне создания нового поручения
     */
    void infoRefresh();

    /*!
     * \brief Метод проверяющий все ли обязательные поля заполнены в окне при создании поручения
     * \throw NeedFixCode если выбрана некорректная дата контроля поручения
     * \throw ExecutionAborted если заполнены не все обязательные поля окна создания поручения
     */
    void isCreatedTaskReadyToSave();

    /*!
     * \brief Заполняет создоваемое поручение данными с формы
     * \param task Поручение, которое заполняется данными
     * \throws NeedFixCode при ошибке преобразования данных
     */
    void fillCreatedTask(Task& task);

    /*!
     * \brief Метод, отображающий содержимое поручения в окне
     * \param task поручение
     * \throws NeedFixCode в случае возниконовения ошибки
     * \throws SqlQueryException в случае ошибки взаимодействия с БД
     */
    void infoRefreshFromTask(const Task& task);

    /*!
     * \brief Метод, заполняющий отредактированное поручение информацией из окна
     * \param task заполняемое поручение
     * \throws NeedFixCode в случае возниконовения ошибки
     */
    void fillEditedTask(Task& task);

  signals:
    void closeSignal();
    void keyPressedSignal(qint32);

  private slots:
      /// Обработчик события изменения текущего отвественного в окне создания поручения
      void responsibleWasChanged(qint32 index);

      /// Обработчик нажатия кнопок backspace и delete при удалении файлов вложения поручения
      /// в окне _createTaskWindow
      void deleteSelectedFilesKeyPressed(qint32 key);

  private:
    /// Индекс не определен
    static const qint32 idxNotDef = -1;

    /// Значение важность по умолчанию 5(4 т.к. номерация индексов в комбобоксе идет с 0)
    static const quint32 defaultImportanceIndex = 4;

    /// Имена файлов, выбранных для вложения в поручение
    QScopedPointer<QStringList> _selectedFileNames;

    /// Текущий ползователь, вошедший в систему
    QScopedPointer<Worker> _currentUser;

    /*!
     * \brief Заполняет selectResponsibleCmBx данными о работниках - соисполнителях поручения
     * \param worker Работник - автор поручения
     */
    void setResponsibleList();

    /// Событие закрытия окна
    void closeEvent(QCloseEvent* event);
    void keyPressEvent(QKeyEvent* event);
};

#endif // CreateTaskWindow_H
