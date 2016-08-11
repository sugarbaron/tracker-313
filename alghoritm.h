#ifndef ALGHORITM_H
#define ALGHORITM_H
#include <QtCore>
#include "exception.h"

/*! \brief qFindEx          Обвязка на кутешный алгоритм поиска qFind
 *  \param container        Контейнер, в котором будет производиться поиск
 *  \param val              Значение, поиск которого будет осуществляться
 *  \throw ExecutionAborted Если переданное значение не найдено
 *  \return                 Константный итератор, указывающий на найденый
 *                          объект
 */
template <typename Container, typename T>
inline typename Container::const_iterator qFindEx(const Container &container, const T &val)
{
  typename Container::const_iterator ptr = qFind(container,val);
  if(container.end() == ptr)
  {
    throw ExecutionAborted("value not found in list");
  }
  return ptr;
}

/*! \brief qRemoveEquals Функция, удаляющая повторяющиеся элементы
 *                       в контейнере
 *  \param container     Контейнер, в котором будет произведено
 *                       удаление
 */
template <typename Container>
inline void qRemoveEquals(Container &container)
{
  Container tmp;
  typename Container::iterator it = container.begin();
  while(it != container.end())
  {
    if(!tmp.contains(*it))
    {
      tmp.push_back(*it);
    }
    ++it;
  }
  container = tmp;
}

#endif // ALGHORITM_H
