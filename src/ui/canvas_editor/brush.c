#include "t3f/t3f.h"

ALLEGRO_BITMAP * pa_create_default_brush(void)
{
	ALLEGRO_BITMAP * bp;
	ALLEGRO_STATE old_state;

	al_store_state(&old_state, ALLEGRO_STATE_TARGET_BITMAP | ALLEGRO_STATE_NEW_BITMAP_PARAMETERS);
	al_set_new_bitmap_flags(0);
	bp = al_create_bitmap(1, 1);
	if(bp)
	{
		al_set_target_bitmap(bp);
		al_clear_to_color(t3f_color_white);
	}
	al_restore_state(&old_state);
	return bp;
}
