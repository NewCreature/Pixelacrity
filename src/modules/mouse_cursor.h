#ifndef PA_MOUSE_CURSOR_H
#define PA_MOUSE_CURSOR_H

#include "t3f/t3f.h"

void pa_allow_mouse_cursor_changes(bool onoff);
void pa_set_mouse_cursor(ALLEGRO_SYSTEM_MOUSE_CURSOR cursor_id);
ALLEGRO_SYSTEM_MOUSE_CURSOR pa_get_mouse_cursor(void);

#endif
