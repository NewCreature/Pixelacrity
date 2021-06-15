#include "t3f/t3f.h"

void pa_combine_bitmap(ALLEGRO_BITMAP * src, ALLEGRO_BITMAP * dest)
{
	ALLEGRO_STATE old_state;
	ALLEGRO_COLOR color;
	ALLEGRO_TRANSFORM identity;
	int i, j;
	unsigned char r, a;

	al_store_state(&old_state, ALLEGRO_STATE_TRANSFORM | ALLEGRO_STATE_TARGET_BITMAP);
//	al_lock_bitmap(src, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READONLY);
//	al_lock_bitmap(dest, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_WRITEONLY);
	al_set_target_bitmap(dest);
	al_identity_transform(&identity);
	al_use_transform(&identity);
	for(i = 0; i < al_get_bitmap_height(src); i++)
	{
		for(j = 0; j < al_get_bitmap_width(src); j++)
		{
			color = al_get_pixel(src, j, i);
			al_unmap_rgba(color, &r, &r, &r, &a);
			if(a > 0)
			{
				al_put_pixel(j, i, color);
			}
		}
	}
//	al_unlock_bitmap(src);
//	al_unlock_bitmap(dest);
	al_restore_state(&old_state);
}
