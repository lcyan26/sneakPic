#include "gui/shortcuts_config.h"

#include "common/common_utils.h"

#include "gui/mouse_shortcuts_handler.h"



#define ADD_MODIFIER(MODIFIER) keyboard_modifier::MODIFIER |

#define MOUSE_SHORTCUT(MOUSE_EVENT_TYPE,MOUSE_BUTTON,...) \
  EXPAND (mouse_shortcut (mouse_event_type::MOUSE_EVENT_TYPE, mouse_button::MOUSE_BUTTON, FOR_EACH (ADD_MODIFIER, __VA_ARGS__) keyboard_modifier::NONE))

#define SET_MOUSE_SHORTCUT(INDEX,MOUSE_EVENT_TYPE,MOUSE_BUTTON,...) \
  EXPAND (m_mouse_shortcuts[(int)mouse_shortcut_enum::INDEX] = MOUSE_SHORTCUT (MOUSE_EVENT_TYPE, MOUSE_BUTTON, __VA_ARGS__);)

shortcuts_config::shortcuts_config ()
{

}

shortcuts_config::~shortcuts_config ()
{

}

void shortcuts_config::fill_by_default ()
{
///------------------------------------------------------------------------------------
///                  |    SHORTCUT_NAME         | EVENT_TYPE|    BUTTON    | MODIFIERS
  SET_MOUSE_SHORTCUT (SELECT_ITEM               , CLICK     , BUTTON_LEFT  , NO_MODIFIERS);
  SET_MOUSE_SHORTCUT (ADD_ITEM_TO_SELECTION     , CLICK     , BUTTON_LEFT  , SHIFT       );
  SET_MOUSE_SHORTCUT (START_PAN                 , DRAG_START, BUTTON_MIDDLE, ALL         );
  SET_MOUSE_SHORTCUT (PAN_PICTURE               , DRAG      , BUTTON_MIDDLE, ALL         );
  SET_MOUSE_SHORTCUT (FIND_CURRENT_OBJECT       , MOVE      , ANY_BUTTON   , ALL         );
  SET_MOUSE_SHORTCUT (START_RUBBERBAND_SELECTION, DRAG_START, BUTTON_LEFT  , NO_MODIFIERS, SHIFT  );
  SET_MOUSE_SHORTCUT (MOVE_RUBBERBAND_SELECTION , DRAG      , BUTTON_LEFT  , NO_MODIFIERS, SHIFT  );
  SET_MOUSE_SHORTCUT (END_RUBBERBAND_SELECTION  , DRAG_END  , BUTTON_LEFT  , NO_MODIFIERS, SHIFT  );
}


