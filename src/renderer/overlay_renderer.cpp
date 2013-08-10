#include "renderer/overlay_renderer.h"

#include "common/memory_deallocation.h"

#include "editor/items_selection.h"

#include "renderer/renderer_items_container.h"
#include "renderer/svg_renderer.h"
#include "renderer/renderer_state.h"
#include "renderer/renderer_config.h"
#include "renderer/qt2skia.h"
#include "renderer/renderer_overlay_root.h"
#include "renderer/renderer_page.h"
#include "renderer/overlay_item_type.h"
#include "renderer/rendered_items_cache.h"

#include "svg/svg_document.h"
#include "svg/items/svg_items_container.h"
#include "svg/items/abstract_svg_item.h"
#include "svg/items/svg_graphics_item.h"

#pragma warning(push, 0)
#include <SkCanvas.h>
#include <SkSurface.h>
#include <SkDevice.h>
#pragma warning(pop)

#include <QRect>
#include <QTransform>
#include <QImage>
#include <QPainter>
#include <memory>



overlay_renderer::overlay_renderer (rendered_items_cache *cache)
{
  m_document = nullptr;
  m_container = nullptr;
  m_renderer = new svg_renderer (nullptr, nullptr);
  memset (m_root_items, 0, sizeof (m_root_items));
  m_cache = cache;
}

overlay_renderer::~overlay_renderer ()
{
  FREE (m_container);
  FREE (m_renderer);
}

void overlay_renderer::set_document (svg_document *document)
{
  FREE (m_container);
  m_container = new renderer_items_container;
  char buf[16];
  for (int i = 0; i < (int)overlay_layer_type::COUNT; i++)
    {
      m_root_items[i] = new renderer_overlay_root (std::string ("#overlay_root") + itoa (i, buf, 10));
      m_container->add_item (m_root_items[i]);
    }

  m_document = document;
}

void overlay_renderer::draw (QPainter &painter, const QRect &rect_to_draw, const QTransform &transform)
{
  if (!m_container)
    return;

  SkBitmap bitmap;
  bitmap.setConfig (SkBitmap::kARGB_8888_Config, rect_to_draw.width (), rect_to_draw.height ());
  bitmap.allocPixels ();
  bitmap.eraseColor (SK_ColorWHITE);
  SkDevice device (bitmap);
  SkCanvas canvas (&device);

  m_cache->lock ();
  canvas.drawBitmap (m_cache->get_current_screen (), 0, 0);
  m_cache->unlock ();

  m_renderer->draw_to_bitmap (rect_to_draw, transform, root (overlay_layer_type::BASE), &bitmap);
  QImage img = qt2skia::qimage (bitmap);
  painter.drawImage (rect_to_draw, img);
}

void overlay_renderer::draw_page (QPainter &painter, const QRect &rect_to_draw, const QTransform &transform)
{
  std::unique_ptr<SkBitmap> bitmap (m_renderer->draw_to_bitmap (rect_to_draw, transform, root (overlay_layer_type::PAGE)));
  QImage img = qt2skia::qimage (*bitmap);
  painter.drawImage (rect_to_draw, img);
}

void overlay_renderer::add_overlay_item (overlay_layer_type type, abstract_renderer_item *item)
{
  m_container->add_item (item);
  m_container->add_child (root (type)->name (), item->name ());
}

void overlay_renderer::remove_overlay_item (overlay_layer_type type, const std::string &item)
{
  root (type)->erase_child (item);
  m_container->remove_item (item);
}

abstract_renderer_item *overlay_renderer::root (overlay_layer_type type) const
{
  return m_root_items[(int)type];
}
