#ifndef PA_UI_MENU_TOOL_H
#define PA_UI_MENU_TOOL_H

#include "t3f/t3f.h"

ALLEGRO_MENU * pa_create_tool_brush_menu(void);
ALLEGRO_MENU * pa_create_tool_menu(ALLEGRO_MENU * brush_mp, int * item_array);

#endif
