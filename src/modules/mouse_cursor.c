#include "t3f/t3f.h"

static bool allow_changes = true;
static ALLEGRO_SYSTEM_MOUSE_CURSOR current_cursor_id = ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT;

void pa_allow_mouse_cursor_changes(bool onoff)
{
	allow_changes = onoff;
}

void pa_set_mouse_cursor(ALLEGRO_SYSTEM_MOUSE_CURSOR cursor_id)
{
	if(allow_changes)
	{
		if(cursor_id != current_cursor_id)
		{
			al_set_system_mouse_cursor(t3f_display, cursor_id);
		}
	}
}

ALLEGRO_SYSTEM_MOUSE_CURSOR pa_get_mouse_cursor(void)
{
	return current_cursor_id;
}
