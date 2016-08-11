#include "guistatemachine.h"
#include "general.h"
#include "exception.h"
#include "ftphardworker.h"
#include "email.h"
#include "smtphardworker.h"
#include "smtpclient.h"
#include <QMessageBox>

GuiStateMachine::GuiStateMachine(QObject *parent) :
  QObject(parent),
  _mainWindow(new MainWindow), _loginWindow(new LoginWindow),
  _createTaskWindow(new CreateTaskWindow),_selectAccompliceWindow(new SelectAccompliceWindow),
  _redirectTaskWindow(new RedirectTaskWindow),_confirmCreatedTaskWindow(new ConfirmNewTaskWindow),
  _reportTaskWindow(new ReportTaskWindow),
  _stMachine(new QStateMachine),_controllerInitState(new QState), _loginState(new QState),_loginShowPassState(new QState),
  _checkLoginPassState(new QState),_loginPassWrongState(new QState), _mainWindowState(new QState),
  _createTaskWindowState(new QState), _selectAccompliceWindowState(new QState), _selectFilesCreateTaskState(new QState),
  _showSelectedAccompliceState(new QState), _saveCreatedTaskToDbState(new QState),
  _redirectTaskWindowState(new QState), _mainWindowInfoRefreshState(new QState),
  _createTaskWindowInfoRefreshState(new QState), _selectAccRedirectTaskWindowState(new QState),
  _showSelectedAccRedirectTask(new QState), _redirectTaskWindowInfoRefreshState(new QState),
  _saveRedirectedTaskToDbState(new QState),_editTaskInfoRefreshState(new QState),
  _editTaskAuthorState(new QState), _editTaskImplementerState(new QState), _saveEditedTaskState(new QState),
  _cancelTaskState(new QState),_checkCreatedTaskForReadyToSave(new QState),
  _confirmCreatedTaskState(new QState), _reportTaskState(new QState), _reportTaskInfoRefreshState(new QState),
  _selectFilesReportState(new QState), _saveReportState(new QState), _applyDeclineReportState(new QState),
  _savePointState(new QHistoryState), _applyReportState(new QState), _declineReportState(new QState),
  _exitState(new QFinalState), _sqlHardWorker(NULL)
{
  // Инициализируем КА
  stMachineInit();
}

GuiStateMachine::~GuiStateMachine()
{
  delete _mainWindow;
  delete _loginWindow;
  delete _createTaskWindow;
  delete _selectAccompliceWindow;
  delete _redirectTaskWindow;
  delete _confirmCreatedTaskWindow;
  delete _reportTaskWindow;
  delete _stMachine;
}

void GuiStateMachine::smStart()
{
  _stMachine->start();
}

void GuiStateMachine::rememberSqlTool(SqlHardWorker* const sqlHardWorker)
{
  _sqlHardWorker = sqlHardWorker;
}

void GuiStateMachine::addStatesToSm()
{
  _stMachine->addState(_controllerInitState);
  _stMachine->addState(_loginState);
  _stMachine->addState(_loginShowPassState);
  _stMachine->addState(_checkLoginPassState);
  _stMachine->addState(_loginPassWrongState);
  _stMachine->addState(_mainWindowState);
  _stMachine->addState(_exitState);
  _stMachine->addState(_createTaskWindowState);
  _stMachine->addState(_selectAccompliceWindowState);
  _stMachine->addState(_selectFilesCreateTaskState);
  _stMachine->addState(_showSelectedAccompliceState);
  _stMachine->addState(_saveCreatedTaskToDbState);
  _stMachine->addState(_redirectTaskWindowState);
  _stMachine->addState(_mainWindowInfoRefreshState);
  _stMachine->addState(_createTaskWindowInfoRefreshState);
  _stMachine->addState(_selectAccRedirectTaskWindowState);
  _stMachine->addState(_showSelectedAccRedirectTask);
  _stMachine->addState(_redirectTaskWindowInfoRefreshState);
  _stMachine->addState(_saveRedirectedTaskToDbState);
  _stMachine->addState(_editTaskAuthorState);
  _stMachine->addState(_editTaskInfoRefreshState);
  _stMachine->addState(_editTaskImplementerState);
  _stMachine->addState(_saveEditedTaskState);
  _stMachine->addState(_savePointState);
  _stMachine->addState(_cancelTaskState);
  _stMachine->addState(_confirmCreatedTaskState);
  _stMachine->addState(_checkCreatedTaskForReadyToSave);
  _stMachine->addState(_reportTaskState);
  _stMachine->addState(_selectFilesReportState);
  _stMachine->addState(_saveReportState);
  _stMachine->addState(_applyDeclineReportState);
  _stMachine->addState(_applyReportState);
  _stMachine->addState(_declineReportState);
  _stMachine->addState(_reportTaskInfoRefreshState);
}

void GuiStateMachine::setConnectionsSm()
{
  // Обработчик по входу в состояние _controllerInitState
  connect(_controllerInitState,SIGNAL(entered()),SLOT(controllerInitStateHnd()));

  // Обработчик по входу в состояние _checkLoginPass
  connect(_checkLoginPassState,SIGNAL(entered()),SLOT(checkLoginPassHnd()));

  // Обработчик по входу в состояние _mainWindowState
  connect(_mainWindowState,SIGNAL(entered()),SLOT(mainWindowStateHnd()));

  // Обработчик по входу в состояние _mainWindowInfoRefreshState
  connect(_mainWindowInfoRefreshState,SIGNAL(entered()),SLOT(mainWindowInfoRefreshStateHnd()));

  // Обработчик по входу в состояние _createTaskWindowState
  connect(_createTaskWindowState,SIGNAL(entered()),SLOT(createTaskWindowStateHnd()));

  // Обработчик по входу в состояние _createTaskWindowInfoRefreshState
  connect(_createTaskWindowInfoRefreshState,SIGNAL(entered()),SLOT(createTaskWindowInfoRefreshStateHnd()));

  // Обработчик по входу в состояние _selectAccompliceWindowState
  connect(_selectAccompliceWindowState,SIGNAL(entered()),SLOT(selectAccompliceWindowStateHnd()));

  // Обработчик по входу в состояние _selectFilesCreateTaskState
  connect(_selectFilesCreateTaskState,SIGNAL(entered()),SLOT(selectFilesCreateTaskStateHnd()));

  // Обработчик по входу в состояние _showSelectedAccompliceState
  connect(_showSelectedAccompliceState,SIGNAL(entered()),SLOT(showSelectedAccompliceHnd()));

  // Обработчик по входу в состояние _saveCreatedTaskToDbState
  connect(_saveCreatedTaskToDbState,SIGNAL(entered()),SLOT(saveCreatedTaskToDbStateHnd()));

  // Обработчик по входу в состояние _redirectTaskWindowInfoRefreshState
  connect(_redirectTaskWindowInfoRefreshState,SIGNAL(entered()),SLOT(redirectTaskWindowInfoRefreshStateHnd()));

  // Обработчик по входу в состояние _selectAccRedirectTaskWindowState
  connect(_selectAccRedirectTaskWindowState,SIGNAL(entered()),SLOT(selectAccRedirectTaskWindowStateHnd()));

  // Обработчик по входу в состояние _showSelectedAccRedirectTask
  connect(_showSelectedAccRedirectTask,SIGNAL(entered()),SLOT(showSelectedAccRedirectTaskHnd()));

  // Обработчик по входу в состояние _saveRedirectedTaskToDbState
  connect(_saveRedirectedTaskToDbState,SIGNAL(entered()),SLOT(saveRedirectedTaskToDbStateHnd()));

  // Обработчик по входу в состояние _editTaskInfoRefreshState
  connect(_editTaskInfoRefreshState, SIGNAL(entered()),SLOT(editTaskInfoRefreshStateHnd()));

  // Обработчик по входу в состояние _saveEditedTaskState
  connect(_saveEditedTaskState,SIGNAL(entered()),SLOT(saveEditedTaskStateHnd()));

  // Обработчик по входу в состояние _cancelTaskState
  connect(_cancelTaskState,SIGNAL(entered()), SLOT(cancelTaskStateHnd()));

  // Обработчик по входу в состояние _confirmCreatedTaskState
  connect(_confirmCreatedTaskState,SIGNAL(entered()), SLOT(confirmCreatedTaskStateHnd()));

  // Обработчик по входу в состояние _checkCreatedTaskForReadyToSave
  connect(_checkCreatedTaskForReadyToSave,SIGNAL(entered()),SLOT(checkCreatedTaskForReadyToSaveHnd()));

  // Обработчик по входу в состояние _reportTaskState
  //connect(_reportTaskState,SIGNAL(entered()),SLOT(reportTaskStateHnd()));

  // Обработчик по входу в состояние _saveReportState
  connect(_saveReportState,SIGNAL(entered()),SLOT(saveReportStateHnd()));

  // Обрабочик по входу в состояние _applyDeclineReportState
  connect(_applyDeclineReportState,SIGNAL(entered()),SLOT(applyDeclineReportStateHnd()));

  // Обработчик по входу в состояние _applyReportState
  connect(_applyReportState,SIGNAL(entered()),SLOT(applyReportStateHnd()));

  // Обработчик по входу в состояние _declineReportState
  connect(_declineReportState,SIGNAL(entered()),SLOT(declineReportStateHnd()));

  // Обработчик по входу в состояние _selectFilesReportState
  connect(_selectFilesReportState,SIGNAL(entered()),SLOT(selectFilesReportStateHnd()));

  // Обработчик по входу в состояние _reportTaskInfoRefreshState
  connect(_reportTaskInfoRefreshState,SIGNAL(entered()),SLOT(reportTaskInfoRefreshStateHnd()));

  // Обработчик по входу в состояние _exitState
  connect(_exitState, SIGNAL(entered()),SLOT(exitStateHnd()));

  // Закрытие окон по завершению работы стейтмашины
  connect(_stMachine, SIGNAL(finished()), _loginWindow, SLOT(close()));
  connect(_stMachine, SIGNAL(finished()), _mainWindow, SLOT(close()));
  connect(_stMachine, SIGNAL(finished()), _createTaskWindow, SLOT(close()));
  connect(_stMachine, SIGNAL(finished()), _confirmCreatedTaskWindow, SLOT(close()));
  connect(_stMachine, SIGNAL(finished()), _redirectTaskWindow, SLOT(close()));
  connect(_stMachine, SIGNAL(finished()), _reportTaskWindow, SLOT(close()));
  connect(_stMachine, SIGNAL(finished()), _selectAccompliceWindow, SLOT(close()));
}

