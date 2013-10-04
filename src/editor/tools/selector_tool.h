#ifndef SELECTOR_TOOL_H
#define SELECTOR_TOOL_H

#include "editor/tools/abstract_tool.h"

class mouse_shortcuts_handler;
class svg_painter;
class rubberband_selection;
class items_move_handler;

class selector_tool : public abstract_tool
{
  mouse_shortcuts_handler *m_mouse_handler;
  rubberband_selection    *m_rubberband;
  svg_painter             *m_painter;
  items_move_handler      *m_move_handler;
public:
  selector_tool (svg_painter *painter);
  ~selector_tool ();

  virtual void activate () override;
  virtual void deactivate () override;
  virtual bool mouse_event (const mouse_event_t &m_event) override;

  virtual void items_changed () override;

private:
  bool start_rubberband_selection (const QPoint &pos);
  bool move_rubberband_selection (const QPoint &pos);
  bool end_rubberband_selection (const mouse_event_t &event);
  
  bool start_moving_object (const QPoint &pos);
  bool move_object (const QPoint &pos);
  bool end_moving_object ();
};

#endif // SELECTOR_TOOL_H
