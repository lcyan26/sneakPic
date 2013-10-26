#ifndef PATH_EDITOR_TOOL_H
#define PATH_EDITOR_TOOL_H

#include "editor/tools/abstract_tool.h"

class mouse_shortcuts_handler;
class svg_painter;
class rubberband_selection;
class overlay_renderer;

class path_editor_tool : public abstract_tool
{
  svg_painter             *m_painter;

  mouse_shortcuts_handler *m_mouse_handler;
  rubberband_selection    *m_rubberband;
  overlay_renderer        *m_overlay;

public:
  path_editor_tool (svg_painter *painter);
  ~path_editor_tool ();


protected:
  virtual void activate () override;
  virtual void deactivate () override;
  virtual bool mouse_event (const mouse_event_t &m_event) override;

  virtual void configure () override;
  virtual void draw (QPainter &painter, const QRect &rect_to_draw, const QTransform &transform) override;

private:
  bool start_rubberband_selection (const QPoint &pos);
  bool move_rubberband_selection (const QPoint &pos);
  bool end_rubberband_selection (const mouse_event_t &event);
};

#endif // PATH_EDITOR_TOOL_H
