#ifndef SVG_ITEM_GROUP_H
#define SVG_ITEM_GROUP_H

#include "svg/items/svg_item_group_type.h"

class renderer_item_group;

class svg_item_group : public svg_item_group_type
{
  SVG_ITEM

public:
  svg_item_group (svg_document *document);
  virtual ~svg_item_group () override;

  virtual abstract_renderer_item *create_renderer_item_impl () const override;

};


#endif // SVG_ITEM_GROUP_H
