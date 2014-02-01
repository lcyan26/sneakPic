#ifndef PEN_TOOL_H
#define PEN_TOOL_H

#include "editor/tools/abstract_tool.h"

class path_preview_renderer;
class svg_path_geom;
class path_builder;
class anchor_handle_renderer;
class path_control_point_renderer;
class pen_handles;
class svg_path_geom_iterator;
class svg_item_path;

class pen_tool : public abstract_tool
{
  typedef std::pair<svg_item_path *, svg_path_geom_iterator> snap_point_t;
  std::unique_ptr<path_preview_renderer> m_preview_renderer;
  std::unique_ptr<path_control_point_renderer> m_left_cp_renderer;
  std::unique_ptr<path_control_point_renderer> m_right_cp_renderer;
  std::unique_ptr<svg_path_geom> m_current_path;
  std::unique_ptr<path_builder> m_path_builder;
  std::unique_ptr<pen_handles> m_pen_handles;

  std::unique_ptr<snap_point_t> m_path_snap_start;
  std::unique_ptr<snap_point_t> m_path_snap_end;

public:
  pen_tool (svg_painter *painter);
  ~pen_tool ();

  virtual void configure () override;
  virtual void deactivate () override;
  virtual void activate () override;

private:
  bool add_segment_simple (const QPoint &pos);
  bool add_segment_start (const QPoint &pos);
  bool add_segment_move (const QPoint &pos);
  bool add_segment_end (const QPoint &pos);
  bool finish_path_add ();

  QPointF snap_point (QPointF point);

  void add_new_point (QPoint pos);
  svg_item_path *add_new_path ();
  svg_item_path *merge_with_path (svg_item_path *path_dst, svg_path_geom_iterator dst_it, svg_item_path *path_src, svg_path_geom_iterator src_it);
};


#endif // PEN_TOOL_H
