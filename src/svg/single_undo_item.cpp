#include "svg/single_undo_item.h"

#include <memory>

#include "common/memory_deallocation.h"
#include "common/debug_utils.h"

#include "svg/undoable_items_container.h"
#include "svg/svg_document.h"
#include "svg/undoable.h"


single_undo_item::item_and_diff::item_and_diff (int id, abstract_state_diff_t *diff)
  : m_id (id), m_diff (diff )
{

}

single_undo_item::item_and_diff::item_and_diff (item_and_diff &&rhs)
{
  m_id = rhs.m_id;
  m_diff = rhs.m_diff;
  rhs.m_diff = 0;
}

single_undo_item::item_and_diff::~item_and_diff ()
{
  FREE (m_diff);
}

single_undo_item::single_undo_item (svg_document *document)
{
  m_document = document;
}

single_undo_item::~single_undo_item ()
{
}

void single_undo_item::undo ()
{
  apply (diff_direction_t::UNDO);
}

void single_undo_item::redo ()
{
  apply (diff_direction_t::REDO);
}

void single_undo_item::add_item_diff (int id, abstract_state_diff_t *diff)
{
  m_diff_map.push_back (std::move (item_and_diff (id, diff)));
}

void single_undo_item::apply (diff_direction_t direction)
{
  undoable_items_container_t *items_container = m_document->get_undoable_items_container ();
  for (const auto &element : m_diff_map)
    {
      int id = element.m_id;
      abstract_state_diff_t *diff = element.m_diff;

      undoable *item = items_container->get_item (id);
      std::unique_ptr<abstract_state_t> cur_state (item ? item->create_state () : nullptr);
      std::unique_ptr<abstract_state_t> new_state (diff->apply_diff (cur_state.get (), direction));
      if (new_state)
        {
          if (!item)
            {
              item = new_state->create_new_item ();
              items_container->add_item (item);
            }

          item->load_from_state (new_state.get ());
        }
      else
        {
          if (item)
            {
              item->load_from_state (nullptr);
              items_container->remove_item (item);
            }
        }
    }
}

