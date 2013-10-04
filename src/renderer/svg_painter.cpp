#include "renderer/svg_painter.h"

#pragma warning(push, 0)
#include <SkCanvas.h>
#include <SkSurface.h>
#include <SkDevice.h>
#pragma warning(pop)

#include <memory>

#include <QEvent>
#include <QLocale>
#include <QMessageBox>
#include <QStatusBar>
#include <QWheelEvent>

#include "common/common_utils.h"
#include "common/math_defs.h"
#include "common/memory_deallocation.h"
#include "common/debug_utils.h"

#include "editor/items_selection.h"
#include "editor/tools/abstract_tool.h"

#include "gui/gl_widget.h"
#include "gui/mouse_shortcuts_handler.h"
#include "gui/shortcuts_config.h"
#include "gui/settings.h"

#include "renderer/abstract_renderer_item.h"
#include "renderer/renderer_state.h"
#include "renderer/render_cache_id.h"
#include "renderer/rendered_items_cache.h"
#include "renderer/svg_renderer.h"
#include "renderer/qt2skia.h"
#include "renderer/events_queue.h"
#include "renderer/event_transform_changed.h"
#include "renderer/overlay_renderer.h"
#include "renderer/current_item_outline_renderer.h"
#include "renderer/svg_page_renderer.h"
#include "renderer/rubberband_selection.h"

#include "svg/svg_document.h"
#include "svg/items/abstract_svg_item.h"
#include "svg/items/svg_items_container.h"
#include "svg/svg_utils.h"





svg_painter::svg_painter (gl_widget *glwidget, rendered_items_cache *cache, events_queue *queue, svg_document *document, settings_t *settings)
  : abstract_painter (glwidget)
{
  m_current_tool = nullptr;
  m_document = nullptr;
  m_page_renderer = nullptr;
  m_cache = cache;
  m_queue = queue;
  m_overlay = new overlay_renderer (m_cache);
  m_settings = settings;
  m_mouse_handler = create_mouse_shortcuts ();
  update_status_bar_widgets ();
  set_document (document);
}

svg_painter::~svg_painter ()
{
  FREE (m_mouse_handler);
  FREE (m_overlay);
}

void svg_painter::update_status_bar_widgets ()
{
  emit zoom_description_changed (QString ("%1 %2 ").arg ( QLocale ().toString (m_cur_transform.m11 () * 100, 'f', 2)).arg (QLocale ().percent ()));
}

void svg_painter::reset_transform ()
{
  double doc_width, doc_height;
  svg_utils::get_doc_dimensions (m_document, doc_width, doc_height);
  double scale = qMin (glwidget ()->width () / doc_width, glwidget ()->height () / doc_height);
  m_cur_transform = QTransform::fromScale (scale, scale);
  send_changes (true);
  update_status_bar_widgets ();
}

void svg_painter::set_document (svg_document *document)
{
  m_document = document;
  m_overlay->set_svg_container (document->item_container ());
  double width, height;
  svg_utils::get_doc_dimensions (document, width, height);
  create_overlay_containers ();
  m_page_renderer->set_dimensions (width, height);

  set_configure_needed (CONFIGURE_TYPE__ITEMS_CHANGED, 1);
  reset_transform ();
  connect (m_document, &svg_document::items_changed, this, &svg_painter::items_changed);
}

unsigned int svg_painter::mouse_event (const mouse_event_t &m_event)
{
  if (m_mouse_handler->process_mouse_event (m_event))
    return 0;

  if (m_current_tool)
    if (m_current_tool->mouse_event (m_event))
      return 0;

  return 0;
}

void svg_painter::draw ()
{
  QPainter painter;
  painter.begin (glwidget ());
  painter.fillRect (glwidget ()->rect(), Qt::white);

  draw_page (painter);
  //draw_base (painter);
  draw_overlay (painter);

  painter.end ();
}

