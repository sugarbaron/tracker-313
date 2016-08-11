#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QWidget>
#include "ui_loginwindow.h"

namespace Ui {
  class LoginWindow;
}

class LoginWindow : public QWidget
{
  Q_OBJECT

  public:
    explicit LoginWindow(QWidget *parent = 0);
    Ui::LoginWindow* ui;
    ~LoginWindow();
  private:
    void closeEvent(QCloseEvent* event);

  signals:
    void closeSignal();
};

#endif // LOGINWINDOW_H
