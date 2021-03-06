#include "gui/gui_document.h"

#include <QTimer>
#include <QAction>

#include "common/memory_deallocation.h"

#include "editor/tools/tools_container.h"

#include "gui/canvas_widget_t.h"
#include "gui/gui_action_id.h"
#include "gui/gui_actions.h"
#include "gui/actions_applier.h"
#include "gui/connection.h"

#include "renderer/rendered_items_cache.h"
#include "renderer/events_queue.h"
#include "renderer/renderer_thread.h"
#include "renderer/svg_renderer.h"
#include "renderer/svg_painter.h"
#include "renderer/event_container_changed.h"

#include "svg/copy_paste_handler.h"

#include "svg/svg_document.h"
#include "editor/items_selection.h"
#include "svg/undo/undo_handler.h"
#include "multi_gui_model.h"
#include "path_operations_handler.h"
#include "object_operations_handler.h"
#include "svg/items/abstract_svg_item.h"


gui_document::gui_document (settings_t *settings, gui_actions *actions, style_controller *controller, multi_gui_model *color_model)
{
  m_color_model = color_model;
  m_style_controller = controller;
  m_actions = actions;
  m_painter = nullptr;
  m_settings = settings;
  m_actions_applier = new actions_applier;
  m_cache = new rendered_items_cache;
  m_queue = new events_queue;
  m_tools_container = new tools_container (m_actions);

  m_renderer_thread = new renderer_thread (new svg_renderer (m_cache, m_queue), m_queue);
  m_renderer_thread->start ();

  update_timer = new QTimer (this);
  update_timer->setInterval (50);
  update_timer->start ();

  CONNECT (update_timer, &QTimer::timeout, this, &gui_document::update_timeout);
  CONNECT (m_tools_container, &tools_container::tool_changed, this, &gui_document::tool_changed);
  CONNECT (m_actions, &gui_actions::actions_update_needed, this, &gui_document::update_actions);

  m_actions_applier->register_action (gui_action_id::UNDO, this, &gui_document::undo);
  m_actions_applier->register_action (gui_action_id::REDO, this, &gui_document::redo);
  m_actions_applier->register_action (gui_action_id::COPY, this, &gui_document::copy);
  m_actions_applier->register_action (gui_action_id::CUT, this, &gui_document::cut);
  m_actions_applier->register_action (gui_action_id::PASTE, this, &gui_document::paste);
  m_actions_applier->register_action (gui_action_id::PASTE_IN_PLACE, this, &gui_document::paste_in_place);
}

gui_document::~gui_document ()
{
  m_renderer_thread->set_exit_needed ();
  m_renderer_thread->wait ();
  FREE (m_renderer_thread);
  FREE (m_queue);
  FREE (m_tools_container);
  FREE (m_painter);
  FREE (m_cache);
  FREE (m_doc);
  FREE (m_actions_applier);
}

bool gui_document::open_file (const QString &filename)
{
  return create_new_document_impl ([=] (svg_document *doc) { return doc->read_file (filename); });
}

bool gui_document::save_file (const QString &filename)
{
  return m_doc->write_file (filename);
}

svg_painter *gui_document::create_painter (canvas_widget_t *widget)
{
  FREE (m_painter);
  m_painter = new svg_painter (widget, m_cache, m_queue, m_doc, m_settings, this);
  put_in (m_copy_paste_handler, m_painter);
  put_in (m_path_handler, m_painter, m_actions_applier);
  put_in (m_object_handler, m_painter, m_actions_applier);
  m_tools_container->update_tools (m_painter);
  m_painter->set_current_tool (m_tools_container->current_tool ());
  widget->set_painter (m_painter);
  return m_painter;
}

QString gui_document::get_filename () const
{
  return m_doc ? m_doc->get_filename () : QString ();
}

void gui_document::update_timeout ()
{
  if (m_cache->has_pending_changes ())
    m_painter->update ();
}

bool gui_document::undo ()
{
  m_doc->undo ();
  return true;
}

bool gui_document::redo ()
{
  m_doc->redo ();
  return true;
}

bool gui_document::paste ()
{
  m_copy_paste_handler->paste (false);
  return true;
}

bool gui_document::paste_in_place ()
{
  m_copy_paste_handler->paste (true);
  return true;
}

bool gui_document::copy ()
{
  if (m_painter->selection ()->empty ())
    return false;

  m_copy_paste_handler->copy ();
  return true;
}

bool gui_document::cut ()
{
  items_selection *selection = m_painter->selection ();
  if (selection->empty ())
    return false;

  m_copy_paste_handler->copy ();
  for (auto item : *selection)
    {
      if (item && item->parent ())
        item->parent ()->remove_child (item);
    }

  selection->clear ();
  m_doc->apply_changes ("Cut");
  return true;
}

void gui_document::tool_changed ()
{
  m_painter->set_current_tool (m_tools_container->current_tool ());
}

bool gui_document::action_triggered (gui_action_id id)
{
  if (id != gui_action_id::INTERRUPT)
    {
      if (action_triggered (gui_action_id::INTERRUPT))
        return true;
    }

  if (m_tools_container->action_triggered (id))
    return true;

  if (m_painter && m_painter->action_triggered (id))
    return true;

  return m_actions_applier->apply_action (id);
}

bool gui_document::create_new_document ()
{
  return create_new_document_impl ([] (svg_document *doc) { return doc->create_new_document (); });
}

bool gui_document::create_new_document_impl (std::function <bool (svg_document *)> create_func)
{
  m_doc = new svg_document ();
  m_doc->set_queue (m_queue);
  if (!create_func (m_doc))
    return false;

  renderer_items_container *renderer_items = m_doc->create_rendered_items (m_cache);
  m_queue->add_event (new event_container_changed (renderer_items));
  CONNECT (m_doc, &svg_document::items_changed, this, &gui_document::items_changed);
  items_changed ();
  return true;
}

bool gui_document::is_new_document ()
{
  return m_doc->is_new_document ();
}

void gui_document::update_actions ()
{
  undo_handler *handler = m_doc->get_undo_handler ();
  m_actions->action (gui_action_id::UNDO)->setEnabled (handler->has_undo ());
  m_actions->action (gui_action_id::UNDO)->setText ("Undo " + handler->undo_name ());

  m_actions->action (gui_action_id::REDO)->setEnabled (handler->has_redo ());
  m_actions->action (gui_action_id::REDO)->setText ("Redo " + handler->redo_name ());
}

void gui_document::items_changed ()
{
  m_actions->actions_update_needed ();
}

void gui_document::add_color_model (gui_model *model)
{
  m_color_model->add_model (model);
}

void gui_document::remove_color_model (gui_model *model)
{
  m_color_model->remove_model (model);
}
