#include "editor/handles_renderer.h"

#include "renderer/overlay_renderer.h"
#include "renderer/overlay_item_type.h"
#include "renderer/renderable_item.h"

#include "editor/items_selection.h"
#include "editor/element_handles.h"
#include "editor/abstract_handle.h"
#include "handles_editor.h"

handles_renderer::handles_renderer (const handles_editor *editor)
{
  m_editor = editor;
}

handles_renderer::~handles_renderer ()
{

}

void handles_renderer::draw (SkCanvas &canvas, const renderer_state &state, const renderer_config *config) const 
{
  for (const auto &item : *m_editor)
    {
      for (auto handle : item.second->handles ())
          handle->draw (canvas, state, config);
    }
}


