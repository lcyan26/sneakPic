#ifndef SVG_ITEM_GROUP_H
#define SVG_ITEM_GROUP_H

#include "svg/items/svg_item_group_type.h"

class renderer_item_group;

class svg_item_group : public svg_item_group_type
{
  SVG_ITEM

  renderer_item_group *m_renderer_item;
public:
  svg_item_group (svg_document *document);
  virtual ~svg_item_group () override;

  virtual void update_renderer_item () override;
  virtual const abstract_renderer_item *get_renderer_item () const override;

protected:
  virtual bool check_item () override { return true; }
};


#endif // SVG_ITEM_GROUP_H
