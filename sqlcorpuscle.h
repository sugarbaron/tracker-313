/*! \file  sqlcorpuscle.h
 *  \brief класс объединяет плэйсхолдер и значение для привязки к нему.
 *  \author sugarbaron ([bravery_and_stupidity] e-mail: sugarbaron1@mail.ru)
 *  \date 12.08.2016
 */
#ifndef SQLCORPUSCLE
#define SQLCORPUSCLE

#include <QString>
#include <QVariant>

/*! \brief класс объединяет плэйсхолдер и значение для привязки к нему. */
class SqlCorpuscle
{
  //секция_данных______________________________________________________________
  /////////////////////////////////////////////////////////////////////////////
  private: QString  _placeholder;
  private: QVariant _valueToBind;
  
  //секция_конструкторов_______________________________________________________
  /////////////////////////////////////////////////////////////////////////////
  public:  SqlCorpuscle(const QString& placeholder, const QVariant& valueToBind);
  public:  ~SqlCorpuscle();
  
  //секция_методов_____________________________________________________________
  /////////////////////////////////////////////////////////////////////////////
  public:  QString  getPlaceholder() const;
  public:  QVariant getValueToBind() const;
};
#endif // SQLCORPUSCLE

