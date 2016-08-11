#ifndef REPORTTASKWINDOW_H
#define REPORTTASKWINDOW_H

#include <QWidget>
#include "task.h"
#include "report.h"
#include "ui_reporttaskwindow.h"

namespace Ui {
  class ReportTaskWindow;
}


/*!
 * \brief Класс, описывающий окно создания отчета по поручению
 */
class ReportTaskWindow : public QWidget
{
  Q_OBJECT

  public:
    /// Конструктор
    explicit ReportTaskWindow(QWidget *parent = 0);

    /// Деструктор
    ~ReportTaskWindow();

    Ui::ReportTaskWindow *ui;

    /// Возможные состояния окна создания/просмотра отчета
    enum WindowState
    {
      createReport = 0,  //!< Создание отчета по поручению
      viewReport = 1     //!< Просмотр отчета по поручению
    };

    /*!
     * \brief  infoRefreshFromTask Метод, отображающий содержимое
     *                             поручения в окне
     * \param  task                Поручение
     * \throws NeedFixCode         В случае если передано невалидное
     *                             поручение
     */
    void infoRefreshFromTask(const Task& task);

    /*!
     * \brief  infoRefreshFromTaskAndReport Метод, отображающий содержимое
     *                                      поручения и отчета в окне просмотра
     *                                      отчета по поручению
     * \param  report                       Отчет
     * \throws NeedFixCode                  В случае возниконовения ошибки
     * \throws SqlQueryException            В случае ошибки взаимодействия с БД
     */
    void infoRefreshFromTaskAndReport(const Task& task,
                                      const Report& report);

    /*!
     * \brief fillReport Заполняет отчет данными с формы и из БД
     * \param report     Заполняемый отчет
     */
    void fillReport(Report &report);

    /*!
     * \brief  updateReportedTask Метод обновляющий поручение,
     *                            по которому был принят/отклонен отчет
     * \param  statusTask         Стутус поручения после отчетности
     *                            ("выполнено" или "есть обновления")
     * \param  task               Выделенное в главном окне поручение
     * \throws NeedFixCode        В случае возниконовения ошибки
     * \throws SqlQueryException  В случае ошибки взаимодействия с БД
     */
    void updateReportedTask(quint32 statusTask, Task &task);

    /*!
     * \brief getReportFiles Метод, возвращающий список имен файлов
     *                       приложенных к отчету по поручению
     * \return               Возвращает абсолютные имена файлов
     */
    QList<QString> getReportFiles();

    /*!
     * \brief addReportFile Метод, добавляющий имя файла к списку
     *                      имен файлов приложенных к отчету
     * \param fileName      Имя файла, которое нужно добавить
     */
    void addReportFile(const QString& fileName);

    /*!
     * \brief  setWindowState Метод задающий текущее состояние окна
     * \param  state          Состояние окна из перечисления WindowState
     * \throws NeedFixCode    В случае передачи некорректного параметра
     */
    void setWindowState(quint32 state);

    /*!
     * \brief getWindowState Метод возвращающий состояние окна
     * \return               Состояние окна из перечисления WindowState
     */
    quint32 getWindowState() const;

  private:
    /// Файлы вложения отчета
    QScopedPointer<QList<QString> > _reportFiles;

    /// Id поручения, отображаемого в окне
    quint32 _idReport;

    /// Состояние окна
    quint32 _windowState;

    /*!
     * \brief closeEvent Обработчик события закрытия окна
     * \param event      Событие
     */
    void closeEvent(QCloseEvent* event);

    /*!
     * \brief keyPressEvent Обработчик события закрытия окна
     * \param event         Событие
     */
    void keyPressEvent(QKeyEvent* event);

  signals:
    /// Сигнал, высылаемый по событию закрытия окна
    void closeSignal();

    /// Сигнал, высылаемый по событию нажатия кнопки на клавиатуре
    void keyPressedSignal(qint32);

    /// Сигнал, высылаемый при изменении состояния окна
    void windowStateChanged();

  private slots:

    /*!
     * \brief loadSeletedFile Метод загружающий файл с FTP сервера
     *                        во временную директорию на локальной машине
     * \param item            Элемент, содержащий имя файла
     */
    void loadSeletedFile(QListWidgetItem* item);

    /// Обработчик изменения состояния окна
    void windowStateChangedHnd();

    /*!
     * \brief deleteSelectedFilesHnd Обработчик нажатия кнопок
     *                               backspace и delete при удалении файлов
     *                               вложения поручения
     * \param key                    Нажатая кнопка
     */
    void deleteSelectedFilesHnd(qint32 key);
};

#endif // REPORTTASKWINDOW_H
