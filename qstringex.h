/*! \file qstringex.h
 *  \author Бубенщиков А.И.
 *  \date 31.01.16
 */

#ifndef QSTRINGEX_H
#define QSTRINGEX_H

#include <QString>


/*!
 * \brief Класс, основанный на QString
 *
 */
class QStringEx : public QString
{
  public:
    // Используем using-объявления для наследования
    // операторов из класса QString, а также чтобы не скрывать
    // имена методов toUint и toInt из класса QString
    using QString::operator !=;
    using QString::operator +=;
    using QString::operator <;
    using QString::operator <=;
    using QString::operator =;
    using QString::operator ==;
    using QString::operator >;
    using QString::operator >=;
    using QString::operator [];

    QStringEx();
    QStringEx(const QString & other);

    /*!
     * \brief Метод преобразующий в строку в quint32
     * \param base Необязательный параметр. Основание системы счисления.
     * \throw NeedFixCode если строка не может быть преобразована в quint32
     * \return Число, содержащееся в строке
     */
    quint32 toUIntEx(quint32 base = 10);

    /*!
     * \brief Метод преобразующий в строку в qint32
     * \param base Необязательный параметр. Основание системы счисления.
     * \throw NeedFixCode если строка не может быть преобразована в qint32
     * \return Число, содержащееся в строке
     */
    qint32 toIntEx(qint32 base = 10);
};

#endif // QSTRINGEX_H
