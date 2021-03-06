#ifndef SVG_BASE_ITEMS_GRADIENT_H
#define SVG_BASE_ITEMS_GRADIENT_H

#include "svg/items/abstract_svg_item.h"

class renderer_base_gradient_item;
class renderer_paint_server;

enum class gradient_type_t
{
  LINEAR,
  RADIAL,
};

class svg_base_items_gradient : public abstract_svg_item
{

public:
  svg_base_items_gradient (svg_document *document) : abstract_svg_item (document) {}
  virtual ~svg_base_items_gradient () override {}

  renderer_paint_server *create_paint_server () const;
  bool fill_gradient (renderer_base_gradient_item *gradient) const;

  bool add_stops_to_gradient (const abstract_svg_item *parent, renderer_base_gradient_item *gradient) const;

  virtual gradient_type_t gradient_type () const = 0;

protected:
  virtual renderer_base_gradient_item *create_gradient () const = 0;
};

#endif // SVG_BASE_ITEMS_GRADIENT_H
