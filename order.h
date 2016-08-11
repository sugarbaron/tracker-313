/*! \file order.h
 *  \brief Описание интерфейса класса, описывающего заказ
 *  \date 20.01.16
 *  \author Бубенщиков А.И.
 */
#ifndef ORDER_H
#define ORDER_H
#include <QtCore>

/*!
 * \brief Класс, описывающий заказ
 */
class Order
{
  private:
    /// Id заказа
    quint32 _id;
    /// Название заказа
    QString _name;
  public:
    Order();
    Order(quint32 id, QString name);
    bool operator ==(quint32 id) const;

    //========== Интерфейсные методы доступа для чтения к закрытым полям класса=================
    quint32 getId() const;
    QString getName() const;

    //========== Интерфейсные методы доступа для записи к закрытым полям класса=================
    void setId(quint32 id);
    void setName(QString name);

    //==========================================================================================
    ///Метод, проверяющий объект на валидность
    bool isValid() const;

    /*!
     * \brief Метод синхронизирует объект БД
     * \param id заказа с которым будет выполнена синхронизация
     * \throws NeedFixCode если воркер, передаваемый аргументом не валидный
     * \throws SqlQueryExcepiton если произошла ошибка запроса к базе данных
     */
    void synhronizeWithDb(quint32 id);
    
    bool isEqual(const Order& other) const;
};

#endif // ORDER_H
