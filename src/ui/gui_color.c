#include "t3f/t3f.h"
#include "t3gui/t3gui.h"
#include "instance.h"
#include "gui_color_picker.h"

int quixel_gui_color_proc(int msg, T3GUI_ELEMENT * d, int c)
{
	switch(msg)
	{
		case MSG_MOUSEDOWN:
		{
			if(d->dp2)
			{
				*(ALLEGRO_COLOR *)d->dp2 = *(ALLEGRO_COLOR *)d->dp;
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