void GuiStateMachine::setTransitionsSm()
{
  // Переход из состояния _controllerInitState в _loginState
  _controllerInitState->addTransition(this,SIGNAL(jumpToLoginState()),_loginState);

  // Переход из состояний _loginState и _loginStateShowPass в checkLoginPass по нажатию кнопки "Ввод"
  _loginState->addTransition(_loginWindow->ui->okBt, SIGNAL(clicked()),_checkLoginPassState);
  _loginShowPassState->addTransition(_loginWindow->ui->okBt, SIGNAL(clicked()),_checkLoginPassState);

  // Переход из состояния _checkLoginPass в _loginPassWrong по сигналу jumpToLoginPassWrongState
  _checkLoginPassState->addTransition(this,SIGNAL(jumpToLoginPassWrongState()),_loginPassWrongState);

  // Перход из состояния _checkLoginPass в _mainWindowInfoRefreshState по сигналу jumpToMainWindowState
  _checkLoginPassState->addTransition(this,SIGNAL(jumpToMainWindowState()),_mainWindowInfoRefreshState);

  // Перход из состояния _loginPassWrong в _loginState по нажатию кнопки "Ок"
  _loginPassWrongState->addTransition(_loginWindow->ui->okBt,SIGNAL(clicked()),_loginState);

  // Переход из состояния _loginState в _loginStateHidePass по нажатию галочки "Показать пароль"
  _loginState->addTransition(_loginWindow->ui->showPassChbx,SIGNAL(clicked()),_loginShowPassState);

  // Переход из состояния _loginStateHidePass в _loginState по снятию галочки "Показать пароль"
  _loginShowPassState->addTransition(_loginWindow->ui->showPassChbx,SIGNAL(clicked()),_loginState);

  // Переходы в состояние _createTaskWindowInfoRefreshState по нажатию кнопки "Создать поручение"
  _mainWindowState->addTransition(_mainWindow->ui->createTaskBt, SIGNAL(clicked(bool)),_createTaskWindowInfoRefreshState);

  // Переход из _createTaskWindowInfoRefreshState в _createTaskWindowState по сигналу автомата
  _createTaskWindowInfoRefreshState->addTransition(this, SIGNAL(jumpTocreateTaskWindowState()),_createTaskWindowState);

  // Переход из состояния _createTaskWindowState в _mainWindowInfoRefreshState по закрытию окна "Создание поручение"
  _createTaskWindowState->addTransition(_createTaskWindow, SIGNAL(closeSignal()),_mainWindowInfoRefreshState);

  // Переход из состояния _createTaskWindowState в _mainWindowInfoRefreshState по нажатию кнопки "Отмена"
  _createTaskWindowState->addTransition(_createTaskWindow->ui->canselBt, SIGNAL(clicked(bool)),_mainWindowInfoRefreshState);

  // Переход из состояний создания нового поручения в состояние _checkCreatedTaskForReadyToSave по нажатию кнопки "Создать"
  _createTaskWindowState->addTransition(_createTaskWindow->ui->createBt, SIGNAL(clicked(bool)),_checkCreatedTaskForReadyToSave);

  // Переход из состояния _checkCreatedTaskForReadyToSave в _createTaskWindowState
  _checkCreatedTaskForReadyToSave->addTransition(this,SIGNAL(jumpTocreateTaskWindowState()),_createTaskWindowState);

  // Переход из состояния _checkCreatedTaskForReadyToSave в _confirmCreatedTaskState
  _checkCreatedTaskForReadyToSave->addTransition(this,SIGNAL(jumpToConfirmCreatedTaskState()),_confirmCreatedTaskState);

  // Переход из состояния _confirmCreatedTaskState в _saveCreatedTaskToDbState по нажатию кнопки "Подтвердить"
  _confirmCreatedTaskState->addTransition(_confirmCreatedTaskWindow->ui->okBt,SIGNAL(clicked(bool)),_saveCreatedTaskToDbState);

  // Переход из состояния _confirmCreatedTaskState в _createTaskWindowState по нажатию кнопки "Отмена"
  _confirmCreatedTaskState->addTransition(_confirmCreatedTaskWindow->ui->cancelBt,SIGNAL(clicked(bool)),_createTaskWindowState);

  // Переход из состояния _confirmCreatedTaskState в _createTaskWindowState по закрытию окна подтверждения создания поручения
  _confirmCreatedTaskState->addTransition(_confirmCreatedTaskWindow,SIGNAL(closeSignal()),_createTaskWindowState);


  //Перход из состояния _saveCreatedTaskToDbState в состояние _mainWindowInfoRefreshState
  _saveCreatedTaskToDbState->addTransition(this, SIGNAL(jumpToMainWindowInfoRefreshState()),_mainWindowInfoRefreshState);

  //Перход из состояния _mainWindowState в состояние _mainWindowInfoRefreshState
  _mainWindowState->addTransition(this, SIGNAL(jumpToMainWindowInfoRefreshState()),_mainWindowInfoRefreshState);

  //Перход из состояния _saveCreatedTaskToDbState в состояние _mainWindowState
  _saveCreatedTaskToDbState->addTransition(this, SIGNAL(jumpToMainWindowState()),_mainWindowState);

  //Перход из состояния _saveCreatedTaskToDbState в состояние _confirmCreatedTaskState
  _saveCreatedTaskToDbState->addTransition(this, SIGNAL(jumpToConfirmCreatedTaskState()),_confirmCreatedTaskState);

  // Переход в состояние _mainWindowInfoRefreshState по изменению номера текущей вкладки поручений
  _mainWindowState->addTransition(_mainWindow->ui->tasksTbWid,SIGNAL(currentChanged(int)),_mainWindowInfoRefreshState);

  // Перход из состояния _mainWindowInfoRefreshState в состояние _mainWindowState
  _mainWindowInfoRefreshState->addTransition(this,SIGNAL(jumpToMainWindowState()),_mainWindowState);

  // Переходы из состояния _mainWindowState в себя же по изменению выделенного поручения полдьзователем
  _mainWindowState->addTransition(_mainWindow->ui->myTasksTblWid,SIGNAL(currentItemChanged(QTableWidgetItem*,QTableWidgetItem*)),_mainWindowState);
  _mainWindowState->addTransition(_mainWindow->ui->requestedByTasksTblWid,SIGNAL(currentItemChanged(QTableWidgetItem*,QTableWidgetItem*)),_mainWindowState);
  _mainWindowState->addTransition(_mainWindow->ui->completeTasksTblWid,SIGNAL(currentItemChanged(QTableWidgetItem*,QTableWidgetItem*)),_mainWindowState);


  // Переходы в состояние _mainWindowInfoRefreshState по нажатию кнопки "Обновить"
  _mainWindowState->addTransition(_mainWindow->ui->refreshBt,SIGNAL(clicked(bool)),_mainWindowInfoRefreshState);

  // Переходы в состояние _selectAccompliceWindowState по нажатию кнопки "Выбор соисполнителей"
  _createTaskWindowState->addTransition(_createTaskWindow->ui->selectAccompliceBt,SIGNAL(clicked(bool)),_selectAccompliceWindowState);

  // Переход в состояние из которого пришли в _selectAccompliceWindowState по нажатию кнопки "отмена" в окне "Выбор соисполнителей"
  _selectAccompliceWindowState->addTransition(_selectAccompliceWindow->ui->cancelBt,SIGNAL(clicked(bool)),_savePointState);

  // Переход в состояние из которого пришли в _selectAccompliceWindowState по закрытию окна "Выбор соисполнителей"
  _selectAccompliceWindowState->addTransition(_selectAccompliceWindow,SIGNAL(closeSignal()),_savePointState);

  // Переход в состояние _showSelectedAccompliceState по нажатию кнопки "Ок" в окне "Выбор соисполнителей"
  _selectAccompliceWindowState->addTransition(_selectAccompliceWindow->ui->okBt,SIGNAL(clicked(bool)),_showSelectedAccompliceState);

  // Переходы в состояние _selectFilesCreateTaskState по нажатию кнопки "Выбор файлов"
  _createTaskWindowState->addTransition(_createTaskWindow->ui->selectFilesBt,SIGNAL(clicked(bool)),_selectFilesCreateTaskState);
  _editTaskAuthorState->addTransition(_createTaskWindow->ui->selectFilesBt,SIGNAL(clicked(bool)),_selectFilesCreateTaskState);

  // Переход из состояния _selectFilesCreateTaskState в в сохраненное состояния(_editTaskState или _createTaskWindowState)
  _selectFilesCreateTaskState->addTransition(this,SIGNAL(jumpTocreateTaskWindowState()),_savePointState);

  // Переход из состояния _showSelectedAccompliceState в сохраненное состояния(_editTaskState или _createTaskWindowState)
  _showSelectedAccompliceState->addTransition(this,SIGNAL(jumpTocreateTaskWindowState()),_savePointState);

  // Переход из _mainWindowState в _redirectTaskWindowState по нажатию "Перепоручить" в меню действий
  _mainWindowState->addTransition((_mainWindow->ui->redirectTaskBt),SIGNAL(clicked(bool)),_redirectTaskWindowInfoRefreshState);

  // Переход из _redirectTaskWindowState в _mainWindowInfoRefreshState по нажатию кнопки "Отмена"
  _redirectTaskWindowState->addTransition((_redirectTaskWindow->ui->cancelBt),SIGNAL(clicked(bool)),_mainWindowInfoRefreshState);

  // Переход из _redirectTaskWindowState в _mainWindowInfoRefreshState по закрытию окна
  _redirectTaskWindowState->addTransition(_redirectTaskWindow,SIGNAL(closeSignal()),_mainWindowInfoRefreshState);

  // Переход из _redirectTaskWindowInfoRefreshState в  _redirectTaskWindowState
  _redirectTaskWindowInfoRefreshState->addTransition(this,SIGNAL(jumpToRedirectTaskWindowState()),_redirectTaskWindowState);

  // Переход из _redirectTaskWindowState в _selectAccRedirectTaskWindowState по
  // нажатию кнопки "Выбрать соисполнителей"
  _redirectTaskWindowState->addTransition(_redirectTaskWindow->ui->selectAccompliceBt,SIGNAL(clicked(bool)),_selectAccRedirectTaskWindowState);

  // Переход из _selectAccRedirectTaskWindowState в _showSelectedAccRedirectTask по нажатию кнопки "Ок" в окне выбора соисполнителей
  _selectAccRedirectTaskWindowState->addTransition(_selectAccompliceWindow->ui->okBt,SIGNAL(clicked(bool)),_showSelectedAccRedirectTask);

  // Переход из _selectAccRedirectTaskWindowState в _redirectTaskWindowState по нажатию кнопки "Отмена" в окне выбора соисполнителей
  _selectAccRedirectTaskWindowState->addTransition(_selectAccompliceWindow->ui->cancelBt,SIGNAL(clicked(bool)),_redirectTaskWindowState);

  // Переход из _selectAccRedirectTaskWindowState в _redirectTaskWindowState по закрытию окна выбора соисполнителей
  _selectAccRedirectTaskWindowState->addTransition(_selectAccompliceWindow,SIGNAL(closeSignal()),_redirectTaskWindowState);

  // Переход из сотояния _redirectTaskWindowState в _saveRedirectedTaskToDbState по нажатию кнопки "Ок"
  _redirectTaskWindowState->addTransition(_redirectTaskWindow->ui->okBt,SIGNAL(clicked(bool)),_saveRedirectedTaskToDbState);

  // Переход из состояния _redirectTaskWindowInfoRefreshState в _mainWindowState
  _redirectTaskWindowInfoRefreshState->addTransition(this,SIGNAL(jumpToMainWindowState()),_mainWindowState);

  // Переход из состояния _saveRedirectedTaskToDbState в _mainWindowState
  _saveRedirectedTaskToDbState->addTransition(this,SIGNAL(jumpToMainWindowState()),_mainWindowState);

  // Переход из состояния _saveRedirectedTaskToDbState в _redirectTaskWindowState
  _saveRedirectedTaskToDbState->addTransition(this,SIGNAL(jumpToRedirectTaskWindowState()),_redirectTaskWindowState);

  // Переход из состояния _saveRedirectedTaskToDbState в _mainWindowInfoRefreshState
  _saveRedirectedTaskToDbState->addTransition(this,SIGNAL(jumpToMainWindowInfoRefreshState()),_mainWindowInfoRefreshState);

  // Переход из состояния _mainWindowState в _editTaskInfoRefreshState по нажатию кнопки
  // "Поручение -> Редактировать" в меню действий
  _mainWindowState->addTransition(_mainWindow->ui->editTaskBt,SIGNAL(clicked(bool)),_editTaskInfoRefreshState);

  // Переход из состояния _editTaskInfoRefreshState в _editTaskImplementerState
  _editTaskInfoRefreshState->addTransition(this,SIGNAL(jumpToEditTaskImplementerState()),_editTaskImplementerState);

  // Переход из состояния _editTaskInfoRefreshState в _editTaskAuthorState
  _editTaskInfoRefreshState->addTransition(this,SIGNAL(jumpToEditTaskAuthorState()),_editTaskAuthorState);

  // Переход из состояния _editTaskInfoRefreshState в _mainWindowState
  _editTaskInfoRefreshState->addTransition(this,SIGNAL(jumpToMainWindowState()),_mainWindowState);

  // Переход из состояния _editTaskImplementerState в _saveEditedTaskState по нажатию кнопки "Сохранить"
  _editTaskImplementerState->addTransition(_createTaskWindow->ui->createBt,SIGNAL(clicked(bool)),_saveEditedTaskState);

  // Переход из состояния _editTaskAuthorState в _saveEditedTaskState по нажатию кнопки "Сохранить"
  _editTaskAuthorState->addTransition(_createTaskWindow->ui->createBt,SIGNAL(clicked(bool)),_saveEditedTaskState);

  // Переход из состояния _editTaskImplementerState в _mainWindowState по нажатию кнопки "Отмена"
  _editTaskImplementerState->addTransition(_createTaskWindow->ui->canselBt,SIGNAL(clicked(bool)),_mainWindowState);

  // Переход из состояния _editTaskAuthorState в _mainWindowState по нажатию кнопки "Отмена"
  _editTaskAuthorState->addTransition(_createTaskWindow->ui->canselBt,SIGNAL(clicked(bool)),_mainWindowState);

  // Переход из состояния _editTaskAuthorState в _selectAccompliceWindowState по нажатию кнопки "Выбор соисполнителей"
  _editTaskAuthorState->addTransition(_createTaskWindow->ui->selectAccompliceBt,SIGNAL(clicked(bool)),_selectAccompliceWindowState);

  // Переход из состояния _editTaskImplementerState в _mainWindowState по закрытию окна
  _editTaskImplementerState->addTransition(_createTaskWindow,SIGNAL(closeSignal()),_mainWindowState);

  // Переход из состояния _editTaskAuthorState в _mainWindowState по закрытию окна
  _editTaskAuthorState->addTransition(_createTaskWindow,SIGNAL(closeSignal()),_mainWindowState);

  // Переход из состояния _saveEditedTaskState в _mainWindowState
  _saveEditedTaskState->addTransition(this,SIGNAL(jumpToMainWindowState()),_mainWindowState);

  // Переход из состояния _saveEditedTaskState в _mainWindowInfoRefreshState
  _saveEditedTaskState->addTransition(this,SIGNAL(jumpToMainWindowInfoRefreshState()),_mainWindowInfoRefreshState);

  // Переход из состояния _saveEditedTaskState в _editTaskAuthorState
  _saveEditedTaskState->addTransition(this,SIGNAL(jumpToEditTaskAuthorState()),_editTaskAuthorState);

  // Переход из состояния _saveEditedTaskState в _editTaskImplementerState
  _saveEditedTaskState->addTransition(this,SIGNAL(jumpToEditTaskImplementerState()),_editTaskImplementerState);

  // Переход из _showSelectedAccRedirectTask в _redirectTaskWindowState
  _showSelectedAccRedirectTask->addTransition(this,SIGNAL(jumpToRedirectTaskWindowState()),_redirectTaskWindowState);

  // Переход из _mainWindowState в _cancelTaskState по нажатию "Снять поручение" в меню действий
  _mainWindowState->addTransition(_mainWindow->ui->cancelTaskBt,SIGNAL(clicked(bool)),_cancelTaskState);

  // Переход из _cancelTaskState в _mainWindowInfoRefreshState
  _cancelTaskState->addTransition(this,SIGNAL(jumpToMainWindowInfoRefreshState()),_mainWindowInfoRefreshState);

  // Переход из _cancelTaskState в _mainWindowState
  _cancelTaskState->addTransition(this,SIGNAL(jumpToMainWindowState()),_mainWindowState);

  // Переход из _mainWindowState в _reportTaskInfoRefreshState по нажатию "Отчитаться" в меню действий
  _mainWindowState->addTransition(_mainWindow->ui->reportTaskBt, SIGNAL(clicked(bool)),_reportTaskInfoRefreshState);

  // Переход из _reportTaskState в _mainWindowState по нажатию кнопки "Отмена"
  _reportTaskState->addTransition(_reportTaskWindow->ui->cancelBt, SIGNAL(clicked(bool)),_mainWindowState);

  // Переход из _reportTaskState в _mainWindowState по закрытию окна
  _reportTaskState->addTransition(_reportTaskWindow,SIGNAL(closeSignal()),_mainWindowState);

  // Переход из _reportTaskState в _saveReportState по нажатию кнопки "Создать"
  _reportTaskState->addTransition(_reportTaskWindow->ui->createBt,SIGNAL(clicked(bool)),_saveReportState);

  // Переход из _saveReportState в _mainWindowInfoRefreshState по сигналу автомата
  _saveReportState->addTransition(this,SIGNAL(jumpToMainWindowInfoRefreshState()),_mainWindowInfoRefreshState);

  // Переход из _saveReportState в _reportTaskState по сигналу автомата
  _saveReportState->addTransition(this,SIGNAL(jumpToReportTaskState()),_reportTaskState);

  // Переход из состояния _mainWindowState в _applyDeclineReportState по нажатию "Принять/отклнить отчет" в меню действий
  _mainWindowState->addTransition(_mainWindow->ui->applyDeclineReportBt,SIGNAL(clicked(bool)),_applyDeclineReportState);

  // Переход из _applyDeclineReportState в _mainWindowState по нажатию кнопки "Отмена"
  _applyDeclineReportState->addTransition(_reportTaskWindow->ui->cancelBt,SIGNAL(clicked(bool)),_mainWindowInfoRefreshState);

  // Переход из _applyDeclineReportState в _mainWindowState по закртию окна отчета
  _applyDeclineReportState->addTransition(_reportTaskWindow,SIGNAL(closeSignal()),_mainWindowInfoRefreshState);

  // Переход из _applyDeclineReportState в _applyReportState по нажатию кнопки "Принять"
  _applyDeclineReportState->addTransition(_reportTaskWindow->ui->createBt,SIGNAL(clicked(bool)),_applyReportState);

  // Переход из _applyDeclineReportState в _declineReportState по нажатию кнопки "Отклонить"
  _applyDeclineReportState->addTransition(_reportTaskWindow->ui->declineReportBt,SIGNAL(clicked(bool)),_declineReportState);

  // Переход из _declineReportState в _applyDeclineReportState
  _declineReportState->addTransition(this,SIGNAL(jumpToApplyDeclineReportState()),_applyDeclineReportState);

  // Переход из _declineReportState в _mainWindowInfoRefreshState
  _declineReportState->addTransition(this,SIGNAL(jumpToMainWindowInfoRefreshState()),_mainWindowInfoRefreshState);

  // Переход из _applyReportState в _mainWindowInfoRefreshState
  _applyReportState->addTransition(this,SIGNAL(jumpToMainWindowInfoRefreshState()),_mainWindowInfoRefreshState);

  // Переход из _reportTaskState в _selectFilesReportState по нажатию кнопки выбора файлов отчета
  _reportTaskState->addTransition(_reportTaskWindow->ui->selectFilesBt, SIGNAL(clicked(bool)),_selectFilesReportState);

  // Переход из _selectFilesReportState в _reportTaskState
  _selectFilesReportState->addTransition(this,SIGNAL(jumpToReportTaskState()),_reportTaskState);

  // Переход из _reportTaskInfoRefreshState в _reportTaskState по сигналу автомата
  _reportTaskInfoRefreshState->addTransition(this,SIGNAL(jumpToReportTaskState()),_reportTaskState);


  //============= Переходы в состояние _exitState====================================================

  // Перходы в _exitState
  _controllerInitState->addTransition(this,SIGNAL(jumpToExitState()),_exitState);
  _checkLoginPassState->addTransition(this,SIGNAL(jumpToExitState()),_exitState);
  _loginState->addTransition(this,SIGNAL(jumpToExitState()),_exitState);
  _loginShowPassState->addTransition(this,SIGNAL(jumpToExitState()),_exitState);
  _checkLoginPassState->addTransition(this,SIGNAL(jumpToExitState()),_exitState);
  _loginPassWrongState->addTransition(this,SIGNAL(jumpToExitState()),_exitState);
  _mainWindowState->addTransition(this,SIGNAL(jumpToExitState()),_exitState);
  _checkLoginPassState->addTransition(_loginWindow,SIGNAL(closeSignal()),_exitState);
  _loginState->addTransition(_loginWindow,SIGNAL(closeSignal()),_exitState);
  _loginShowPassState->addTransition(_loginWindow,SIGNAL(closeSignal()),_exitState);
  _checkLoginPassState->addTransition(_loginWindow,SIGNAL(closeSignal()),_exitState);
  _loginPassWrongState->addTransition(_loginWindow,SIGNAL(closeSignal()),_exitState);
  _mainWindowState->addTransition(_mainWindow,SIGNAL(closeSignal()),_exitState);
  _mainWindowState->addTransition(_mainWindow,SIGNAL(jumpToExitState()),_exitState);
  _createTaskWindowState->addTransition(this,SIGNAL(jumpToExitState()),_exitState);
  _selectAccompliceWindowState->addTransition(this,SIGNAL(jumpToExitState()),_exitState);
  _selectFilesCreateTaskState->addTransition(this,SIGNAL(jumpToExitState()),_exitState);
  _showSelectedAccompliceState->addTransition(this,SIGNAL(jumpToExitState()),_exitState);
  _saveCreatedTaskToDbState->addTransition(this,SIGNAL(jumpToExitState()),_exitState);
  _redirectTaskWindowState->addTransition(this,SIGNAL(jumpToExitState()),_exitState);
  _mainWindowState->addTransition(_mainWindow->ui->exitBt,SIGNAL(clicked(bool)),_exitState);
  _mainWindowInfoRefreshState->addTransition(this,SIGNAL(jumpToExitState()),_exitState);
  _createTaskWindowInfoRefreshState->addTransition(this,SIGNAL(jumpToExitState()),_exitState);
  _selectAccRedirectTaskWindowState->addTransition(this,SIGNAL(jumpToExitState()),_exitState);
  _showSelectedAccRedirectTask->addTransition(this,SIGNAL(jumpToExitState()),_exitState);
  _redirectTaskWindowInfoRefreshState->addTransition(this,SIGNAL(jumpToExitState()),_exitState);
  _saveRedirectedTaskToDbState->addTransition(this,SIGNAL(jumpToExitState()),_exitState);
  _saveEditedTaskState->addTransition(this,SIGNAL(jumpToExitState()),_exitState);
  _cancelTaskState->addTransition(this,SIGNAL(jumpToExitState()),_exitState);
  _confirmCreatedTaskState->addTransition(this,SIGNAL(jumpToExitState()),_exitState);
  _reportTaskState->addTransition(this,SIGNAL(jumpToExitState()),_exitState);
  _selectFilesReportState->addTransition(this,SIGNAL(jumpToExitState()),_exitState);
  _saveReportState->addTransition(this,SIGNAL(jumpToExitState()),_exitState);
  _editTaskInfoRefreshState->addTransition(this,SIGNAL(jumpToExitState()),_exitState);
  _applyDeclineReportState->addTransition(this,SIGNAL(jumpToExitState()),_exitState);
  _applyReportState->addTransition(this,SIGNAL(jumpToExitState()),_exitState);
  _declineReportState->addTransition(this,SIGNAL(jumpToExitState()),_exitState);
  _selectFilesReportState->addTransition(this,SIGNAL(jumpToExitState()),_exitState);
  _reportTaskInfoRefreshState->addTransition(this,SIGNAL(jumpToExitState()),_exitState);

  // Переход из состояний _loginState и _loginStateShowPass в exitState по нажатию кнопки "Выход"
  _loginState->addTransition(_loginWindow->ui->cancelBt,SIGNAL(clicked()),_exitState);
  _loginShowPassState->addTransition(_loginWindow->ui->cancelBt,SIGNAL(clicked()),_exitState);
}

