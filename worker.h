/*! \file Worker.h
 *  \brief Описание интерфейса класса Worker
 *  \author Бубенщиков А.И.
 *  \date 16.12.2015
 */
#ifndef Worker_H
#define Worker_H

#include <QObject>


/*!
 * \brief Класс, описывающий сушьность работника
 */
class Worker
{
  private:
    /// ID работника
    quint32 _id;

    /// Фамилия работника
    QString _surname;

    /// Имя работника
    QString _name;

    /// Отчество работника
    QString _patronymic;

    /// Должность работника
    QString _position;

    /// Электронная почта работника
    QString _email;

    /// Логин работника в системе
    QString _login;

    /// Пароль работника в системе
    QString _pass;

    /// ID заместителя
    quint32 _idVice;

    /// Флаг показывающий, что у работника есть права бога
    bool _isGod;

  public:
    Worker();
    bool operator ==(const Worker& w) const;
    bool operator ==(const quint32 id) const;

    //========== Интерфейсные методы доступа для чтения к закрытым полям класса=================
    quint32 getId() const;
    QString getPosition() const;
    QString getEmail() const;
    QString getLogin() const;
    QString getPass() const;
    QString getName() const;
    QString getSurname() const;
    QString getPatronymic() const;

    /*!
     * \brief Интерфейсный метод, возвращающий Id заместителя
     * \return Id заместителя или -1, если заместитель отсутствует
     */
    quint32 getVice() const;
    /// Интерфейсный метод, проверяющий есть ли у работника права бога
    bool isGod() const;

    //========== Интерфейсные методы доступа для записи к закрытым полям класса=================
    void setId(quint32 id);
    void setName(QString name);
    void setSurname(QString surname);
    void setPatronymic(QString patronymic);
    void setPosition(QString position);
    void setEmail(QString email);
    void setLogin(QString login);
    void setPass(QString pass);
    void setVice(quint32 idVice);
    void setIsGod(bool isGod);

    //==========================================================================================
    /// Метод проверяющий что все обязательные поля объекта заполнены и их содержимое корректно
    bool isValid() const;

    /*!
     * \brief Вычисляет и возвращает полные ФИО работника
     * \throws NeedFixCodeException в случае если обязательные поля объекта пусты
     * \return Полные ФИО работника
     */
    QString getFullName() const;

    /*!
     * \brief Dозвращает фамилию и инициалы работника в формате:
     * [Фамилия]<пробел>[1-ая буква имени]<точка>[1-ая буква отчетства]<точка>
     * \throws NeedFixCodeException в случае если обязательные поля объекта пусты
     * \return ФИО работника в формате: [Фамилия]<пробел>[1-ая буква имени]<точка>[1-ая буква отчетства]<точка>
     */
    QString getShortName() const;

    /*!
     * \brief Метод синхронизирует объект БД
     * \param id работника с которым будет выполнена синхронизация
     * \throws NeedFixCode если воркер, передаваемый аргументом не валидный
     * \throws SqlQueryExcepiton если произошла ошибка запроса к базе данных
     */
    void synhronizeWithDb(quint32 id);
    
    bool isEqual(const Worker& other) const;

  signals:

  public slots:

};

Q_DECLARE_METATYPE(Worker)

#endif // Worker_H
