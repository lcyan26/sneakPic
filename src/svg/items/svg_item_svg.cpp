#include "svg/items/svg_item_svg.h"

#include "common/memory_deallocation.h"

#include "svg/attributes/abstract_attribute.h"
#include "svg/attributes/svg_attributes_length_type.h"

#include "renderer/renderer_item_svg.h"


svg_item_svg::svg_item_svg (svg_document *document)
  : svg_item_group_type (document)
{
}

svg_item_svg::~svg_item_svg ()
{
}

double svg_item_svg::width () const
{
  static const svg_attribute_width default_width (nullptr, 210, svg_length_units::MM); /// A4
  return get_computed_attribute<svg_attribute_width> (&default_width)->value ();
}

double svg_item_svg::height () const
{
  static const svg_attribute_height default_height (nullptr, 297, svg_length_units::MM); /// A4
  return get_computed_attribute<svg_attribute_height> (&default_height)->value ();
}

void svg_item_svg::update_renderer_item ()
{

}

abstract_renderer_item *svg_item_svg::create_renderer_item () const
{
  renderer_item_svg *render_item = new renderer_item_svg (id ().toStdString ());

  render_item->set_height (height ());
  render_item->set_width (width ());
  update_group_item (render_item);
  return render_item;
}