void GuiStateMachine::stMachineInit()
{
  // Привязываем свойства графических объектов к состояниям КА
  assignStatesPropertys();

  // Добавляем состояния в конечный автомат
  addStatesToSm();

  // Подключаем сигналы и слоты к состояниями КА
  setConnectionsSm();

  // Устанавливаем переходы КА
  setTransitionsSm();

  // Устанавливаем стартовове состояние
  _stMachine->setInitialState(_controllerInitState);
}

void GuiStateMachine::assignStatesPropertys()
{
  // Свойства объектов в состоянии _loginState
  _loginState->assignProperty(_loginWindow,"visible","true");
  _loginState->assignProperty(_loginWindow,"palette",QPalette(QColor(212, 208, 200, 255),QColor(0, 170, 255, 255)));
  _loginState->assignProperty(_loginWindow->ui->cancelBt,"visible","true");
  _loginState->assignProperty(_loginWindow->ui->loginEd,"visible","true");
  _loginState->assignProperty(_loginWindow->ui->passEd,"visible","true");
  _loginState->assignProperty(_loginWindow->ui->showPassChbx,"visible","true");
  _loginState->assignProperty(_loginWindow->ui->passLbl,"visible","true");
  _loginState->assignProperty(_loginWindow->ui->mainLbl,"font",QFont("Ms  Shell Dlg 2", 14));
  _loginState->assignProperty(_loginWindow->ui->userNameLbl,"font",QFont("Ms  Shell Dlg 2", 14));
  _loginState->assignProperty(_loginWindow->ui->userNameLbl,"geometry",QRect(40, 91, 161, 21));
  _loginState->assignProperty(_loginWindow->ui->okBt,"geometry",QRect(120, 240, 101, 31));
  _loginState->assignProperty(_loginWindow->ui->okBt,"text","Вход");
  _loginState->assignProperty(_loginWindow->ui->mainLbl,"text",tr("Регистрация пользователя в системе"));
  _loginState->assignProperty(_loginWindow->ui->mainLbl,"geometry",QRect(90, 31, 361, 21));
  _loginState->assignProperty(_loginWindow->ui->userNameLbl,"text",tr("Имя пользователя"));
  _loginState->assignProperty(_loginWindow->ui->passEd,"echoMode","Password");
  _loginState->assignProperty(_loginWindow->ui->showPassChbx,"checked",false);

  // Свойства объектов в состоянии _loginShowPassState
  _loginShowPassState->assignProperty(_loginWindow,"visible","true");
  _loginShowPassState->assignProperty(_loginWindow,"palette",QPalette(QColor(212, 208, 200, 255),QColor(0, 170, 255, 255)));
  _loginShowPassState->assignProperty(_loginWindow->ui->cancelBt,"visible","true");
  _loginShowPassState->assignProperty(_loginWindow->ui->loginEd,"visible","true");
  _loginShowPassState->assignProperty(_loginWindow->ui->passEd,"visible","true");
  _loginShowPassState->assignProperty(_loginWindow->ui->showPassChbx,"visible","true");
  _loginShowPassState->assignProperty(_loginWindow->ui->passLbl,"visible","true");
  _loginShowPassState->assignProperty(_loginWindow->ui->mainLbl,"font",QFont("Ms  Shell Dlg 2", 14));
  _loginShowPassState->assignProperty(_loginWindow->ui->userNameLbl,"font",QFont("Ms  Shell Dlg 2", 14));
  _loginShowPassState->assignProperty(_loginWindow->ui->userNameLbl,"geometry",QRect(40, 91, 161, 21));
  _loginShowPassState->assignProperty(_loginWindow->ui->okBt,"geometry",QRect(120, 240, 101, 31));
  _loginShowPassState->assignProperty(_loginWindow->ui->okBt,"text",tr("Вход"));
  _loginShowPassState->assignProperty(_loginWindow->ui->mainLbl,"text",tr("Регистрация пользователя в системе"));
  _loginShowPassState->assignProperty(_loginWindow->ui->mainLbl,"geometry",QRect(90, 31, 361, 21));
  _loginShowPassState->assignProperty(_loginWindow->ui->userNameLbl,"text",tr("Имя пользователя"));
  _loginShowPassState->assignProperty(_loginWindow->ui->passEd,"echoMode","Normal");
  _loginShowPassState->assignProperty(_loginWindow->ui->showPassChbx,"checked",true);

  // Свойства объектов в состоянии _loginPassWrongState
  _loginPassWrongState->assignProperty(_loginWindow,"visible","true");
  _loginPassWrongState->assignProperty(_loginWindow,"palette",QPalette(QColor(212, 208, 200, 255), QColor(255, 89, 100, 255)));
  _loginPassWrongState->assignProperty(_loginWindow->ui->cancelBt,"visible","false");
  _loginPassWrongState->assignProperty(_loginWindow->ui->loginEd,"visible","false");
  _loginPassWrongState->assignProperty(_loginWindow->ui->passEd,"visible","false");
  _loginPassWrongState->assignProperty(_loginWindow->ui->showPassChbx,"visible","false");
  _loginPassWrongState->assignProperty(_loginWindow->ui->passLbl,"visible","false");
  _loginPassWrongState->assignProperty(_loginWindow->ui->mainLbl,"font",QFont("Ms  Shell Dlg 2", 14));
  _loginPassWrongState->assignProperty(_loginWindow->ui->userNameLbl,"font",QFont("Ms  Shell Dlg 2", 14));
  _loginPassWrongState->assignProperty(_loginWindow->ui->userNameLbl,"geometry",QRect(160,90,281,21));
  _loginPassWrongState->assignProperty(_loginWindow->ui->okBt,"geometry",QRect(200,180,101,31));
  _loginPassWrongState->assignProperty(_loginWindow->ui->okBt,"text",tr("Ок"));
  _loginPassWrongState->assignProperty(_loginWindow->ui->mainLbl,"text",tr("Ошибка ввода регистрационных данных"));
  _loginPassWrongState->assignProperty(_loginWindow->ui->userNameLbl,"text",tr("Попробуйте еще раз"));

  // Свойства объектов в состоянии _mainWindowState
  _mainWindowState->assignProperty(_loginWindow,"visible","false");
  _mainWindowState->assignProperty(_mainWindow,"visible",true);
  _mainWindowState->assignProperty(_loginWindow,"visible", false);
  _mainWindowState->assignProperty(_createTaskWindow,"visible", false);
  _mainWindowState->assignProperty(_redirectTaskWindow,"visible", false);
  _mainWindowState->assignProperty(_createTaskWindow->ui->includeToOrderChBx,"checked", false);
  _mainWindowState->assignProperty(_createTaskWindow->ui->includeToProjectChBx,"checked", false);
  _mainWindowState->assignProperty(_createTaskWindow->ui->descriptionTxtEd,"plainText","");
  _mainWindowState->assignProperty(_createTaskWindow->ui->commentTxtEd,"plainText","");
  _mainWindowState->assignProperty(_confirmCreatedTaskWindow,"visible", false);
  _mainWindowState->assignProperty(_reportTaskWindow,"visible",false);

  // Свойства объектов в состоянии _createTaskWindowState
  _createTaskWindowState->assignProperty(_createTaskWindow,"visible", true);
  _createTaskWindowState->assignProperty(_selectAccompliceWindow,"visible", false);
  _createTaskWindowState->assignProperty(_selectAccompliceWindow,"visible", false);
  _createTaskWindowState->assignProperty(_createTaskWindow->ui->selectedAccompliceLst,"enabled", true);
  _createTaskWindowState->assignProperty(_createTaskWindow->ui->selectResponsibleCmBx,"enabled", true);
  _createTaskWindowState->assignProperty(_createTaskWindow->ui->descriptionTxtEd,"enabled", true);
  _createTaskWindowState->assignProperty(_createTaskWindow->ui->deadLineDatEd,"enabled", true);
  _createTaskWindowState->assignProperty(_createTaskWindow->ui->includeToOrderChBx,"enabled", true);
  _createTaskWindowState->assignProperty(_createTaskWindow->ui->includeToProjectChBx,"enabled", true);
  _createTaskWindowState->assignProperty(_createTaskWindow->ui->selectImportanceCmBx,"enabled", true);
  _createTaskWindowState->assignProperty(_createTaskWindow->ui->selectedFilesLst,"enabled", true);
  _createTaskWindowState->assignProperty(_createTaskWindow->ui->createBt,"text",tr("СОЗДАТЬ"));
  _createTaskWindowState->assignProperty(_createTaskWindow->ui->windowNameLbl, "text", tr("Создание нового поручения"));
  _createTaskWindowState->assignProperty(_createTaskWindow, "windowTitle", tr("Создание нового поручения"));
  _createTaskWindowState->assignProperty(_confirmCreatedTaskWindow,"visible", false);

  // Свойства объектов в состоянии _selectAccompliceWindowState
  _selectAccompliceWindowState->assignProperty(_selectAccompliceWindow,"visible", true);

  // Свойства объектов в состоянии _redirectTaskWindowState
  _redirectTaskWindowState->assignProperty(_redirectTaskWindow,"visible", true);
  _redirectTaskWindowState->assignProperty(_selectAccompliceWindow,"visible", false);

  // Свойства объектов в состоянии _selectAccRedirectTaskWindowState
  _selectAccRedirectTaskWindowState->assignProperty(_selectAccompliceWindow,"visible", true);

  // Свойства объектов в состоянии _editTaskAuthorState
  _editTaskAuthorState->assignProperty(_createTaskWindow, "visible", true);
  _editTaskAuthorState->assignProperty(_createTaskWindow->ui->descriptionTxtEd,"enabled", false);
  _editTaskAuthorState->assignProperty(_createTaskWindow->ui->deadLineDatEd,"enabled", true);
  _editTaskAuthorState->assignProperty(_createTaskWindow->ui->includeToOrderChBx,"enabled", true);
  _editTaskAuthorState->assignProperty(_createTaskWindow->ui->includeToProjectChBx,"enabled", true);
  //_editTaskAuthorState->assignProperty(_createTaskWindow->ui->selectOrderCmBx,"enabled", true);
  //_editTaskAuthorState->assignProperty(_createTaskWindow->ui->selectProjectCmBx,"enabled", true);
  _editTaskAuthorState->assignProperty(_createTaskWindow->ui->selectedAccompliceLst,"enabled", true);
  _editTaskAuthorState->assignProperty(_createTaskWindow->ui->selectResponsibleCmBx,"enabled", true);
  _editTaskAuthorState->assignProperty(_createTaskWindow->ui->selectImportanceCmBx,"enabled", true);
  _editTaskAuthorState->assignProperty(_createTaskWindow->ui->selectedFilesLst,"enabled", true);
  _editTaskAuthorState->assignProperty(_createTaskWindow->ui->createBt,"text",tr("СОХРАНИТЬ"));
  _editTaskAuthorState->assignProperty(_createTaskWindow->ui->windowNameLbl, "text", tr("Редактирование поручения"));
  _editTaskAuthorState->assignProperty(_createTaskWindow, "windowTitle", tr("Редактирование поручения"));
  _editTaskAuthorState->assignProperty(_selectAccompliceWindow, "visible", false);

  // Свойства объектов в состоянии _editTaskImplementerState
  _editTaskImplementerState->assignProperty(_createTaskWindow, "visible", true);
  _editTaskImplementerState->assignProperty(_createTaskWindow->ui->descriptionTxtEd,"enabled", false);
  _editTaskImplementerState->assignProperty(_createTaskWindow->ui->deadLineDatEd,"enabled", false);
  _editTaskImplementerState->assignProperty(_createTaskWindow->ui->includeToOrderChBx,"enabled", false);
  _editTaskImplementerState->assignProperty(_createTaskWindow->ui->includeToProjectChBx,"enabled", false);
  _editTaskImplementerState->assignProperty(_createTaskWindow->ui->selectOrderCmBx,"enabled", false);
  _editTaskImplementerState->assignProperty(_createTaskWindow->ui->selectProjectCmBx,"enabled", false);
  _editTaskImplementerState->assignProperty(_createTaskWindow->ui->selectedAccompliceLst,"enabled", false);
  _editTaskImplementerState->assignProperty(_createTaskWindow->ui->selectResponsibleCmBx,"enabled", false);
  _editTaskImplementerState->assignProperty(_createTaskWindow->ui->selectImportanceCmBx,"enabled", false);
  _editTaskImplementerState->assignProperty(_createTaskWindow->ui->selectedFilesLst,"enabled", false);
  _editTaskImplementerState->assignProperty(_createTaskWindow->ui->createBt,"text",tr("СОХРАНИТЬ"));
  _editTaskImplementerState->assignProperty(_createTaskWindow->ui->windowNameLbl, "text", tr("Редактирование поручения"));
  _editTaskImplementerState->assignProperty(_createTaskWindow, "windowTitle", tr("Редактирование поручения"));

  // Свойства объектов в состоянии _confirmCreatedTaskState
  _confirmCreatedTaskState->assignProperty(_confirmCreatedTaskWindow,"visible", true);

  // Свойства объектов в состоянии _reportTaskState
  _reportTaskState->assignProperty(_reportTaskWindow,"visible",true);
  _reportTaskState->assignProperty(_reportTaskWindow->ui->declineReportBt,"visible",false);
  _reportTaskState->assignProperty(_reportTaskWindow->ui->selectFilesBt,"visible",true);
  _reportTaskState->assignProperty(_reportTaskWindow,"windowTitle",tr("Создание отчета по поручению"));
  _reportTaskState->assignProperty(_reportTaskWindow->ui->windowNameLbl, "text",tr("Создание отчета по поручению"));
  _reportTaskState->assignProperty(_reportTaskWindow->ui->createBt, "text",tr("СОЗДАТЬ"));
  _reportTaskState->assignProperty(_reportTaskWindow->ui->commentTxtEd, "readOnly", true);
  _reportTaskState->assignProperty(_reportTaskWindow->ui->reportTxtEd, "readOnly", false);
  _reportTaskState->assignProperty(_reportTaskWindow->ui->setIsOverdueChBx,"visible",false);
  _reportTaskState->assignProperty(_reportTaskWindow->ui->setIsOverdueChBx,"checked",false);

  // Свойства объектов в состоянии _applyDeclineReportState
  _applyDeclineReportState->assignProperty(_reportTaskWindow,"visible",true);
  _applyDeclineReportState->assignProperty(_reportTaskWindow->ui->declineReportBt,"visible",true);
  _applyDeclineReportState->assignProperty(_reportTaskWindow->ui->selectFilesBt,"visible",false);
  _applyDeclineReportState->assignProperty(_reportTaskWindow,"windowTitle",tr("Принять/отклонить отчет по поручению"));
  _applyDeclineReportState->assignProperty(_reportTaskWindow->ui->windowNameLbl, "text",tr("Принять/отклонить отчет по поручению"));
  _applyDeclineReportState->assignProperty(_reportTaskWindow->ui->createBt, "text",tr("ПРИНЯТЬ"));
  _applyDeclineReportState->assignProperty(_reportTaskWindow->ui->commentTxtEd, "readOnly", false);
  _applyDeclineReportState->assignProperty(_reportTaskWindow->ui->reportTxtEd, "readOnly", true);
  _applyDeclineReportState->assignProperty(_reportTaskWindow->ui->setIsOverdueChBx,"visible",false);
  _applyDeclineReportState->assignProperty(_reportTaskWindow->ui->setIsOverdueChBx,"checked",false);
}

