#ifndef TREE_STRUCTURE_H
#define TREE_STRUCTURE_H

#include <map>
#include <string>

#include "common/debug_utils.h"
#include "common/memory_deallocation.h"

template< typename T>
class tree_structure
{
  T *m_parent;
  T *m_next_sibling;
  T *m_prev_sibling;

  T *m_first_child;
  T *m_last_child;
public:
  tree_structure ()
  {
    m_parent = m_next_sibling = m_prev_sibling = m_first_child = m_last_child = nullptr;
  }

  ~tree_structure ()
  {
    T *cur = m_first_child, *next = nullptr;
    while (cur)
      {
        next = cur->next_sibling ();
        FREE (cur);
        cur = next;
      }
  }

  T *parent () const { return m_parent; }
  T *next_sibling () const { return m_next_sibling; }
  T *prev_sibling () const { return m_prev_sibling; }

  T *first_child () const { return m_first_child; }
  T *last_child () const { return m_last_child; }

  void insert_child (T *position, T *new_child)
  {
    DEBUG_ASSERT (new_child && new_child->parent () == nullptr);
    T *prev = position ? position->prev_sibling () : m_last_child;
    T *next = position;

    new_child->set_next_sibling (next);
    new_child->set_prev_sibling (prev);
    new_child->set_parent (static_cast<T *> (this));

    if (prev)
      prev->set_next_sibling (new_child);

    if (next)
      next->set_prev_sibling (new_child);

    if (next == m_first_child)
      m_first_child = new_child;

    if (prev == m_last_child)
      m_last_child = new_child;
  }

  void remove_child (T *child)
  {
    DEBUG_ASSERT (child && child->parent () == this);
    T *prev = child->prev_sibling ();
    T *next = child->next_sibling ();

    if (prev)
      prev->set_next_sibling (next);

    if (next)
      next->set_prev_sibling (prev);

    if (child == m_first_child)
      m_first_child = next;

    if (child == m_last_child)
      m_last_child = prev;

    child->set_parent (nullptr);
    child->set_next_sibling (nullptr);
    child->set_prev_sibling (nullptr);
  }

private:
  void set_parent (T *parent) { m_parent = parent; }
  void set_next_sibling (T *next) { m_next_sibling = next; }
  void set_prev_sibling (T *prev) { m_prev_sibling = prev; }
};

#endif // TREE_STRUCTURE_H
