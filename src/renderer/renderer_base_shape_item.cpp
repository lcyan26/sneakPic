#include "renderer/renderer_base_shape_item.h"

#include <QPainter>

renderer_base_shape_item::renderer_base_shape_item ()
{
}

renderer_base_shape_item::~renderer_base_shape_item ()
{
}

void renderer_base_shape_item::set_stroke_linecap (Qt::PenCapStyle linecap)
{
  m_pen.setCapStyle (linecap);
}

void renderer_base_shape_item::set_stroke_linejoin (Qt::PenJoinStyle linejoin)
{
  m_pen.setJoinStyle (linejoin);
}

void renderer_base_shape_item::set_stroke_miterlimit (double miterlimit)
{
  m_pen.setMiterLimit (miterlimit);
}

void renderer_base_shape_item::set_stroke_width (double width)
{
  m_pen.setWidthF (width);
}

void renderer_base_shape_item::set_stroke_color (const QColor &color)
{
  m_pen.setColor (color);
}

void renderer_base_shape_item::set_show_stroke (bool show)
{
  m_pen.setStyle (show ? Qt::SolidLine : Qt::NoPen);
}

void renderer_base_shape_item::set_fill_color (const QColor &color)
{
  m_brush.setColor (color);
}

void renderer_base_shape_item::set_show_fill (bool show)
{
  m_brush.setStyle (show ? Qt::SolidPattern : Qt::NoBrush);
}

void renderer_base_shape_item::configure_painter (QPainter &painter) const
{
  painter.setPen (m_pen);
  painter.setBrush (m_brush);
}

void renderer_base_shape_item::adjust_bbox (QRectF &bbox) const
{
  /// add pen width to a bbox
  double adjust_value = m_pen.widthF ();
  bbox.adjust (-adjust_value, -adjust_value, adjust_value, adjust_value);
}