void GuiStateMachine::controllerInitStateHnd()
{
  try
  {
    // Инициализируем ресурсы, устанавливаем соединения с серверами
    General::controllerInit();
    // Вписываем имя пользователя в loginWindow из файла настроек
    Settings* settings = General::getSettings();
    QString login = settings->getDefaultLogin();
    _loginWindow->ui->loginEd->setText(login);
    qDebug()<<"initialization complete";
  }
  catch(ConnectException& ex)
  {
    QMessageBox::critical(0, qApp->tr("ОШИБКА"),qApp->tr("Ошибка подключения к MySQL Server"), QMessageBox::Ok);
    qCritical()<<ex.getInfo();
    qCritical()<<"Init fail, app will be close";
    emit(jumpToExitState());
  }
  catch(SqlQueryException& ex)
  {
    QMessageBox::critical(0, qApp->tr("ОШИБКА"),qApp->tr("Ошибка создания базы данных"), QMessageBox::Ok);
    qCritical()<<ex.getInfo();
    qCritical()<<"Init fail, app will be close";
    emit(jumpToExitState());
  }
  catch(InitException& ex)
  {
    QMessageBox::critical(0, qApp->tr("ОШБИКА"),qApp->tr("Ошибка инициализации подсистемы времени"), QMessageBox::Ok);
    qCritical()<<ex.getInfo();
    qCritical()<<"Init fail, app will be close";
    emit(jumpToExitState());
  }
  catch(SettingsFileException& ex)
  {
    QMessageBox::critical(0, qApp->tr("ОШБИКА"),qApp->tr("Настроечный файл не найден или не корректен"), QMessageBox::Ok);
    qCritical()<<ex.getInfo();
    qCritical()<<"Init fail, app will be close";
    emit(jumpToExitState());
  }
  catch(FtpException& ex)
  {
    QMessageBox::critical(0, qApp->tr("ОШИБКА"),qApp->tr("Ошибка подключения к FTP серверу с файлами"), QMessageBox::Ok);
    qCritical()<<ex.getInfo();
    qCritical()<<"Init fail, app will be close";
    emit(jumpToExitState());
  }
  catch(TestFailed& ex)
  {
    QMessageBox::critical(0, qApp->tr("ОШИБКА"),qApp->tr("test failed"), QMessageBox::Ok);
    qCritical()<<ex.getInfo();
    qCritical()<<"Init fail, app will be close";
    emit(jumpToExitState());
  }
  catch(...)
  {
    qCritical()<<"catch unwaited exception";
    QMessageBox::critical(0, tr("ОШБИКА"),tr("Критическая ошибка"), QMessageBox::Ok);
    emit(jumpToExitState());
  }
  emit(jumpToLoginState());
}