void svg_painter::configure ()
{
  if (get_configure_needed (CONFIGURE_TYPE__ITEMS_CHANGED))
    {
      set_configure_needed (CONFIGURE_TYPE__ITEMS_CHANGED, 0);
      set_configure_needed (CONFIGURE_TYPE__REDRAW, 1);
      send_changes (true);
    }

  if (get_configure_needed (CONFIGURE_TYPE__REDRAW))
    {
      update_drawing (m_cur_transform);
    }


  return;
}

void svg_painter::wheelEvent (QWheelEvent *qevent)
{
  if (qevent->orientation () == Qt::Vertical)
    {
      double scale = pow (1.2, qevent->delta() / 240.0);
      FIX_UNUSED (scale);
      QPointF event_pos_local_before = m_cur_transform.inverted ().map (QPointF (qevent->pos ()));
      m_cur_transform.scale (scale, scale);
      QPointF event_pos_local_after = m_cur_transform.inverted ().map (QPointF (qevent->pos ()));
      QPointF vector = event_pos_local_after - event_pos_local_before;
      m_cur_transform.translate (vector.x (), vector.y ());
      send_changes (true);
      update_status_bar_widgets ();

      glwidget ()->update ();
      qevent->accept ();
    }
}

void svg_painter::leaveEvent (QEvent *qevent)
{
  FIX_UNUSED (qevent);
}

void svg_painter::keyPressEvent (QKeyEvent * qevent)
{
  if (qevent->key () == Qt::Key_1 && qevent->modifiers () == Qt::NoModifier)
    {
      reset_transform ();
      set_configure_needed (CONFIGURE_TYPE__REDRAW, 1);
      glwidget ()->update ();
      qevent->accept ();
    }
}

void svg_painter::resizeEvent (QResizeEvent * /*qevent*/)
{
  send_changes (false);
  glwidget ()->update ();
}

void svg_painter::update_drawing (QTransform transform)
{
  svg_renderer renderer (m_cache, nullptr);
  renderer.update_drawing (transform, QRectF (glwidget ()->rect ()), (int)render_cache_type::ROOT_ITEM);
}

void svg_painter::send_changes (bool interrrupt_rendering)
{
  
  auto object_pair = render_cache_id::get_id_for_pixel_rect (m_cur_transform, glwidget ()->rect (), (int)render_cache_type::ROOT_ITEM);
  m_queue->add_event_and_wait (new event_transform_changed (object_pair.first, object_pair.second, m_cur_transform, interrrupt_rendering));
  set_configure_needed (CONFIGURE_TYPE__REDRAW, 1);
}

abstract_svg_item *svg_painter::get_current_item (const QPoint &pos)
{
  //// first, change current position and zoom to previous zoom level
  double cache_zoom_x = m_cache->zoom_x ();
  double cache_zoom_y = m_cache->zoom_y ();
  double cur_zoom_x = m_cur_transform.m11 ();
  double cur_zoom_y = m_cur_transform.m22 ();
  QTransform scale_transform = QTransform::fromScale (cache_zoom_x / cur_zoom_x, cache_zoom_y / cur_zoom_y);
  QTransform transform = m_cur_transform * scale_transform;
  QPointF scaled_pos = scale_transform.map (QPointF (pos));
  m_cache->lock ();
  DO_ON_EXIT (m_cache->unlock ());

  //// find corresponding block in cache
  render_cache_id id = render_cache_id::get_id_by_pixel_pos (scaled_pos.x (), scaled_pos.y (), transform, (int)render_cache_type::ROOT_ITEM_SELECTION);
  SkBitmap bitmap = m_cache->bitmap (id);
  if (bitmap.empty ())
    return nullptr;

  /// pick item id, which is packed in color
  QImage img = qt2skia::qimage (bitmap);
  QRectF block_rect = id.pixel_rect (transform);
  QPointF point_to_pick = scaled_pos - block_rect.topLeft ();
  int block_size = rendered_items_cache::block_pixel_size ();
  DEBUG_ASSERT (   point_to_pick.x () >= 0 && point_to_pick.y () >= 0
                && point_to_pick.x () < block_size && point_to_pick.y () < block_size);
  QColor color (img.pixel (point_to_pick.toPoint ()));
  int item_id = rendered_items_cache::get_id_by_color (color);

  std::string selected_item_name = m_cache->get_selection_name (item_id);
  if (selected_item_name.empty ())
    return nullptr;

  return m_document->item_container ()->get_item (selected_item_name);
}

