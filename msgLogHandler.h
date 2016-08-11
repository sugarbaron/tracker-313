/*! \file msgloghandler.h
 *  \brief Файл с описанием функции-обработчика, вызываемой при записи сообщения в лог
 *  \author Бубенщиков А.И.
 *  \date 10.12.2015
 */
#ifndef MSGLOGHANDLER_H
#define MSGLOGHANDLER_H

#include <QDebug>

void logMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg);

#endif // MSGLOGHANDLER_H
