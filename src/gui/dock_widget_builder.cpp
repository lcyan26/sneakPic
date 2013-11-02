#include "gui\dock_widget_builder.h"

#include <QDockWidget>
#include <QMainWindow>
#include <QWidget>

dock_widget_builder::dock_widget_builder (QMainWindow *main_window)
{
  m_main_window = main_window;
}

void dock_widget_builder::add_widget (QWidget *widget, Qt::DockWidgetArea default_pos, visibility_state default_visibility)
{
  QDockWidget *dock_widget = new QDockWidget (widget->windowTitle ());
  widget->setParent (dock_widget);
  dock_widget->setFeatures (QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
  dock_widget->setAllowedAreas (Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  dock_widget->setWidget (widget);
  m_main_window->addDockWidget (default_pos, dock_widget);
  dock_widget->setVisible (default_visibility == visibility_state::visible);
}
