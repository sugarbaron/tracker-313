/*! \file project.h
 *  \brief Описание интерфейса класса, описывающего проект
 *  \date 20.01.16
 *  \author Бубенщиков А.И.
 */
#ifndef PROJECT_H
#define PROJECT_H
#include <QtCore>

/*!
 * \brief Класс, описывающий проект
 */
class Project
{
  private:
    /// Id проекта
    quint32 _id;
    /// Название проекта
    QString _name;
  public:
    Project();
    Project(quint32 id, QString name);
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
     * \param id проекта с которым будет выполнена синхронизация
     * \throws NeedFixCode если воркер, передаваемый аргументом не валидный
     * \throws SqlQueryExcepiton если произошла ошибка запроса к базе данных
     */
    void synhronizeWithDb(quint32 id);
    
    bool isEqual(const Project& other) const;
};
Q_DECLARE_METATYPE(Project)
#endif // PROJECT_H
