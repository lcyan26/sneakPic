#ifndef SINGLE_SUBPATH_H
#define SINGLE_SUBPATH_H

#include "path/single_path_point.h"

#include <vector>

class QTransform;
class subpath_iterator;
enum class cp_type : char;


class single_subpath
{
  bool m_is_closed;
  vector<single_path_point> m_elements;

public:
  single_subpath ();
  ~single_subpath ();

  size_t total_points () const;
  size_t total_segments () const;

  /// i'm too lazy to write const iterator(
  /// TODO: write const iterator or something
  subpath_iterator begin () const;
  subpath_iterator end () const;
  subpath_iterator last_point () const;

  bool is_closed () const { return m_is_closed; }
  void set_closed (bool val) { m_is_closed = val; }

  void apply_transform (const QTransform &transform);

  const vector<single_path_point> &elements () const { return m_elements; }
  vector<single_path_point> &elements () { return m_elements; }

  void reverse ();
  void erase( size_t point_num );
  friend class subpath_iterator;
  friend class path_builder;
};

class subpath_iterator
{
  single_subpath *m_subpath;
  size_t m_point_num;
public:
  subpath_iterator ();
  subpath_iterator (single_subpath &subpath, size_t point_num);
  ~subpath_iterator () {}

  subpath_iterator &operator ++ ();
  bool operator == (const subpath_iterator &rhs) const;
  bool operator != (const subpath_iterator &rhs) const;

  bool is_valid () const { return m_subpath != 0;}
  size_t point_num () const { return m_point_num; }

  subpath_iterator neighbour (cp_type type) const;
  subpath_iterator left () const;
  subpath_iterator right () const;

  const QPointF &anchor_point () const;
  QPointF &anchor_point ();

  const QPointF &control_point (cp_type type) const;
  QPointF &control_point (cp_type type);
  bool has_control_point (cp_type type) const;
};


#endif // SINGLE_SUBPATH_H
