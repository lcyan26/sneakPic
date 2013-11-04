#include "editor/tools/abstract_tool.h"

#include "common/memory_deallocation.h"

#include "gui/mouse_shortcuts_handler.h"
#include "gui/settings.h"
#include "gui/actions_applier.h"

#include "renderer/svg_painter.h"
#include "renderer/overlay_renderer.h"




abstract_tool::abstract_tool (svg_painter *painter)
{
  m_painter = painter;
  m_mouse_handler = new mouse_shortcuts_handler (m_painter->settings ()->shortcuts_cfg ());
  m_overlay = new overlay_renderer;
  m_actions_applier = new actions_applier;
}

abstract_tool::~abstract_tool ()
{
  FREE (m_mouse_handler);
  FREE (m_overlay);
  FREE (m_actions_applier);
}

void abstract_tool::draw (QPainter &painter, const QRect &rect_to_draw, const QTransform &transform)
{
  m_overlay->draw (painter, rect_to_draw, transform);
}

bool abstract_tool::mouse_event (const mouse_event_t &m_event)
{
  return m_mouse_handler->process_mouse_event (m_event);
}

bool abstract_tool::action_triggered (gui_action_id id)
{
  return m_actions_applier->apply_action (id);
}

void abstract_tool::activate ()
{

}

void abstract_tool::deactivate ()
{

}

void abstract_tool::configure ()
{

}
