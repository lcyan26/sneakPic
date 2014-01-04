#include "qt_utils.h"
// File to contain some stuff which have to do with Qt, later some parts may be moved to separate files

#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>

QWidget *create_container_widget (QWidget *parent)
{
  QWidget *widget = new QWidget (parent);
  return widget;
}

void finish_with_spacer (QGridLayout *layout)
{
  layout->addItem (new QSpacerItem (0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding), layout->rowCount (), 0);
}

QGridLayout *create_common_grid_layout (QWidget *parent)
{
  QWidget *widget = create_container_widget (parent);
  QGridLayout *layout = new QGridLayout (widget);
  layout->setContentsMargins (6, 6, 6, 6);
  layout->setVerticalSpacing (5);
  layout->setHorizontalSpacing (10);
  return layout;
}

QHBoxLayout *create_common_hbox_layout (QWidget *parent)
{
  QWidget *widget = create_container_widget (parent);
  QHBoxLayout *layout = new QHBoxLayout (widget);
  layout->setContentsMargins (6, 6, 6, 6);
  layout->setSpacing (10);
  return layout;
}

QVBoxLayout *create_common_vbox_layout (QWidget *parent)
{
  QWidget *widget = create_container_widget (parent);
  QVBoxLayout *layout = new QVBoxLayout (widget);
  layout->setContentsMargins (6, 6, 6, 6);
  layout->setSpacing (5);
  return layout;
}

QGridLayout *create_inner_grid_layout (QWidget *parent)
{
  QWidget *widget = create_container_widget (parent);
  if (parent) // if parent is nullptr then layout will be placed somewhere afterwards
    parent->layout ()->addWidget (widget);
  QGridLayout *layout = new QGridLayout (widget);
  layout->setVerticalSpacing (5);
  layout->setHorizontalSpacing (10);
  layout->setContentsMargins (0, 0, 0, 0);
  return layout;
}

QHBoxLayout *create_inner_hbox_layout (QWidget *parent)
{
  QWidget *widget = create_container_widget (parent);
  if (parent) // if parent is nullptr then layout will be placed somewhere afterwards
    parent->layout ()->addWidget (widget);
  QHBoxLayout *layout = new QHBoxLayout (widget);
  layout->setSpacing (10);
  layout->setContentsMargins (0, 0, 0, 0);
  return layout;
}

QHBoxLayout *create_inner_hbox_layout (QLayout *parent_layout)
{
  return create_inner_hbox_layout (parent_layout->parentWidget ());
}

QVBoxLayout *create_inner_vbox_layout (QWidget *parent)
{
  QWidget *widget = create_container_widget (parent);
  if (parent) // if parent is nullptr then layout will be placed somewhere afterwards
    parent->layout ()->addWidget (widget);
  QVBoxLayout *layout = new QVBoxLayout (widget);
  layout->setSpacing (5);
  layout->setContentsMargins (0, 0, 0, 0);
  return layout;
}

QVBoxLayout *create_inner_vbox_layout (QLayout *parent_layout)
{
  return create_inner_vbox_layout (parent_layout->parentWidget ());
}
