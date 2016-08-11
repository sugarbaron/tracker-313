#include "loginwindow.h"
#include "general.h"

LoginWindow::LoginWindow(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::LoginWindow)
{
  ui->setupUi(this);
}

LoginWindow::~LoginWindow()
{
  delete ui;
}

void LoginWindow::closeEvent(QCloseEvent* event)
{
  emit(closeSignal());
  QWidget::closeEvent(event);
}
