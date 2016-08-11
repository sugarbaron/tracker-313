/*! \file report.h
 *  \author Бубенщиков А.И.
 *  \date 16.02.16
 */

#ifndef REPORT_H
#define REPORT_H
#include <QtCore>

/*!
 * \brief Класс описывающий отчет по поручению
 */
class Report
{
  private:

    /// Id отчёта
    quint32 _id;

    /// Текст отчета
    QString _text;

    /// Список файлов вложения отчета
    QStringList _files;

    /// Дата создания отчета по поручению
    QDate _creationDate;

  public:
    Report();

    bool operator ==(const Report& r) const;
    bool operator ==(const quint32 id) const;

    //========== Интерфейсные методы доступа для записи к закрытым полям класса=================
    void setId(quint32 id);
    void setText(const QString& text);
    void setFiles(const QStringList& files);
    void setCreationDate(const QDate& date);

    //========== Интерфейсные методы доступа для чтения к закрытым полям класса=================
    quint32 getId() const;
    QString getText() const;
    QStringList getFiles() const;
    QDate getCreationDate() const;

    //===========================================================================================

    /// Метод проверяющий что все обязательные поля объекта заполнены и их содержимое корректно
    bool isValid() const;

    /*!
     * \brief synhronizeWithDb   Метод синхронизирует отчет с БД
     * \param id                 Id отчета в БД с которым необходимо
     *                           выполнить синхронизацию объекта
     * \throws NeedFixCode       Если id, передаваемый аргументом
     *                           равен 0
     * \throws SqlQueryExcepiton Если произошла ошибка запроса к базе
     *                           данных
     */
    void synhronizeWithDb(quint32 id);
    
    bool isEqual(const Report& other) const;
};

#endif // REPORT_H
