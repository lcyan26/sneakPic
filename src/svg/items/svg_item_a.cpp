#include "svg/items/svg_item_a.h"

#include "renderer/renderer_item_group.h"

svg_item_a::svg_item_a (svg_document *document)
  : svg_item_group_type (document)
{
}

svg_item_a::~svg_item_a ()
{
}

abstract_renderer_item *svg_item_a::create_renderer_item () const
{
  renderer_item_group *render_item = new renderer_item_group (name ().toStdString ());
  return render_item;
}

bool svg_item_a::check_item ()
{
  return true;
}