void GuiStateMachine::checkLoginPassHnd()
{
  qDebug()<<"enter checkLoginPass state";
  // Проверяем введенные пользовтелем данные
  try
  {
    if(_loginWindow->ui->loginEd->text().isEmpty())
    {
      throw ExecutionAborted("empty login");
    }
    Worker worker = _sqlHardWorker->getWorker(_loginWindow->ui->loginEd->text());
    if(_loginWindow->ui->passEd->text()== worker.getPass()
       &&(_loginWindow->ui->loginEd->text()== worker.getLogin()))
    {
      General::setRegistredUserId(worker.getId());
      // Отправляем сигнал автомату, что данные корректны
      qDebug()<<"login pass succ";
      emit(jumpToMainWindowState());
    }
    else
    {
      throw ExecutionAborted("pass wrong");
    }
  }// обрабатываем исключение генерирующееся в случае, если пользователь не найден в БД
  catch(ExecutionAborted& ex)
  {
    qDebug()<<ex.getInfo();
    qDebug()<<"login = "<<_loginWindow->ui->loginEd->text()
            <<" pass = "<<_loginWindow->ui->passEd->text();
    // Очищаем поле ввода пароля
    _loginWindow->ui->passEd->clear();
    // Отправляем сигнал автомату, что данные некорректны
    emit(jumpToLoginPassWrongState());
  }
  catch(SqlQueryException& ex)
  {
    qCritical()<<ex.getInfo();
    QMessageBox::critical(0, tr("ОШБИКА"),tr("Ошибка запроса к БД"), QMessageBox::Ok);
    // если не смогли залогиниться, то дальше работать не можем
    // переходим в _exitState
    emit(jumpToExitState());
  }
  catch(Exception& ex)
  {
    qCritical()<<"catch unwaited exception";
    qCritical()<<ex.getInfo();
    QMessageBox::critical(0, tr("ОШБИКА"),tr("Критическая ошибка"), QMessageBox::Ok);
    // если не смогли залогиниться, то дальше работать не можем
    // переходим в _exitState
    emit(jumpToExitState());
  }
}

void GuiStateMachine::mainWindowInfoRefreshStateHnd()
{
  qDebug()<<"enter mainWindowInfoRefreshState";
  try
  {
    // актуализируем информацию в главном окне
    _mainWindow->infoRefresh();
  }
  catch(SqlQueryException& ex)
  {
    qCritical()<<ex.getInfo();
    QMessageBox::critical(0, tr("ОШБИКА"),tr("Ошибка запроса к БД"), QMessageBox::Ok);
  }
  catch(Exception& ex)
  {
    qCritical()<<"catch unwaited exception";
    qCritical()<<ex.getInfo();
    QMessageBox::critical(0, tr("ОШБИКА"),tr("Критическая ошибка"), QMessageBox::Ok);
    emit(jumpToExitState());
  }
  emit(jumpToMainWindowState());
}

void GuiStateMachine::mainWindowStateHnd()
{
  qDebug()<<"enter mainWindowState";
  try
  {
    // отображаем детальную информацию о выделенном поручении
    try{_mainWindow->showDetailSelectedTaskInfo();}
    catch(ExecutionAborted){}

    // изменяем состояние пунктов меню "Поручение"
    _mainWindow->changeTaskActionBar();

    // если поручение еще не просматривалось исполнителем
    // то изменяем статус поручения
    _mainWindow->refreshStatusTask();

    // обновляем информацию в главном окне, после изменения
    // статуса поручения
    emit(jumpToMainWindowInfoRefreshState());
  }
  catch(ExecutionAborted){}// если обновлять статус не нужно
  catch(Exception& ex)
  {
    qCritical()<<"catch unwaited exception";
    qCritical()<<ex.getInfo();
    QMessageBox::critical(0, tr("ОШБИКА"),tr("Критическая ошибка"), QMessageBox::Ok);
    emit(jumpToExitState());
  }
}

void GuiStateMachine::createTaskWindowInfoRefreshStateHnd()
{
  qDebug()<<"enter createTaskWindowInfoRefreshState";
  // Обновляем информацию
  try
  {
    _createTaskWindow->infoRefresh();
  }
  catch(SqlQueryException& ex)
  {
    qCritical()<<ex.getInfo();
    QMessageBox::critical(0, tr("ОШБИКА"),tr("Ошибка запроса к БД"), QMessageBox::Ok);
  }
  catch(Exception& ex)
  {
    qCritical()<<"catch unwaited exception";
    qCritical()<<ex.getInfo();
    QMessageBox::critical(0, tr("ОШБИКА"),tr("Критическая ошибка"), QMessageBox::Ok);
    emit(jumpToExitState());
  }
  emit(jumpTocreateTaskWindowState());
}

void GuiStateMachine::createTaskWindowStateHnd()
{
  qDebug()<<"enter createTaskWindowState";
  _savePointState->setDefaultState(_createTaskWindowState);
}

void GuiStateMachine::checkCreatedTaskForReadyToSaveHnd()
{
  try
  {
    // Проверяем что все необходимые поля окна создания поручения заполнены
    _createTaskWindow->isCreatedTaskReadyToSave();
  }
  catch(ExecutionAborted& ex)
  {
    qDebug()<<ex.getInfo();
    emit(jumpTocreateTaskWindowState());
  }
  emit(jumpToConfirmCreatedTaskState());
}

