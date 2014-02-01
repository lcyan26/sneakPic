#include "stroke_style.h"


#include "gui/settings.h"

#include "common/memory_deallocation.h"

#include "gui/connection.h"

#include "editor/items_selection.h"
#include "editor/stroke_style.h"

#include "svg/attributes/svg_attribute_stroke_linecap.h"
#include "svg/attributes/svg_attribute_stroke_linejoin.h"
#include "svg/attributes/svg_attribute_stroke_miterlimit.h"
#include "svg/attributes/svg_attribute_stroke_width.h"
#include "svg/attributes/svg_attributes_fill_stroke.h"
#include "svg/attributes/svg_attributes_number.h"
#include "svg/items/abstract_svg_item.h"

stroke_style::stroke_style ()
{
  m_internal_color = new QColor ();
  m_color = 0;
  m_selection = 0;
  m_width = 0;
  m_settings = 0;
  m_linejoin = Qt::PenJoinStyle::SvgMiterJoin;
}

void stroke_style::init (settings_t *settings_arg)
{
  m_settings = settings_arg;
  m_color = settings_arg->stroke_color ();
  m_linecap = settings_arg->stroke_linecap ();
  m_linejoin = settings_arg->stroke_linejoin ();
  m_miterlimit = settings_arg->stroke_miterlimit ();
  m_width = settings_arg->stroke_width ();
}

void stroke_style::update_from_selection ()
{
  if (!m_selection)
    {
      return;
    }

  if (!is_single_item_selected ())
    {
      m_color = 0;
      return;
    }

  abstract_svg_item *selected_item = *m_selection->begin ();
  m_width = selected_item->get_computed_attribute <svg_attribute_stroke_width> ()->value ();
  m_linejoin = selected_item->get_computed_attribute <svg_attribute_stroke_linejoin> ()->value ();
  m_miterlimit = selected_item->get_computed_attribute <svg_attribute_stroke_miterlimit> ()->value ();
  m_linecap = selected_item->get_computed_attribute <svg_attribute_stroke_linecap> ()->value ();
  auto stroke = selected_item->get_computed_attribute <svg_attribute_stroke> ();
  auto stroke_opacity = (* (m_selection->begin ()))->get_computed_attribute <svg_attribute_stroke_opacity> ();
  if (stroke == 0)
  {
    m_color = 0;
    return;
  }
paint_server_type stroke_type = stroke->server_type ();
switch (stroke_type)
  {
  case paint_server_type::COLOR:
    *m_internal_color = stroke->color ();
      if (stroke_opacity != 0)
        m_internal_color->setAlphaF (stroke_opacity->value ());
      m_color = m_internal_color;
      break;
    case paint_server_type::NONE:
    case paint_server_type::IRI:
    case paint_server_type::CURRENT_COLOR:
      m_color = 0;
      break;
    }
}

void stroke_style::set_selection_color ()
{
  if (!m_color || !is_single_item_selected ())
    return;

  auto stroke = (* (m_selection->begin ()))->get_attribute_for_change <svg_attribute_stroke> ();
  auto stroke_opacity = (* (m_selection->begin ()))->get_attribute_for_change <svg_attribute_stroke_opacity> ();

  stroke->set_to_color (*m_color);
  stroke_opacity->set_value (m_color->alphaF ());
}

bool stroke_style::is_single_item_selected ()
{
  return m_selection && m_selection->count () == 1;
}

void stroke_style::init (items_selection *selection)
{
  m_selection = selection;
  update_from_selection ();
  // In this case should update each time selection is updated
}

stroke_style::~stroke_style ()
{
  FREE (m_internal_color);
}

void stroke_style::apply_color_to_selection ()
{
  // In this case we're updating color without calling apply changes.
  set_selection_color ();
}

void stroke_style::update_width (double value)
{
  m_width = value;

  if (m_settings)
    m_settings->set_stroke_width (value);
  else
    {
      if (is_single_item_selected ())
        {
           (* (m_selection->begin ()))->get_attribute_for_change <svg_attribute_stroke_width> ()->set_value (value);
        }
    }
}

void stroke_style::update_miterlimit (double value)
{
  m_miterlimit = value;

  if (m_settings)
    m_settings->set_stroke_miterlimit (value);
  else
  {
    if (is_single_item_selected ())
    {
      (* (m_selection->begin ()))->get_attribute_for_change <svg_attribute_stroke_miterlimit> ()->set_value (value);
    }
  }
}

void stroke_style::update_linejoin (Qt::PenJoinStyle value)
{
  m_linejoin = value;

  if (m_settings)
    m_settings->set_stroke_linejoin (value);
  else
  {
    if (is_single_item_selected ())
    {
      (* (m_selection->begin ()))->get_attribute_for_change <svg_attribute_stroke_linejoin> ()->set_value (value);
    }
  }
}

void stroke_style::update_linecap (Qt::PenCapStyle value)
{
  m_linecap = value;

  if (m_settings)
    m_settings->set_stroke_linecap (value);
  else
  {
    if (is_single_item_selected ())
    {
      (* (m_selection->begin ()))->get_attribute_for_change <svg_attribute_stroke_linecap> ()->set_value (value);
    }
  }
}

Qt::PenJoinStyle stroke_style::stroke_linejoin () const
{
  return m_linejoin;
}

Qt::PenCapStyle stroke_style::stroke_linecap () const
{
  return m_linecap;
}