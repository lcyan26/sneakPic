#ifndef ATRRIBUTE_POINTER_H
#define ATRRIBUTE_POINTER_H

class abstract_attribute;
class abstract_svg_item;

#include <utility>

class abstract_attribute_pointer
{
protected:
  abstract_attribute *m_cur_attribute;
  abstract_svg_item *m_item;

public:
  abstract_attribute_pointer (abstract_attribute *attribute, abstract_svg_item *item);
  abstract_attribute_pointer (abstract_attribute_pointer &&rhs);

  ~abstract_attribute_pointer ();

private:
  abstract_attribute_pointer (const abstract_attribute_pointer &rhs);
  abstract_attribute_pointer &operator = (const abstract_attribute_pointer &rhs);
  abstract_attribute_pointer &operator = (abstract_attribute_pointer &&rhs);
};

template <typename T>
class attribute_pointer : public abstract_attribute_pointer
{
public:
  attribute_pointer (T *attribute, abstract_svg_item *item)
    : abstract_attribute_pointer (attribute, item)
  {
  }

  attribute_pointer (attribute_pointer<T> &&rhs)
    : abstract_attribute_pointer (std::move (rhs))
  {
  }

  ~attribute_pointer () {}

  T *operator-> () { return static_cast<T *> (m_cur_attribute); }
  T &operator * () { return *static_cast<T *> (m_cur_attribute); }


};

#endif // ATRRIBUTE_POINTER_H