void svg_painter::draw_base (QPainter &painter)
{
  m_cache->lock ();
  QImage img = qt2skia::qimage (m_cache->get_current_screen ((int)render_cache_type::ROOT_ITEM));
  painter.drawImage (img.rect (), img);
  m_cache->unlock ();
}

void svg_painter::draw_overlay (QPainter &painter)
{
  m_overlay->draw (painter, glwidget ()->rect (), m_cur_transform);
}

void svg_painter::draw_page (QPainter &painter)
{
  m_overlay->draw_page (painter, glwidget ()->rect (), m_cur_transform);
}

#include "svg/attributes/svg_attribute_transform.h"

bool svg_painter::select_item (const QPoint &pos, bool clear_selection)
{
  if (clear_selection)
    m_selection->clear ();
  abstract_svg_item *item = get_current_item (pos);
  
  if (item)
    m_selection->add_item (item);

  glwidget ()->update ();
  return true;
}

mouse_shortcuts_handler *svg_painter::create_mouse_shortcuts ()
{
  mouse_shortcuts_handler *handler = new mouse_shortcuts_handler (m_settings->shortcuts_cfg ());
  ADD_SHORTCUT (handler, SELECT_ITEM           , return select_item (m_event.pos (), true));
  ADD_SHORTCUT (handler, ADD_ITEM_TO_SELECTION , return select_item (m_event.pos (), false));
  ADD_SHORTCUT (handler, FIND_CURRENT_OBJECT   , return find_current_object (m_event.pos ()));
  
  ADD_SHORTCUT_DRAG (handler, PAN, return start_pan (m_event.pos ()), return pan_picture (m_event.pos ()), return true);
  return handler;
}

bool svg_painter::start_pan (const QPoint &pos)
{
  m_last_transform = m_cur_transform;
  m_drag_start_pos = pos;
  return true;
}

bool svg_painter::pan_picture (const QPoint &pos)
{
  QTransform last_inverted = m_last_transform.inverted ();
  QPointF last_pos_local = last_inverted.map (QPointF (m_drag_start_pos));
  QPointF cur_pos_local = last_inverted.map (QPointF (pos));
  QPointF diff = cur_pos_local - last_pos_local;

  m_cur_transform = QTransform (m_last_transform).translate (diff.x (), diff.y ());
  send_changes (false);
  glwidget ()->update ();
  return true;
}

bool svg_painter::find_current_object (const QPoint &pos)
{
  abstract_svg_item *current_item = get_current_item (pos);
  std::string item_string = current_item ? current_item->name () : std::string ();
  if (item_string != m_item_outline->current_item ())
    {
      m_item_outline->set_current_item (item_string);
      glwidget ()->update ();
    }

  return true;
}


void svg_painter::create_overlay_containers ()
{
  m_selection = new items_selection (m_overlay);
  m_item_outline = new current_item_outline_renderer (m_overlay);
  m_page_renderer = new svg_page_renderer (m_overlay);
}

void svg_painter::items_changed ()
{
  m_current_tool->items_changed ();

  m_selection->update_items ();
  m_item_outline->update_items ();
  m_page_renderer->update_items ();
  glwidget ()->update ();
}

void svg_painter::set_current_tool (abstract_tool *tool)
{
  if (m_current_tool)
    m_current_tool->deactivate ();

  m_current_tool = tool;
  if (m_current_tool)
    m_current_tool->activate ();
}

QPointF svg_painter::get_local_pos (const QPointF &mouse_pos) const
{
  return m_cur_transform.inverted ().map (mouse_pos);
}

void svg_painter::redraw ()
{
  glwidget ()->update ();
}
