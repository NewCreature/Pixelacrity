#include "t3f/t3f.h"
#include "t3gui/t3gui.h"
#include "instance.h"
#include "gui_color.h"

int quixel_gui_color_proc(int msg, T3GUI_ELEMENT * d, int c)
{
	ALLEGRO_STATE old_state;
	ALLEGRO_COLOR color;
	float r, g, b, a;

	switch(msg)
	{
		case MSG_MOUSEDOWN:
		{
			if(d->dp2)
			{
				if(c == 1)
				{
					*(ALLEGRO_COLOR *)d->dp2 = *(ALLEGRO_COLOR *)d->dp;
				}
				else
				{
					*(ALLEGRO_COLOR *)d->dp3 = *(ALLEGRO_COLOR *)d->dp;
				}
			}
			break;
		}
		case MSG_DRAW:
		{
			al_store_state(&old_state, ALLEGRO_STATE_BLENDER);
			al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA);
			al_unmap_rgba_f(*(ALLEGRO_COLOR *)d->dp, &r, &g, &b, &a);
			color = al_map_rgba_f(r * a, g * a, b * a, a);
			al_draw_filled_rectangle(d->x, d->y, d->x + d->w, d->y + d->h, color);
			al_restore_state(&old_state);
			break;
		}
	}
	return D_O_K;
}
