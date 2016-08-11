/*! \file GuiStateMachine.h
 *  \brief Описание интерфейса класса GuiStateMachine
 *  \author Бубенщиков А.И.
 *  \date 11.12.2015
 */
#ifndef GuiStateMachine_H
#define GuiStateMachine_H

#include <QObject>
#include <QFileDialog>
#include "mainwindow.h"
#include "loginwindow.h"
#include "createtaskwindow.h"
#include "selectaccomplicewindow.h"
#include "redirecttaskwindow.h"
#include "confirmnewtaskwindow.h"
#include "reporttaskwindow.h"
#include "sqlhardworker.h"
#include <QStateMachine>
#include <QtCore>
#include <QTreeView>


/*! \brief Класс описывающий автомат,
 *  обеспечивающий работу графического интерфейса
 */
class GuiStateMachine : public QObject
{
  Q_OBJECT

  private:

  //===============================================================================================
  //============================= Графические объекты =============================================
  //===============================================================================================
  /// Главное окно приложения
  MainWindow* _mainWindow;

  /// Окно регистрации пользователя
  LoginWindow* _loginWindow;

  /// Окно создания нового поручения
  CreateTaskWindow* _createTaskWindow;

  /// Окно выбора соисполнителей при создании нового поручения(вспомогательное диалоговое окно)
  SelectAccompliceWindow* _selectAccompliceWindow;

  /// Окно перенаправления поручения
  RedirectTaskWindow* _redirectTaskWindow;

  /// Окно подтверждения правильности данных при создании поручения
  ConfirmNewTaskWindow* _confirmCreatedTaskWindow;

  /// Окно создания отчета по поручению
  ReportTaskWindow* _reportTaskWindow;

  //===============================================================================================
  //============================= Объекты КА ======================================================
  //===============================================================================================

  /// Конечный автомат
  QStateMachine* _stMachine;
  //============================= Структура КА ====================================================
  //=================== Состояния окна регистрации пользователя ===================================

  /// Состояние инициализации ресурсов и установки соединений с серверами
  QState* _controllerInitState;

  /// Состояние в котором открыто окно регистрации пользователя(пароль скрыт)
  QState* _loginState;

  /// Состояние в котором открыто окно регистрации пользователя(пароль открыт)
  QState* _loginShowPassState;

  /// Состояние проверки корректности введенных данных пользователя(логин и пароль)
  QState* _checkLoginPassState;

  /// Состояние неверно введенных регистрационных данных
  QState* _loginPassWrongState;

  //===================== Состояния главного окна ==================================================
  //================================================================================================

  /// Состояние главного окна(исходное состояние)
  QState* _mainWindowState;

  //===================== Состояния окна создания поручения ========================================
  //================================================================================================

  /// Состояние создания нового поручения
  QState* _createTaskWindowState;

  /// Состояние выбора соисполнителей при создании поручения
  QState* _selectAccompliceWindowState;

  /// Состояние выбора файлов вложения при создании поручения
  QState* _selectFilesCreateTaskState;

  /// Состояние отрисовки выбранных соисполнителей в окне создания нового поручения
  QState* _showSelectedAccompliceState;

  /// Состояние проверки корректности введенной информации при создании
  /// поручения и сохранения поручения в базу
  QState* _saveCreatedTaskToDbState;

  /// Состояние окна перенаправления поручения
  QState* _redirectTaskWindowState;

  /// Состояние обновления информации в главном окне
  QState* _mainWindowInfoRefreshState;

  /// Состояние обновления информации в окне создания нового поручения
  QState* _createTaskWindowInfoRefreshState;

  /// Состояние выбора соисполнителей при перенаправлении поручения
  QState* _selectAccRedirectTaskWindowState;

  /// Состояние отображения выбранных соисполнителей в окне перенаправления поручения
  QState* _showSelectedAccRedirectTask;

  /// Состяние обновления информации в окне перенаправления поручения
  QState* _redirectTaskWindowInfoRefreshState;

  /// Состояние проверки корректности введенной информации при перенаправлении
  /// поручения и сохранения перенаправленного поручения в базу
  QState* _saveRedirectedTaskToDbState;

  /// Состояние обновления информации в окне редактирования поручения
  QState* _editTaskInfoRefreshState;

  /// Состояние редактирования поручения автором поручения
  QState* _editTaskAuthorState;

  /// Состояние редактирования поручения исполнителем поручения
  QState* _editTaskImplementerState;

  /// Сосстояние сохранения отредактированного поручения
  QState* _saveEditedTaskState;

  /// Состояние снятия поручения
  QState* _cancelTaskState;

  /// Состояние проверки заполнения обязательных полей при создании поручения
  QState* _checkCreatedTaskForReadyToSave;

  /// Состояние подтверждения создания нового поручения
  QState* _confirmCreatedTaskState;

  /// Состояние создания отчета по поручению
  QState* _reportTaskState;

  /// Состояние обновления информации в окне создания отчета
  QState* _reportTaskInfoRefreshState;

  /// Состояние выбора файлов вложения к отчету о поручении
  QState* _selectFilesReportState;

  /// Состояние сохранения отчета по поручению
  QState* _saveReportState;

  /// Состояние принятия/отклонения отчета по поручению
  QState* _applyDeclineReportState;

  /// Сохраненное состояние автомта(используется для возврата в заданный узел)
  QHistoryState* _savePointState;

  /// Состояния подтверждения принятия отчета по поручению
  QState* _applyReportState;

  /// Состояния подтверждения отклонения отчета по поручению
  QState* _declineReportState;

  //===============================================================================================
  /// Выход КА
  QFinalState* _exitState;
  
