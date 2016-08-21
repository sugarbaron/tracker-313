#-------------------------------------------------
#
# Project created by QtCreator 2015-12-02T15:58:00
#
#-------------------------------------------------

QT       += core gui
QT       += sql
QT       += network
QT       += ftp

QMAKE_CXXFLAGS_DEBUG += -g3 -pg
QMAKE_LFLAGS_DEBUG += -pg -lgmon

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Tracker313
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    general.cpp \
    msgloghandler.cpp \
    loginwindow.cpp \
    guistatemachine.cpp \
    worker.cpp \
    createtaskwindow.cpp \
    exception.cpp \
    selectaccomplicewindow.cpp \
    task.cpp \
    project.cpp \
    order.cpp \
    redirecttaskwindow.cpp \
    qstringex.cpp \
    confirmnewtaskwindow.cpp \
    reporttaskwindow.cpp \
    report.cpp \
    ftphardworker.cpp \
    clockunit.cpp \
    email.cpp \
    smtphardworker.cpp \
    sqlhardworkerunit.cpp \
    sqlhardworker_tester.cpp \
    sqlexchangeunit.cpp \
    sqlqueryparser.cpp \
    settingsunit.cpp \
    sqlcorpuscle.cpp

HEADERS  += mainwindow.h \
    sqlhardworker.h \
    general.h \
    msgloghandler.h \
    loginwindow.h \
    guistatemachine.h \
    worker.h \
    createtaskwindow.h \
    exception.h \
    selectaccomplicewindow.h \
    task.h \
    project.h \
    order.h \
    redirecttaskwindow.h \
    alghoritm.h \
    qstringex.h \
    confirmnewtaskwindow.h \
    reporttaskwindow.h \
    report.h \
    ftphardworker.h \
    clockunit.h \
    email.h \
    smtphardworker.h \
    sqlhardworkerunit.h \
    sqlhardworker_tester.h \
    sqlexchange.h \
    sqlexchangeunit.h \
    clock.h \
    settings.h \
    settingsunit.h \
    sqlqueryparser.h \
    sqldatabasecreator.h \
    sqlcorpuscle.h

FORMS    += mainwindow.ui \
    loginwindow.ui \
    createtaskwindow.ui \
    selectaccomplicewindow.ui \
    redirecttaskwindow.ui \
    confirmnewtaskwindow.ui \
    reporttaskwindow.ui

SMTP_LIBRARY_LOCATION = $$PWD/smtp
win32:CONFIG(release, debug|release): LIBS += -L$$SMTP_LIBRARY_LOCATION -lSMTPEmail
else:win32:CONFIG(debug, debug|release): LIBS += -L$$SMTP_LIBRARY_LOCATION -lSMTPEmail
else:unix: LIBS += -L$$SMTP_LIBRARY_LOCATION -lSMTPEmail

INCLUDEPATH += $$SMTP_LIBRARY_LOCATION
DEPENDPATH += $$SMTP_LIBRARY_LOCATION
