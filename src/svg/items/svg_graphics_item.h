#ifndef SVG_GRAPHICS_ITEM_H
#define SVG_GRAPHICS_ITEM_H

#include "svg/items/abstract_svg_item.h"

class QTransform;

class svg_graphics_item : public abstract_svg_item
{
public:
  svg_graphics_item (svg_document *document);
  ~svg_graphics_item ();

  virtual const svg_graphics_item *to_graphics_item () const override { return this; }
  virtual svg_graphics_item *to_graphics_item () override { return this; }

  virtual abstract_renderer_item *create_renderer_item () const = 0;
  virtual abstract_renderer_item *create_overlay_item (overlay_item_type overlay_type) const = 0;

  QTransform full_transform () const;
};

#endif // SVG_GRAPHICS_ITEM_H