  //===============================================================================================
  /// остальные данные КА
  SqlHardWorker* _sqlHardWorker;
  
  //===============================================================================================
  //============================= Методы класса ===================================================
  //===============================================================================================

  /// Метод инициализации КА
  void stMachineInit();

  /// Метод привязки свойст графических объектов к состояниям КА
  void assignStatesPropertys();

  /// Метод, добавляющий состояния в КА
  void addStatesToSm();

  /// Метод, подключающий сигналы и слоты к состояниям КА
  void setConnectionsSm();

  /// Метод, устанавливающий переходы КА
  void setTransitionsSm();

  //================================================================//

  public:
    ///Конструктор
    explicit GuiStateMachine(QObject *parent = 0);

    /// Деструктор
    ~GuiStateMachine();

    /// Метод запуска стейтмашины
    void smStart();
    
    /// Запомнить инструмент, который следует использовать для работы с sql-базой
    void rememberSqlTool(SqlHardWorker* const sqlHardWorker);

  signals:

    //==============================================================================================
    //============================= Сигналы переходов автомата =====================================
    //==============================================================================================

    /// Сигнал автомату для перехода в состояние _loginState
    void jumpToLoginState();

    /// Сигнал автомату для перехода в состояние _mainWindowState
    void jumpToMainWindowState();

    /// Сигнал автомату для перехода в состояние _mainWindowInfoRefreshState
    void jumpToMainWindowInfoRefreshState();

    /// Сигнал автомату о том, что введеные регистрационные данные не верны
    void jumpToLoginPassWrongState();

    /// Сигнал автомату о том, что произошла ошибка и нужно завершить работу приложения(переход в _exitState)
    void jumpToExitState();

    /// Сигнал автомату для перехода в состояние _createTaskWindowState
    void jumpTocreateTaskWindowState();

    /// Сигнал автомату для перехода в состояние _mainWindowCurrentTaskChangedState
    void jumpToMainWindowCurrentTaskChangedState();

    /// Сигнал автомату для перехода в состояние _redirectTaskWindowState
    void jumpToRedirectTaskWindowState();

    /// Сигнал автомату для перехода в состояние _editTaskImplementerState
    void jumpToEditTaskImplementerState();

    /// Сигнал автомату для перехода в состояние _editTaskAuthorState
    void jumpToEditTaskAuthorState();

    /// Сигнал автомату для перехода в состояние _confirmCreatedTaskState
    void jumpToConfirmCreatedTaskState();

    /// Сигнал автомату для перехода в состояние _reportTaskState
    void jumpToReportTaskState();

    /// Сигнал автомату для перехода в состояние _applyDeclineReportState
    void jumpToApplyDeclineReportState();

    //==============================================================================================
  private slots:

    //==============================================================================================
    //============================= Обработчики состояний автомата =================================
    //==============================================================================================

    /// Обработчик состояния _controllerInitState
    void controllerInitStateHnd();

    /// Обработчик состояния _checkLoginPass
    void checkLoginPassHnd();

    /// Обработчик состояния _mainWindowState
    void mainWindowStateHnd();

    /// Обработчик состояния _mainWindowInfoRefreshState
    void mainWindowInfoRefreshStateHnd();

    /// Обработчик состояния _createTaskWindowState
    void createTaskWindowStateHnd();

    /// Обработчик состояния _createTaskWindowInfoRefreshState
    void createTaskWindowInfoRefreshStateHnd();

    /// Обработчик состояния _selectAccompliceWindowState
    void selectAccompliceWindowStateHnd();

    /// Обработчик состояния _selectFilesCreateTaskState
    void selectFilesCreateTaskStateHnd();

    /// Обработчик состояния _showSelectedAccompliceState
    void showSelectedAccompliceHnd();

    /// Обработчик состояния _saveCreatedTaskToDbState
    void saveCreatedTaskToDbStateHnd();

    /// Обработчик состояния _redirectTaskWindowInfoRefreshState
    void redirectTaskWindowInfoRefreshStateHnd();

    /// Обработчик состояния _selectAccRedirectTaskWindowState
    void selectAccRedirectTaskWindowStateHnd();

    /// Обработчик состояния _showSelectedAccRedirectTask
    void showSelectedAccRedirectTaskHnd();

    /// Обработчик состояния _saveRedirectedTaskToDbState
    void saveRedirectedTaskToDbStateHnd();

    /// Обработчик состояния _editTaskInfoRefreshState
    void editTaskInfoRefreshStateHnd();

    /// Обработчик состояния _saveEditedTaskState
    void saveEditedTaskStateHnd();

    /// Обработчик состояния _cancelTaskState
    void cancelTaskStateHnd();

    /// Обработчик состояния _confirmCreatedTaskState
    void confirmCreatedTaskStateHnd();

    /// Обработчик состояния _checkCreatedTaskForReadyToSave
    void checkCreatedTaskForReadyToSaveHnd();

    /// Обработчик состояния _reportTaskState
    //void reportTaskStateHnd();

    /// Обработчик состояния _saveReportState
    void saveReportStateHnd();

    /// Обработчик состояния _applyDeclineReportState
    void applyDeclineReportStateHnd();

    /// Обработчик состояния _applyReportState
    void applyReportStateHnd();

    /// Обработчик состояния _declineReportState
    void declineReportStateHnd();

    /// Обработчик состояния _selectFilesReportState
    void selectFilesReportStateHnd();

    /// Обработчик состояния _reportTaskInfoRefreshState
    void reportTaskInfoRefreshStateHnd();

    /// Обработчик состояния _exitState
    void exitStateHnd();
};


#endif // GuiStateMachine_H
