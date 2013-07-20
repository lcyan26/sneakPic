#include "renderer/renderer_base_shape_item.h"

#include "common/memory_deallocation.h"
#include "common/debug_utils.h"

#include <QPainter>

#include "renderer_paint_server.h"
#include "renderer/qt2skia.h"
#include "renderer/rendered_items_cache.h"
#include "renderer/renderer_state.h"
#include "renderer/renderer_config.h"

#pragma warning(push, 0)
#include <SkCanvas.h>
#include <SkSurface.h>
#include <SkDevice.h>
#include <SkPoint.h>
#pragma warning(pop)

renderer_base_shape_item::renderer_base_shape_item (const std::string &name)
  : abstract_renderer_item (name)
{
  m_has_clip_path = false;
  visible = true;
  m_stroke = new SkPaint;
  m_stroke->setStyle (SkPaint::kStroke_Style);
  m_fill = new SkPaint;
  m_fill->setStyle (SkPaint::kFill_Style);
  m_stroke->setAntiAlias (true);
  m_fill->setAntiAlias (true);
  m_stroke_server = nullptr;
  m_fill_server = nullptr;
}

renderer_base_shape_item::~renderer_base_shape_item ()
{
  FREE (m_stroke);
  FREE (m_fill);
  FREE (m_stroke_server);
  FREE (m_fill_server);
}

void renderer_base_shape_item::set_stroke_linecap (Qt::PenCapStyle linecap)
{
  switch  (linecap)
    {
    case Qt::FlatCap: m_stroke->setStrokeCap (SkPaint::kButt_Cap); break;
    case Qt::RoundCap: m_stroke->setStrokeCap (SkPaint::kRound_Cap); break;
    case Qt::SquareCap: m_stroke->setStrokeCap (SkPaint::kSquare_Cap); break;
    default:
      return;
    }
  
}

void renderer_base_shape_item::set_stroke_linejoin (Qt::PenJoinStyle linejoin)
{
  switch (linejoin)
    {
    case Qt::BevelJoin: m_stroke->setStrokeJoin (SkPaint::kBevel_Join); break;
    case Qt::SvgMiterJoin: m_stroke->setStrokeJoin (SkPaint::kMiter_Join); break;
    case Qt::RoundJoin: m_stroke->setStrokeJoin (SkPaint::kRound_Join); break;
    default:
      return;
    }
}

void renderer_base_shape_item::set_visibility (bool visible_arg)
{
  visible = visible_arg;
}

void renderer_base_shape_item::set_stroke_miterlimit (double miterlimit)
{
  m_stroke->setStrokeMiter (SkFloatToScalar (miterlimit));
}

void renderer_base_shape_item::set_stroke_width (double width)
{
  m_stroke->setStrokeWidth (SkFloatToScalar (width));
}

bool renderer_base_shape_item::configure_painter (SkPaint &paint, bool stroke, bool config_for_selection) const
{
  if (!visible)
    return false;

  if (stroke)
    {
      paint = *m_stroke;
      if (m_stroke_server)
        m_stroke_server->fill_paint (paint);
    }
  else
    {
      paint = *m_fill;
      if (m_fill_server)
        m_fill_server->fill_paint (paint);
    }

  if (SkColorGetA (paint.getColor ()) == 0)
    return false;

  if (config_for_selection)
    return configure_painter_for_selection (paint);
  else  
    return true;
}

void renderer_base_shape_item::set_stroke_server (const renderer_paint_server *server)
{
  m_stroke_server = server->clone ();
}

void renderer_base_shape_item::set_fill_server (const renderer_paint_server *server)
{
  m_fill_server = server->clone ();
}

void renderer_base_shape_item::draw (SkCanvas &canvas, const renderer_state &state, const renderer_config *config) const
{
  QTransform item_transform = transform () * state.transform ();
  QRectF transformed_rect = state.transform ().mapRect (bounding_box ());
  if (!state.rect ().intersects (transformed_rect.toRect ()))
    return;

  SkPath path = qt2skia::path (m_path);
  canvas.save ();
  canvas.setMatrix (qt2skia::matrix (item_transform));
  if (m_has_clip_path)
    canvas.clipPath (qt2skia::path (m_clip_path), SkRegion::kReplace_Op, !config->render_for_selection ());

  for (int i = 0; i < 2; i++)
    {
      SkPaint paint;
      if (!configure_painter (paint, i != 0, config->render_for_selection ()))
        continue;

      canvas.drawPath (path, paint);
    }

  canvas.restore ();
}
