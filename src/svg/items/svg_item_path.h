#ifndef SVG_ITEM_PATH_H
#define SVG_ITEM_PATH_H

#include "svg/items/svg_base_shape_item.h"

class svg_path_geom;

class svg_item_path : public svg_base_shape_item
{
  SVG_ITEM

public:
  svg_item_path (svg_document *document);
  virtual ~svg_item_path () override;

  const svg_path_geom *get_svg_path () const;
  virtual QPainterPath get_path () const override;

  virtual bool process_item_after_read () override;
protected:
};


#endif // SVG_ITEM_PATH_H
