#ifndef SVG_PAINTER_H
#define SVG_PAINTER_H

#include "renderer/abstract_painter.h"

#include <QTransform>

class svg_document;
class abstract_svg_item;
class abstract_renderer_item;
class rendered_items_cache;
class render_cache_id;
class svg_renderer;
class events_queue;
class overlay_renderer;
class items_selection;
class mouse_shortcuts_handler;
class settings_t;


class svg_painter : public abstract_painter
{
  QTransform m_cur_transform;
  QTransform m_last_transform;
  QPoint m_drag_start_pos;
  svg_document *m_document;
  rendered_items_cache *m_cache;
  events_queue *m_queue;
  overlay_renderer *m_overlay;
  items_selection *m_selection;
  mouse_shortcuts_handler *m_mouse_handler;
  settings_t *m_settings;

public:
  svg_painter (gl_widget *glwidget, const mouse_filter *mouse_filter_object, rendered_items_cache *cache, events_queue *queue, settings_t *settings);
  ~svg_painter ();

  void set_document (svg_document *document);

  virtual unsigned int mouse_event (const mouse_event_t &m_event) override;
  virtual void draw () override;
  virtual void configure () override;

  virtual void wheelEvent (QWheelEvent *qevent) override;
  virtual void leaveEvent (QEvent *qevent) override;
  virtual void resizeGL (int width, int height) override;
  virtual bool event (QEvent * /*qevent*/) override { return false; }
  virtual bool keyReleaseEvent (QKeyEvent *qevent) override;

  settings_t *settings () const { return m_settings; }

private:
  void reset_transform ();
  void send_changes (bool interrrupt_rendering);
  void get_cache_id (const QTransform &transform, render_cache_id &first, render_cache_id &last, const QRectF &rect) const;
  abstract_svg_item *get_current_item (const QPoint &pos);
  void update_drawing (QTransform transform);
  void draw_base (QPainter &painter);
  void draw_overlay (QPainter &painter);
  void draw_page (QPainter &painter);
  void select_item (const QPoint &pos);

  mouse_shortcuts_handler *create_mouse_shortcuts ();
  void start_pan (const QPoint &pos);
  void pan_picture (const QPoint &pos);
  void find_current_object (const QPoint &pos);
};

#endif // SVG_PAINTER_H
