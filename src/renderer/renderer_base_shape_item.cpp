#include "renderer/renderer_base_shape_item.h"

#include "common/memory_deallocation.h"

#include <QPainter>

#include "renderer_paint_server.h"
#include "renderer/qt2skia.h"

#pragma warning(push, 0)
#include <SkCanvas.h>
#include <SkSurface.h>
#include <SkDevice.h>
#pragma warning(pop)

renderer_base_shape_item::renderer_base_shape_item ()
{
  m_has_clip_path = false;
  visible = true;
  m_stroke = new SkPaint;
  m_stroke->setStyle (SkPaint::kStroke_Style);
  m_fill = new SkPaint;
  m_fill->setStyle (SkPaint::kFill_Style);
  m_stroke->setAntiAlias (true);
  m_fill->setAntiAlias (true);
}

renderer_base_shape_item::~renderer_base_shape_item ()
{
  FREE (m_stroke);
  FREE (m_fill);
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
    case Qt::MiterJoin: m_stroke->setStrokeJoin (SkPaint::kMiter_Join); break;
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

bool renderer_base_shape_item::configure_painter (SkPaint &paint, bool stroke) const
{
  if (!visible)
    return false;

  if (stroke)
    paint = *m_stroke;
  else
    paint = *m_fill;

  return true;
}

void renderer_base_shape_item::adjust_bbox (QRectF &bbox) const
{
  /// add pen width to a bbox
  double adjust_value = m_stroke->getStrokeWidth ();
  bbox.adjust (-adjust_value, -adjust_value, adjust_value, adjust_value);
  bbox = transform ().mapRect (bbox);
}

void renderer_base_shape_item::set_stroke_server (const renderer_paint_server *server)
{
  server->fill_paint (*m_stroke);
}

void renderer_base_shape_item::set_fill_server (const renderer_paint_server *server)
{
  server->fill_paint (*m_fill);
}