void GuiStateMachine::confirmCreatedTaskStateHnd()
{
  qDebug()<<"enter confirmCreatedTaskState";
  try
  {
    // Создаем поручение и заполняем его поля введенной информацией
    Task task;
    _createTaskWindow->fillCreatedTask(task);
    _confirmCreatedTaskWindow->infoRefreshFromTask(task);
  }
  catch(Exception& ex)
  {
    qCritical()<<"catch unwaited exception";
    qCritical()<<ex.getInfo();
    QMessageBox::critical(0, tr("ОШБИКА"),tr("Критическая ошибка"), QMessageBox::Ok);
    emit(jumpToExitState());
  }
}

void GuiStateMachine::selectAccompliceWindowStateHnd()
{
  qDebug()<<"enter selectAccompliceWindowState";
  try
  {

    // Если список ответственных пуст
    if(0 == _createTaskWindow->ui->selectResponsibleCmBx->count())
    {
      throw ExecutionAborted("responsible list is empty");
    }
    bool ok = false;
    // Достаем Id работника, котоырй выбран ответвенным за поручение
    quint32 currentResponsibleId = _createTaskWindow->ui->selectResponsibleCmBx->currentData().toUInt(&ok);
    if(!ok)
    {
      qCritical()<<"currentResponsible to uint convert error";
      throw CriticalOperationException("critical operation");
    }

    // Обновляем список соисполнителей в окне выбора соисполнителей
    _selectAccompliceWindow->infoRefresh(currentResponsibleId);

    qint32 listWorkersSize = _selectAccompliceWindow->ui->accompliceLst->count();
    // Расставляем галочки в зависимости от того, какие работники уже были выбраны соисполнителями
    for(qint32 i = 0; i < listWorkersSize; ++i)
    {
      _selectAccompliceWindow->ui->accompliceLst->item(i)->setCheckState(Qt::Unchecked);
      qint32 selectedWorkersSize = _createTaskWindow->ui->selectedAccompliceLst->count();
      for(qint32 j =0; j < selectedWorkersSize; ++j)
      {
        if(_createTaskWindow->ui->selectedAccompliceLst->item(j)->text() == _selectAccompliceWindow->ui->accompliceLst->item(i)->text())
        {
          _selectAccompliceWindow->ui->accompliceLst->item(i)->setCheckState(Qt::Checked);
        }
      }
    }
  }
  // если список соисполнителей пуст
  catch(ExecutionAborted){}
  catch(SqlQueryException& ex)
  {
    qCritical()<<ex.getInfo();
    QMessageBox::critical(0, tr("ОШБИКА"),tr("Ошибка запроса к БД"), QMessageBox::Ok);
  }
  catch(Exception& ex)
  {
    qCritical()<<"catch unwaited exception";
    qCritical()<<ex.getInfo();
    QMessageBox::critical(0, tr("ОШБИКА"),tr("Критическая ошибка"), QMessageBox::Ok);
    emit(jumpToExitState());
  }
}

void GuiStateMachine::showSelectedAccompliceHnd()
{
  qDebug()<<"enter showSelectedAccompliceState";
  _createTaskWindow->ui->selectedAccompliceLst->clear();
  qint32 size =_selectAccompliceWindow->ui->accompliceLst->count();
  // Перебираем все элементы, отображаем выбранные в окне _createTaskWindow
  for(qint32 i =0; i < size; ++i)
  {
    if(Qt::Checked == _selectAccompliceWindow->ui->accompliceLst->item(i)->checkState())
    {
      QListWidgetItem* item = new QListWidgetItem();
      item->setText(_selectAccompliceWindow->ui->accompliceLst->item(i)->text());
      item->setData(Qt::UserRole,_selectAccompliceWindow->ui->accompliceLst->item(i)->data(Qt::UserRole));
      _createTaskWindow->ui->selectedAccompliceLst->addItem(item);
    }
  }
  emit(jumpTocreateTaskWindowState());
}

void GuiStateMachine::selectFilesCreateTaskStateHnd()
{
  qDebug()<<"enter selectFilesCreateTaskState";
  // Получаем полный путь к добавленным файлам
  QStringList lst = QFileDialog::getOpenFileNames(_createTaskWindow, tr("Добавить файлы"));

  // Добавляем только те файлы, которых еще нет в списке
  foreach (QString name, lst)
  {
    if(!(_createTaskWindow->getSelectedFileNames().contains(name)))
    {
      _createTaskWindow->addSelectedFileName(name);
    }
  }
  try
  {
    // Преобразуем в короткие имена файлов и отображаем список
    _createTaskWindow->ui->selectedFilesLst->clear();
    _createTaskWindow->ui->selectedFilesLst->addItems(FtpHardWorker::getShortFileNames(_createTaskWindow->getSelectedFileNames()));
    emit(jumpTocreateTaskWindowState());
  }
  catch(Exception& ex)
  {
    qCritical()<<"catch unwaited exception";
    qCritical()<<ex.getInfo();
    QMessageBox::critical(0, tr("ОШБИКА"),tr("Критическая ошибка"), QMessageBox::Ok);
    emit(jumpToExitState());
  }
}

void GuiStateMachine::saveCreatedTaskToDbStateHnd()
{
  qDebug()<<"enter saveCreatedTaskToDbState";
  try
  {
    // Проверяем что все необходимые поля окна создания поручения заполнены
    //_createTaskWindow->isCreatedTaskReadyToSave();

    // Создаем поручение и заполняем его поля введенной информацией
    Task task;
    _createTaskWindow->fillCreatedTask(task);

    // Сохраняем поручение в базе данных
    _sqlHardWorker->addTask(task);

    // Отправим уведомление о создании новго поручения ответственному
    QString subjectMsg = "Получено новое поручение";
    QString textMsg = "Суть поручения: " + task.getDescription();
    Email email(task.getResponsibleDb().getEmail(),subjectMsg,textMsg);
    SmtpHardWorker* smtp = SmtpHardWorker::getInstance();
    smtp->sendEmail(email);

    // Отправим уведомление соисполнителям о создании нового поручения
    QList<Worker> acc;
    try
    {acc = task.getAccomplicesDb();}
    catch(ExecutionAborted)
    {}

    foreach (Worker w, acc)
    {
      email.setRecipient(w.getEmail());
      smtp->sendEmail(email);
    }

    // Переводим автомат в состояние главного окна
    emit(jumpToMainWindowInfoRefreshState());
  }
  catch(SqlQueryException& ex)
  {
    qCritical()<<ex.getInfo();
    QMessageBox::critical(0, tr("ОШБИКА"),tr("Ошибка запроса к БД"), QMessageBox::Ok);
    emit(jumpTocreateTaskWindowState());
  }
  catch(ExecutionAborted& ex)
  {
    qDebug()<<ex.getInfo();
    emit(jumpToConfirmCreatedTaskState());
  }
  catch(FtpException& ex)
  {
    qCritical()<<ex.getInfo();
    QMessageBox::critical(0, tr("ОШБИКА"),tr("Ошибка загрузки файлов на FTP сервер"), QMessageBox::Ok);
    emit(jumpToConfirmCreatedTaskState());
  }
  catch(SmtpException& ex)
  {
    qCritical()<<ex.getInfo();
    QMessageBox::critical(0, tr("ОШБИКА"),tr("Не удалось отправить уведомление по email"), QMessageBox::Ok);
    emit(jumpToMainWindowInfoRefreshState());
  }
  catch(Exception& ex)
  {
    qCritical()<<"catch unwaited exception";
    qCritical()<<ex.getInfo();
    QMessageBox::critical(0, tr("ОШБИКА"),tr("Критическая ошибка"), QMessageBox::Ok);
    emit(jumpToExitState());
  }
}

void GuiStateMachine::redirectTaskWindowInfoRefreshStateHnd()
{
  qDebug()<<"enter redirectTaskWindowState";
  try
  {
    // Получаем id выделенного поручения на активной вкладке
    quint32 currentTaskId = 0;
    try{currentTaskId = _mainWindow->getCurrentTaskId();}
    catch(ExecutionAborted)
    {
      // не должны сюда попасть, т.к. если поручение не выделено,
      // кнопка "перепоручить" заблокирована
      QString msg = "task not selected";
      qCritical()<<msg;
      throw NeedFixCode(msg);
    }

        // Обновляем информацию в окне
    _redirectTaskWindow->infoRefresh(currentTaskId);
    emit(jumpToRedirectTaskWindowState());
  }
  catch(SqlQueryException& ex)
  {
    qCritical()<<ex.getInfo();
    QMessageBox::critical(0, tr("ОШБИКА"),tr("Ошибка запроса к БД"), QMessageBox::Ok);
    emit(jumpToMainWindowState());
  }
  catch(ExecutionAborted& ex)
  {
    qDebug()<<ex.getInfo();
    QMessageBox::critical(0, tr("ОШБИКА"),tr("Выделенное поручение не найдено в БД"), QMessageBox::Ok);
    emit(jumpToMainWindowState());
  }
  catch(Exception& ex)
  {
    qCritical()<<"catch unwaited exception";
    qCritical()<<ex.getInfo();
    QMessageBox::critical(0, tr("ОШБИКА"),tr("Критическая ошибка"), QMessageBox::Ok);
    emit(jumpToExitState());
  }
}

void GuiStateMachine::selectAccRedirectTaskWindowStateHnd()
{
  try
  {
    // Если список ответственных пуст
    if(0 == _redirectTaskWindow->ui->selectResponsibleCmBx->count())
    {
      throw ExecutionAborted("responsible list is empty");
    }
    bool ok = false;
    // Достаем Id работника, котоырй выбран ответвенным за поручение
    quint32 currentResponsibleId = _redirectTaskWindow->ui->selectResponsibleCmBx->currentData().toUInt(&ok);
    if(!ok)
    {
      qCritical()<<"currentResponsible to uint convert error";
      throw CriticalOperationException("critical operation");
    }

    // Обновляем список соисполнителей в окне выбора соисполнителей
    _selectAccompliceWindow->infoRefresh(currentResponsibleId);

    qint32 listWorkersSize = _selectAccompliceWindow->ui->accompliceLst->count();
    // Расставляем галочки в зависимости от того, какие работники уже были выбраны соисполнителями
    for(qint32 i = 0; i < listWorkersSize; ++i)
    {
      _selectAccompliceWindow->ui->accompliceLst->item(i)->setCheckState(Qt::Unchecked);
      qint32 selectedWorkersSize = _redirectTaskWindow->ui->selectedAccompliceLst->count();
      for(qint32 j =0; j < selectedWorkersSize; ++j)
      {
        if(_redirectTaskWindow->ui->selectedAccompliceLst->item(j)->text() == _selectAccompliceWindow->ui->accompliceLst->item(i)->text())
        {
          _selectAccompliceWindow->ui->accompliceLst->item(i)->setCheckState(Qt::Checked);
        }
      }
    }
  }
  // если список соисполнителей пуст
  catch(ExecutionAborted){}
  catch(SqlQueryException& ex)
  {
    qCritical()<<ex.getInfo();
    QMessageBox::critical(0, tr("ОШБИКА"),tr("Ошибка запроса к БД"), QMessageBox::Ok);
  }
  catch(Exception& ex)
  {
    qCritical()<<"catch unwaited exception";
    qCritical()<<ex.getInfo();
    QMessageBox::critical(0, tr("ОШБИКА"),tr("Критическая ошибка"), QMessageBox::Ok);
    emit(jumpToExitState());
  }
}

void GuiStateMachine::showSelectedAccRedirectTaskHnd()
{
  qDebug()<<"enter showSelectedAccompliceState";
  _redirectTaskWindow->ui->selectedAccompliceLst->clear();
  qint32 size =_selectAccompliceWindow->ui->accompliceLst->count();
  // Перебираем все элементы, отображаем выбранные в окне _createTaskWindow
  for(qint32 i =0; i < size; ++i)
  {
    if(Qt::Checked == _selectAccompliceWindow->ui->accompliceLst->item(i)->checkState())
    {
      QListWidgetItem* item = new QListWidgetItem();
      item->setText(_selectAccompliceWindow->ui->accompliceLst->item(i)->text());
      item->setData(Qt::UserRole,_selectAccompliceWindow->ui->accompliceLst->item(i)->data(Qt::UserRole));
      _redirectTaskWindow->ui->selectedAccompliceLst->addItem(item);
    }
  }
  emit(jumpToRedirectTaskWindowState());
}

