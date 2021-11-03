#include "t3f/t3f.h"
#include "t3gui/t3gui.h"
#include "modules/color.h"
#include "instance.h"

int pa_gui_color_proc(int msg, T3GUI_ELEMENT * d, int c)
{
	switch(msg)
	{
		case MSG_MOUSEDOWN:
		{
			if(t3f_key[ALLEGRO_KEY_LCTRL] || t3f_key[ALLEGRO_KEY_RCTRL] || t3f_key[ALLEGRO_KEY_COMMAND])
			{
				if(c == 1 && d->dp4)
				{
					*(ALLEGRO_COLOR *)d->dp = *(ALLEGRO_COLOR *)d->dp4;
				}
				else if(d->dp5)
				{
					*(ALLEGRO_COLOR *)d->dp = *(ALLEGRO_COLOR *)d->dp5;
				}
			}
			else
			{
				if(c == 1 && d->dp2)
				{
					*(ALLEGRO_COLOR *)d->dp2 = *(ALLEGRO_COLOR *)d->dp;
				}
				else if(d->dp3)
				{
					*(ALLEGRO_COLOR *)d->dp3 = *(ALLEGRO_COLOR *)d->dp;
				}
			}
			break;
		}
		case MSG_DRAW:
		{
			al_draw_filled_rectangle(d->x, d->y, d->x + d->w, d->y + d->h, *(ALLEGRO_COLOR *)d->dp);
			break;
		}
	}
	return D_O_K;
}
