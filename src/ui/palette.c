#include "t3f/t3f.h"

ALLEGRO_BITMAP * quixel_create_palette(int width, int height)
{
	ALLEGRO_BITMAP * bp;
	ALLEGRO_COLOR color;
	ALLEGRO_STATE old_state;
	ALLEGRO_TRANSFORM identity;
	int i, j;
	float h, s, l;

	bp = al_create_bitmap(width, height);
	if(bp)
	{
		al_store_state(&old_state, ALLEGRO_STATE_TARGET_BITMAP | ALLEGRO_STATE_TRANSFORM);
		al_identity_transform(&identity);
		al_lock_bitmap(bp, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_WRITEONLY);
		al_set_target_bitmap(bp);
		al_use_transform(&identity);
		for(i = 0; i < height; i++)
		{
			for(j = 0; j < width; j++)
			{
				h = (float)i * (360.0 / (float)height);
				l = 0.5;
				s = (float)j * (1.0 / (float)width);
				color = al_color_hsl(h, s, l);
				al_put_pixel(j, i, color);
			}
		}
		al_unlock_bitmap(bp);
		al_restore_state(&old_state);
		return bp;
	}
	return NULL;
}