void GuiStateMachine::saveRedirectedTaskToDbStateHnd()
{
  qDebug()<<"enter saveRedirectedTaskToDbState";
  try
  {
    // Проверяем что все необходимые поля окна создания поручения заполнены
    _redirectTaskWindow->isTaskReadyToSave();

    // Создаем поручение и заполняем его поля введенной информацией
    Task task;
    // Id родительского поручения для задачи
    quint32 idParentTask = _mainWindow->getCurrentTaskId();
    _redirectTaskWindow->fillTask(task, idParentTask);

    // Сохраняем поручение в базе данных
    _sqlHardWorker->addTask(task);

    // Отправим уведомление о создании новго поручения ответственному
    QString subjectMsg = "Получено новое поручение";
    QString textMsg = "Суть поручения: " + task.getDescription();
    Email email(task.getResponsibleDb().getEmail(),subjectMsg,textMsg);
    SmtpHardWorker* smtp = SmtpHardWorker::getInstance();
    smtp->sendEmail(email);

    // Отправим уведомление соисполнителям о создании нового поручения
    QList<Worker> acc;
    try
    {acc = task.getAccomplicesDb();}
    catch(ExecutionAborted)
    {}

    foreach (Worker w, acc)
    {
      email.setRecipient(w.getEmail());
      smtp->sendEmail(email);
    }

    // Переводим автомат в состояние главного окна
    emit(jumpToMainWindowInfoRefreshState());
  }
  catch(SqlQueryException& ex)
  {
    qCritical()<<ex.getInfo();
    QMessageBox::critical(0, tr("ОШБИКА"),tr("Ошибка запроса к БД"), QMessageBox::Ok);
    emit(jumpToRedirectTaskWindowState());
  }
  catch(ExecutionAborted& ex)
  {
    qDebug()<<ex.getInfo();
    emit(jumpToRedirectTaskWindowState());
  }
  catch(FtpException& ex)
  {
    QMessageBox::critical(0, tr("ОШБИКА"),tr("Ошибка копирования файлов на FTP сервере"), QMessageBox::Ok);
    qCritical()<<ex.getInfo();
    emit(jumpToRedirectTaskWindowState());
  }
  catch(SmtpException& ex)
  {
    qCritical()<<ex.getInfo();
    QMessageBox::critical(0, tr("ОШБИКА"),tr("Не удалось отправить уведомление по email"), QMessageBox::Ok);
    emit(jumpToMainWindowInfoRefreshState());
  }
  catch(Exception& ex)
  {
    qCritical()<<"catch unwaited exception";
    qCritical()<<ex.getInfo();
    QMessageBox::critical(0, tr("ОШБИКА"),tr("Критическая ошибка"), QMessageBox::Ok);
    emit(jumpToExitState());
  }
}

void GuiStateMachine::editTaskInfoRefreshStateHnd()
{
  qDebug()<<"enter editTaskInfoRefreshState";
  // Загружаем информацию из поручения
  try
  {
    quint32 currentTaskId = 0;
    try{currentTaskId = _mainWindow->getCurrentTaskId();}
    catch(ExecutionAborted)
    {
      // не должны сюда попасть, т.к. если поручение не выделено,
      // кнопка "перепоручить" заблокирована
      QString msg = "task not selected";
      qCritical()<<msg;
      throw NeedFixCode(msg);
    }

    Task task;
    task.synhronizeWithDb(currentTaskId);

    try
    {
      _createTaskWindow->infoRefreshFromTask(task);
    }
    catch(ExecutionAborted)// если загрузка файлов поручения была отменена
    {
      emit(jumpToMainWindowState());
      return;
    }

    // Если редактирует автор поручения - он может изменить любое
    // поле, кроме поля "Суть поручения"
    if(General::getRegistredUserId() == task.getAuthorId())
    {
      // запоминаем состояние в которое должен перейти автомат
      // из общих состояний выбора соисполнителей и выбора файлов
      // при создании нового поручения и редактировании поручения
      _savePointState->setDefaultState(_editTaskAuthorState);
      emit(jumpToEditTaskAuthorState());
    }
    else// Ответственный за поручение может только добавлять комментарий
    {
      _savePointState->setDefaultState(_editTaskAuthorState);
      emit(jumpToEditTaskImplementerState());
    }
  }
  catch(SqlQueryException& ex)
  {
    qCritical()<<ex.getInfo();
    QMessageBox::critical(0, tr("ОШБИКА"),tr("Ошибка запроса к БД"), QMessageBox::Ok);
    emit(jumpToMainWindowState());
  }
  catch(FtpException& ex)
  {
    qCritical()<<ex.getInfo();
    QMessageBox::critical(0, tr("ОШБИКА"),tr("Не удалось скачать файл"), QMessageBox::Ok);
    emit(jumpToMainWindowState());
  }
  catch(ExecutionAborted& ex)
  {
    qDebug()<<ex.getInfo();
    QMessageBox::critical(0, tr("ОШБИКА"),tr("Выделенное поручение не найдено в БД"), QMessageBox::Ok);
    emit(jumpToMainWindowState());
  }
  catch(Exception& ex)
  {
    qCritical()<<"catch unwaited exception";
    qCritical()<<ex.getInfo();
    QMessageBox::critical(0, tr("ОШБИКА"),tr("Критическая ошибка"), QMessageBox::Ok);
    emit(jumpToExitState());
  }
}

void GuiStateMachine::saveEditedTaskStateHnd()
{
  qDebug()<<"enter saveEditedTaskState";
  try
  {
    quint32 currentTaskId = 0;
    try{currentTaskId = _mainWindow->getCurrentTaskId();}
    catch(ExecutionAborted)
    {
      // не должны сюда попасть, т.к. если поручение не выделено,
      // кнопка "перепоручить" заблокирована
      QString msg = "task not selected";
      qCritical()<<msg;
      throw NeedFixCode(msg);
    }

    Task task;
    Task editedTask;
    task.synhronizeWithDb(currentTaskId);
    editedTask = task;
    _createTaskWindow->fillEditedTask(editedTask);
    try
    {
      _sqlHardWorker->updateTask(editedTask,true);
    }
    catch(ExecutionAborted)// если заказчка файлов вложения поручения была отменена
    {
      // Если редактирует автор поручения - он может изменить любое
      // поле, кроме поля "Суть поручения"
      if(General::getRegistredUserId() == task.getAuthorId())
      {
        // запоминаем состояние в которое должен перейти автомат
        // из общих состояний выбора соисполнителей и выбора файлов
        // при создании нового поручения и редактировании поручения
        _savePointState->setDefaultState(_editTaskAuthorState);
        emit(jumpToEditTaskAuthorState());
      }
      else// Ответственный за поручение может только добавлять комментарий
      {
        _savePointState->setDefaultState(_editTaskAuthorState);
        emit(jumpToEditTaskImplementerState());
      }
      return;
    }

    // Если изменилися ответственный за поручение, нужно отправить ему уведомление
    // по почте
    SmtpHardWorker* smtp = SmtpHardWorker::getInstance();
    QString subjectMsg = "Получено новое поручение";
    QString textMsg = "Суть поручения: " + editedTask.getDescription();
    if(task.getResponsibleId() != editedTask.getResponsibleId())
    {
      Email email(editedTask.getResponsibleDb().getEmail(),subjectMsg,textMsg);
      smtp->sendEmail(email);
    }

    // Нужно отправить уведомление по почте всем новым соисполнителям поручения
    QList<quint32> oldAcc = task.getAccomplicesId();
    QList<quint32> newAcc = editedTask.getAccomplicesId();
    foreach (quint32 accId, newAcc)
    {
      if(!oldAcc.contains(accId))
      {
        Worker worker;
        worker.synhronizeWithDb(accId);
        Email email(worker.getEmail(),subjectMsg,textMsg);
        smtp->sendEmail(email);
      }
    }
    emit(jumpToMainWindowInfoRefreshState());
  }
  catch(SqlQueryException& ex)
  {
    qCritical()<<ex.getInfo();
    QMessageBox::critical(0, tr("ОШБИКА"),tr("Ошибка запроса к БД"), QMessageBox::Ok);
    emit(jumpToMainWindowState());
  }
  catch(ExecutionAborted& ex)
  {
    qDebug()<<ex.getInfo();
    QMessageBox::critical(0, tr("ОШБИКА"),tr("Выделенное поручение не найдено в БД"), QMessageBox::Ok);
    emit(jumpToMainWindowState());
  }
  catch(FtpException& ex)
  {
    qCritical()<<ex.getInfo();
    QMessageBox::critical(0, tr("ОШБИКА"),tr("Не удалось загрузить файл"), QMessageBox::Ok);
    emit(jumpToMainWindowState());
  }
  catch(Exception& ex)
  {
    qCritical()<<"catch unwaited exception";
    qCritical()<<ex.getInfo();
    QMessageBox::critical(0, tr("ОШБИКА"),tr("Критическая ошибка"), QMessageBox::Ok);
    emit(jumpToExitState());
  }
}

void GuiStateMachine::cancelTaskStateHnd()
{
  qDebug()<<"enter cancelTaskState";
  try
  {
    quint32 currentTaskId = 0;
    try{currentTaskId = _mainWindow->getCurrentTaskId();}
    catch(ExecutionAborted)
    {
      // не должны сюда попасть, т.к. если поручение не выделено,
      // кнопка "перепоручить" заблокирована
      QString msg = "task not selected";
      qCritical()<<msg;
      throw NeedFixCode(msg);
    }
    Task task;
    // Достаем задачу из БД
    task.synhronizeWithDb(currentTaskId);

    QMessageBox msgBox;
    msgBox.setWindowTitle(tr("Снять поручение"));
    msgBox.setText(tr("Хотите снять выбранное поручение?"));
    QPushButton* yesButton = msgBox.addButton(tr("Да"),QMessageBox::ActionRole);
    QCheckBox* checkbox = new QCheckBox(&msgBox);
    checkbox->setText(tr("отметить поручение как просроченное"));
    msgBox.addButton(tr("Нет"),QMessageBox::ActionRole);
    msgBox.setCheckBox(checkbox);
    msgBox.checkBox()->setVisible(false);

    // Если поручение было просрочено, то покажем галочку
    // "отметить поручение как просроченное"
    Clock* systemClock = General::getSystemClock();
    QDateTime now = systemClock->getTime();
    QDate today = now.date();
    if(today > task.getDeadlineDate())
    {
      msgBox.checkBox()->setVisible(true);
    }
    msgBox.exec();
    if(msgBox.clickedButton() == yesButton)
    {
      if(msgBox.checkBox()->isChecked())
      {
        task.setIsOverdue(true);
      }
      else
      {
        task.setIsOverdue(false);
      }
      // Меняем статус задачи на "Снято"
      task.setStatus(Task::canceled);
      // Обновляем задачу в БД
      _sqlHardWorker->updateTask(task,false);
    }
  }
  catch(SqlQueryException& ex)
  {
    qCritical()<<ex.getInfo();
    QMessageBox::critical(_mainWindow, tr("ОШБИКА"),tr("Ошибка запроса к БД"), QMessageBox::Ok);
    emit(jumpToMainWindowState());
  }
  catch(ExecutionAborted& ex)
  {
    qDebug()<<ex.getInfo();
    QMessageBox::critical(0, tr("ОШБИКА"),tr("Выделенное поручение не найдено в БД"), QMessageBox::Ok);
    emit(jumpToMainWindowState());
  }
  catch(Exception& ex)
  {
    qCritical()<<"catch unwaited exception";
    qCritical()<<ex.getInfo();
    QMessageBox::critical(_mainWindow, tr("ОШБИКА"),tr("Критическая ошибка"), QMessageBox::Ok);
    emit(jumpToExitState());
  }
  emit(jumpToMainWindowInfoRefreshState());
}

