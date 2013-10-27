#include "gui/menu_builder.h"

#include <QMenuBar>

#include "gui/gui_actions.h"
#include "gui/gui_action_id.h"

menu_builder::menu_builder (QMenuBar *bar, const gui_actions *actions)
{
  m_bar = bar;
  m_actions = actions;
  update_menu ();
}

menu_builder::~menu_builder ()
{

}

void menu_builder::update_menu ()
{
  m_bar->clear ();
  create_file_menu (m_bar->addMenu ("File"));
  create_edit_menu (m_bar->addMenu ("Edit"));
}

void menu_builder::create_file_menu (QMenu *menu)
{
  add_action (menu, gui_action_id::OPEN);
  add_action (menu, gui_action_id::OPEN_RECENT);
  menu->addSeparator ();

  add_action (menu, gui_action_id::SAVE_AS);
  menu->addSeparator ();
  add_action (menu, gui_action_id::QUIT);
}

void menu_builder::add_action (QMenu *menu, gui_action_id id)
{
  menu->addAction (m_actions->action (id));
}

void menu_builder::create_edit_menu (QMenu *menu)
{
  add_action (menu, gui_action_id::UNDO);
  add_action (menu, gui_action_id::REDO);
}