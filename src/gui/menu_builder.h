#ifndef MENU_BUILDER_H
#define MENU_BUILDER_H

class QMenuBar;
class QMenu;

class gui_actions;
class dock_widget_builder;

enum class   gui_action_id;

class menu_builder
{
  const gui_actions *m_actions;
  QMenuBar *m_bar;
  QMenu *m_view_menu;
  dock_widget_builder *m_dock_widget_builder;
public:
  menu_builder (QMenuBar *bar, const gui_actions *actions, QMenu *view_menu);
  ~menu_builder ();

  void update_menu ();

private:
  void create_file_menu (QMenu *menu);
  void add_action (QMenu *menu, gui_action_id id);
  void create_edit_menu (QMenu *menu);
  void create_view_menu( QMenu *menu);
  void create_options_menu (QMenu *menu);
  void create_path_menu (QMenu *menu);
  void create_object_menu (QMenu *menu);
};

#endif // MENU_BUILDER_H
