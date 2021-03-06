#include "svg/attributes/svg_attribute_xlink_href.h"

#include "svg/items/svg_items_container.h"
#include "svg/items/abstract_svg_item.h"
#include "svg/svg_document.h"
#include "svg/svg_item_observer.h"
#include "svg/simple_item_observer.h"
#include "svg/changed_items_container.h"
#include "svg/undo/undo_handler.h"

svg_attribute_xlink_href::svg_attribute_xlink_href ()
{

}

svg_attribute_xlink_href::~svg_attribute_xlink_href ()
{
}

bool svg_attribute_xlink_href::read (const char *data, bool /*from_css*/)
{
  bool ret = m_iri.read (data);
  update_observers ();
  return ret;
}

bool svg_attribute_xlink_href::write (QString &data, bool /*to_css*/) const
{
  return m_iri.write (data);
}

iri_type svg_attribute_xlink_href::get_data_type () const
{
  return m_iri.get_type ();
}


QImage svg_attribute_xlink_href::get_image_data (const QString &svg_name) const
{
  return m_iri.get_image_data (svg_name);
}

string svg_attribute_xlink_href::get_fragment_name () const
{
  return m_iri.get_fragment_name ();
}