void GuiStateMachine::reportTaskInfoRefreshStateHnd()
{
  qDebug()<<"enter reportTaskInfoRefreshState";
  quint32 currentTaskId = 0;
  try{currentTaskId = _mainWindow->getCurrentTaskId();}
  catch(ExecutionAborted)
  {
    // не должны сюда попасть, т.к. если поручение не выделено,
    // кнопка "перепоручить" заблокирована
    QString msg = "task not selected";
    qCritical()<<msg;
    QMessageBox::critical(_mainWindow, tr("ОШБИКА"),tr("Критическая ошибка"), QMessageBox::Ok);
    emit(jumpToExitState());
    return;
  }

  try
  {
    _reportTaskWindow->setWindowState(ReportTaskWindow::createReport);
    Task task;
    // Достаем задачу из БД
    task.synhronizeWithDb(currentTaskId);
    _reportTaskWindow->infoRefreshFromTask(task);
    emit(jumpToReportTaskState());
  }
  catch(SqlQueryException& ex)
  {
    qCritical()<<ex.getInfo();
    QMessageBox::critical(_mainWindow, tr("ОШБИКА"),tr("Ошибка запроса к БД"), QMessageBox::Ok);
    emit(jumpToMainWindowState());
  }
  catch(ExecutionAborted& ex)
  {
    qDebug()<<ex.getInfo();
    QMessageBox::critical(0, tr("ОШБИКА"),tr("Выделенное поручение не найдено в БД"), QMessageBox::Ok);
    emit(jumpToMainWindowState());
  }
  catch(Exception& ex)
  {
    qCritical()<<"catch unwaited exception";
    qCritical()<<ex.getInfo();
    QMessageBox::critical(_mainWindow, tr("ОШБИКА"),tr("Критическая ошибка"), QMessageBox::Ok);
    emit(jumpToExitState());
  }
}

void GuiStateMachine:: saveReportStateHnd()
{
  qDebug()<<"enter save report state";
  // Объявляем локальные переменные
  // Id поручения по которому сохраняется отчет
  quint32 currentTaskId = 0;
  // Отчет
  Report report;
  // Таск
  Task task;

  try{currentTaskId = _mainWindow->getCurrentTaskId();}
  catch(ExecutionAborted)
  {
    // не должны сюда попасть, т.к. если поручение не выделено,
    // кнопка "перепоручить" заблокирована
    QString msg = "task not selected";
    qCritical()<<msg;
    QMessageBox::critical(0, tr("ОШБИКА"),tr("Критическая ошибка"), QMessageBox::Ok);
    emit(jumpToExitState());
    return;
  }

  if(_reportTaskWindow->ui->reportTxtEd->toPlainText().isEmpty())
  {
    QMessageBox::information(0, tr("ВНИМАНИЕ"),tr("Поле \"Текст отчета\" не может быть пустым"), QMessageBox::Ok);
    emit(jumpToReportTaskState());
    return;
  }

  try
  {
    task.synhronizeWithDb(currentTaskId);
    quint32 reportId = task.getReportId();

    // Заполняем поручение данными с формы
    _reportTaskWindow->fillReport(report);
    if(reportId == 0)
    {
      _sqlHardWorker->addReport(report, currentTaskId);
      // Теперь у отчёта появился айди
      reportId = report.getId();
      // Учтём это в объекте поручения
      task.setReportId(reportId);
    }
    else
    {
      report.setId(reportId);
      _sqlHardWorker->updateReport(report);
    }

    try
    {
      // Обновляем статус поручения на "есть заявка на отчет"
      task.setStatus(Task::isBidForReport);
      // Добавляем в комментарий текст отчета по поручению
      QString comment;
      // Пользователь, вошедший в систему
      Worker currentUser;
      currentUser.synhronizeWithDb(General::getRegistredUserId());
      Clock* systemClock = General::getSystemClock();
      QDateTime date = systemClock->getTime();
      if(!task.getComment().isEmpty())
      {
        comment = task.getComment() + "\n";
      }
      comment += "[" + date.toString("dd.MM.yy hh:mm:ss")+ " "
          + currentUser.getShortName() + "] отчет: ";
      comment += _reportTaskWindow->ui->reportTxtEd->toPlainText();
      task.setComment(comment);
      // Обновляем поручение в БД
      _sqlHardWorker->updateTask(task,false);
    }
    catch(ExecutionAborted)
    {// если поручение не найдено в БД, то беда, ломаемся
      QString msg = tr("Task with id = ")
                  + currentTaskId
                  + tr("Not found");
      qCritical()<<msg;
      throw NeedFixCode(msg);
    }

    // Отправим уведомление о создании отчета по поручению автору
    QString subjectMsg = "Получен отчет по поручению";
    QString textMsg = "Суть поручения: " + task.getDescription();
    Email email(task.getAuthorDb().getEmail(),subjectMsg,textMsg);
    SmtpHardWorker* smtp = SmtpHardWorker::getInstance();
    smtp->sendEmail(email);
    emit(jumpToMainWindowInfoRefreshState());
  }
  // это исключение получаем, нажали кнопку "отмена" при
  // загрузке файлов вложения на сервер
  catch(ExecutionAborted)
  {emit(jumpToReportTaskState());}
  catch(SqlQueryException& ex)
  {
    qCritical()<<ex.getInfo();
    QMessageBox::critical(_mainWindow, tr("ОШБИКА"),tr("Ошибка запроса к БД"), QMessageBox::Ok);
    emit(jumpToReportTaskState());
  }
  catch(SmtpException& ex)
  {
    qCritical()<<ex.getInfo();
    QMessageBox::critical(0, tr("ОШБИКА"),tr("Не удалось отправить уведомление по email"), QMessageBox::Ok);
    emit(jumpToMainWindowInfoRefreshState());
  }
  catch(Exception& ex)
  {
    qCritical()<<"catch unwaited exception";
    qCritical()<<ex.getInfo();
    QMessageBox::critical(_mainWindow, tr("ОШБИКА"),tr("Критическая ошибка"), QMessageBox::Ok);
    emit(jumpToExitState());
  }
}

void GuiStateMachine::applyDeclineReportStateHnd()
{
  qDebug()<<"enter applyDeclineReportState state";
  try
  {
    _reportTaskWindow->setWindowState(ReportTaskWindow::viewReport);
    Task task;
    task.synhronizeWithDb(_mainWindow->getCurrentTaskId());
    // Если поручение просрочено отображаем галочку
    // "пометить поручение как просроченное"
    Clock* systemClock = General::getSystemClock();
    QDateTime now = systemClock->getTime();
    QDate today = now.date();
    if(today > task.getDeadlineDate())
    {
      _reportTaskWindow->ui->setIsOverdueChBx->setVisible(true);
    }
    Report report;
    report.synhronizeWithDb(task.getReportId());
    _reportTaskWindow->infoRefreshFromTaskAndReport(task,report);
  }
  catch(SqlQueryException& ex)
  {
    qCritical()<<ex.getInfo();
    QMessageBox::critical(_mainWindow, tr("ОШБИКА"),tr("Ошибка запроса к БД"), QMessageBox::Ok);
    emit(jumpToReportTaskState());
  }
  catch(Exception& ex)
  {
    qCritical()<<"catch unwaited exception";
    qCritical()<<ex.getInfo();
    QMessageBox::critical(_mainWindow, tr("ОШБИКА"),tr("Критическая ошибка"), QMessageBox::Ok);
    emit(jumpToExitState());
  }
}

void GuiStateMachine::applyReportStateHnd()
{
  qDebug()<<"enter applyReportState state";
  try
  {
    Task task;
    task.synhronizeWithDb(_mainWindow->getCurrentTaskId());
    // Если выбрана галочка "пометить поручение как просроченное",
    // устанавливаем флаг в поручение
    if(_reportTaskWindow->ui->setIsOverdueChBx->isChecked())
    {
      task.setIsOverdue(true);
    }
    _reportTaskWindow->updateReportedTask(Task::complete, task);
    emit(jumpToMainWindowInfoRefreshState());
  }
  catch(SqlQueryException& ex)
  {
    qCritical()<<ex.getInfo();
    QMessageBox::critical(_mainWindow, tr("ОШБИКА"),tr("Ошибка запроса к БД"), QMessageBox::Ok);
    emit(jumpToApplyDeclineReportState());
  }
  catch(SmtpException& ex)
  {
    qCritical()<<ex.getInfo();
    QMessageBox::critical(0, tr("ОШБИКА"),tr("Не удалось отправить уведомление по email"), QMessageBox::Ok);
    emit(jumpToMainWindowInfoRefreshState());
  }
  catch(Exception& ex)
  {
    qCritical()<<"catch unwaited exception";
    qCritical()<<ex.getInfo();
    QMessageBox::critical(_mainWindow, tr("ОШБИКА"),tr("Критическая ошибка"), QMessageBox::Ok);
    emit(jumpToExitState());
  }
}

void GuiStateMachine::declineReportStateHnd()
{
  qDebug()<<"enter declineReportState state";
  try
  {
    Task task;
    task.synhronizeWithDb(_mainWindow->getCurrentTaskId());
    _reportTaskWindow->updateReportedTask(Task::updated, task);
    emit(jumpToMainWindowInfoRefreshState());
  }
  catch(ExecutionAborted)
  {emit(jumpToApplyDeclineReportState());}
  catch(SqlQueryException& ex)
  {
    qCritical()<<ex.getInfo();
    QMessageBox::critical(_mainWindow, tr("ОШБИКА"),tr("Ошибка запроса к БД"), QMessageBox::Ok);
    emit(jumpToReportTaskState());
  }
  catch(Exception& ex)
  {
    qCritical()<<"catch unwaited exception";
    qCritical()<<ex.getInfo();
    QMessageBox::critical(_mainWindow, tr("ОШБИКА"),tr("Критическая ошибка"), QMessageBox::Ok);
    emit(jumpToExitState());
  }
}

void GuiStateMachine::selectFilesReportStateHnd()
{
  qDebug()<<"enter selectFilesReportState";
  // Получаем полный путь к добавленным файлам
  QStringList lst = QFileDialog::getOpenFileNames(_reportTaskWindow, tr("Добавить файлы"));

  // Добавляем только те файлы, которых еще нет в списке
  foreach (QString name, lst)
  {
    if(!(_reportTaskWindow->getReportFiles().contains(name)))
    {
      _reportTaskWindow->addReportFile(name);
    }
  }
  try
  {
    // Преобразуем в короткие имена файлов и отображаем список
    _reportTaskWindow->ui->selectedFilesLst->clear();
    _reportTaskWindow->ui->selectedFilesLst->addItems(FtpHardWorker::getShortFileNames(_reportTaskWindow->getReportFiles()));
    emit(jumpToReportTaskState());
  }
  catch(Exception& ex)
  {
    qCritical()<<"catch unwaited exception";
    qCritical()<<ex.getInfo();
    QMessageBox::critical(0, tr("ОШБИКА"),tr("Критическая ошибка"), QMessageBox::Ok);
    emit(jumpToExitState());
  }
}

void GuiStateMachine::exitStateHnd()
{
  qDebug()<<"enter exitState";
  // Закрываем соединение с SMTP сервером
  SmtpHardWorker* smtp = SmtpHardWorker::getInstance();
  smtp->disconnect();

  // Закрываем соединение с FTP сервером
  FtpHardWorker* ftp = General::getFtpHardWorker();
  ftp->disconnect();
  ftp->removeTmpFolder();

  // Закрываем соединение с БД
  _sqlHardWorker->disconnect();
}
