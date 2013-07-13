#ifndef RENDERED_ITEMS_CACHE_H
#define RENDERED_ITEMS_CACHE_H

#include <map>
#include <utility>
#include <string>

class SkBitmap;
class render_cache_id;
class QMutex;
class QTransform;
class QColor;

class rendered_items_cache
{
  static const int m_block_pixel_size;

  std::map<render_cache_id, SkBitmap> *m_cache;
  std::map<render_cache_id, SkBitmap> *m_next_zoom_cache;
  std::map<int, std::string> m_selection_map;

  double m_zoom_x, m_zoom_y;

  QMutex *m_mutex;
  bool m_pending_changes;

public:
  rendered_items_cache ();
  ~rendered_items_cache ();

  static int block_pixel_size () { return m_block_pixel_size; }

  void add_bitmap (const render_cache_id &id, const SkBitmap &pixmap, bool add_to_new_cache);
  void remove_from_cache (const render_cache_id &id);
  bool is_cached (const render_cache_id &id, bool next_cache) const;

  void zoom_level_changed (double zoom_x, double zoom_y);
  void clear_next_zoom_cache ();
  void clear ();
  void clear_selection_mapping ();

  void lock ();
  void unlock ();

  bool has_pending_changes ();
  bool same_zoom (const QTransform &transform);

  void add_selection_mapping (int id, const std::string &name);
  void remove_selection_mapping (int id);

  std::string get_selection_name (int id) const;

  /// call this functions only after calling lock
  SkBitmap bitmap (const render_cache_id &id) const;
  double zoom_x () const { return m_zoom_x; }
  double zoom_y () const { return m_zoom_y; }
  //************************************ 

  static QColor get_selection_color (int id);
  static int get_id_by_color (const QColor &color);
};

#endif // RENDERED_ITEMS_CACHE_H