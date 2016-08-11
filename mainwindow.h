#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QListWidgetItem>
#include "ui_mainwindow.h"
#include "msgloghandler.h"
#include "task.h"
#include "worker.h"
#include "project.h"
#include "order.h"
#include "report.h"
#include "ftphardworker.h"

namespace Ui {
class MainWindow;
}

/*!
 * \brief Интерфейс класса, описывающего главное окно приложения
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /// Номера вкладок с перчнем поручений в главном окне
    enum TasksTabWidgetNum
    {
      myTasks = 0,
      requestedByTasks = 1,
      completeTasks = 2
    };

    explicit MainWindow(QWidget *parent = 0);

    /// Интерфейс окна
    Ui::MainWindow *ui;

    /// Метод, настраивающий свойства таблицы с поручениями
    void initTaskTable(QTableWidget *tableWidget);

    /// Метод доступа к защищенному полю _registredUserId
    void setRegistredUserId(quint32);

    /// Метод обновления информации в главном окне
    void infoRefresh();

    /*! \brief Отображает подробную информацию о поручении при его выделении в таблице поручений
     *  \param registredUserId Id пользователя, вошедшего в систему
     */
    void showDetailSelectedTaskInfo();

    /*!
     * \brief Метод, изменяющий доступность пунктов в меню "Поручение" в
     * зависимости от выделенного поручения
     */
    void changeTaskActionBar();

    ///Метод, возвращающий id выделенного поручения на активной вкладке
    quint32 getCurrentTaskId();

    ///Метод, возвращающий id отчета по выделенному поручению на активной вкладке
    quint32 getCurrentReportId();

    /*!
     * \brief Метод, изменяющий статус выделенного поручения
     *
     * Если у поручения был статус "Новое" и его выделил пользователь,
     * который является ответственным за поручение, то статус менятся
     * на "В работе". Если у поручения был статус "Есть обнолвение"
     * и его выделил пользователь, который является ответственным за
     * поручение, то статус менятся на "В работе".
     * \throws ExecutionAborted если ни одно поручение не выделено
     *         или статус поручения не требует обновления
     */
    void refreshStatusTask();

    ~MainWindow();

private:
    /// Константа для использования в условных выражения
    static const qint32 currentRowNotDefined = -1;

    /// Номера колонок в таблицах отображения поручений
    enum TasksTableColumnNum
    {
      descriptionColumn = 0,
      responsibleColumn = 1,
      accompliceColumn = 2,
      deadlineColumn = 3,
      statusColumn = 4,
      idColumn = 5
    };

    /// Интервал срабатывания таймера счета времени в главном окне(в мс)
    static const qint32 cTimerInterval  = 1000;

    /// Таймер для счета времени в главном окне
    QTimer* _timer;

    /// Дата и время для отображения в главном окне приложения
    QDateTime* _mainWindowDateTime;

    /// Метод, возвращающий указатель на текущую таблицу поручений
    QTableWidget* getCurrentTableWidget();

    /// Кэшированные данные о поручениях
    QScopedPointer<QList<Task> > _localTasksCash;

    /// Кэшированные данные о работниках
    QScopedPointer<QList<Worker> > _localWorkersCash;

    /// Кэшированные данные о проектах
    QScopedPointer<QList<Project> > _localProjectsCash;

    /// Кэшированные данные о заказах
    QScopedPointer<QList<Order> > _localOrdersCash;

    /// Кэшированные данные о отчетах по поручениям
    QScopedPointer<QList<Report> > _localReportsCash;

    /// Метод запуска отображения текущей даты и времени в главном окне
    void startMainWindowDateTime();

    /*!
     * \brief Метод добавляющий текст в ячейку таблицы
     * \param rowNum Номер строки
     * \param column Колонка
     * \param text Текст, который будет добавлен
     * \param color Цвет текста, которым будет отображен элемент
     */
    void addItemInTable(quint32 rowNum, quint32 column, const QString &text, const QColor &color);

    /*!
     * \brief  Метод обновляющий список поручений в таблице поручений
     * \param taskList Список поручений
     */
    void refreshTasksInTable();

    /// Метод, обновляющий кэшированные данные из БД
    void refreshLocalCash();

    /*!
     * \brief getColorForTask Метод, определяющий каким цветом
     *                        нужно подсветить поручение в таблице
     *
     *                        Если в поручении передан неизвестный
     *                        статус, то метод вернет белый цвет
     * \param task            Поручение
     * \return                Цвет
     */
    QColor getColorForTask(const Task& task);

    /*!
     * \brief initContexMenuForFilesTasks Метод для инициализации контекстного
     *                                    меню, появлюяющегося при нажатии правой
     *                                    кнопкой мыши на файл вложения поручения
     *                                    или отчета
     */
    void initContexMenuForFilesTasks();

    /*!
     * \brief makePathToFileOnFtp Метод конструирующий путь к выделенному файлу
     *                            вложения отчета/поручения на FTP сервере
     * \throws ExecutionAborted   В случае, если файл не выделен
     * \throws NeedFixCode        В случае, если выделено несколько файлов
     */
    QString makePathToFileOnFtp();

    /// Метод очищающий панель с детальной информацией о поручении
    void clearDetailSelectedTaskInfo();

    /// Событие закрытия окна
    void closeEvent(QCloseEvent* event);

    /// Событие изменения размера окна
    void resizeEvent(QResizeEvent* event);

private slots:

    /// Обработчик срабатывания сигнала таймера на обновление времени и даты
    /// в главном окне
    void refreshDateTime();

    /*!
     * \brief loadFile Метод загружающий файл с FTP сервера
     *                 во временную директорию на локальной
     *                 машине
     */
    void loadFile();

    /*!
     * \brief saveFile Метод загружающий файл с FTP сервера
     *                 и сохранающий в парку на локльной
     *                 машине, выбранную пользователем в диалоге
     */
    void saveFile();

    /// Обработчики для поимки бага с дерганием таблицы и подвисанием трекера
    void resizeSignalEmited();
    void sectionResizedSignalEmited();

  signals:
    /// Сигнал закрытия главного окна
    void closeSignal();
    /// Сигнал изменения размера главного окна
    void resizeSignal();
    /// Сигнал автомату для перехода в exitState при критической ошибке
    void jumpToExitState();
};

#endif // MAINWINDOW_H
