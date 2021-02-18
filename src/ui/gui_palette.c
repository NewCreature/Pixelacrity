#include "t3f/t3f.h"
#include "t3gui/t3gui.h"
#include "instance.h"
#include "palette.h"
#include "gui_palette.h"

static ALLEGRO_BITMAP * _palette_bitmap = NULL;

int quixel_gui_palette_proc(int msg, T3GUI_ELEMENT * d, int c)
{
	switch(msg)
	{
		case MSG_START:
		{
			_palette_bitmap = quixel_create_palette(d->w, d->h);
			break;
		}
		case MSG_END:
		{
			if(_palette_bitmap)
			{
				al_destroy_bitmap(_palette_bitmap);
				_palette_bitmap = NULL;
			}
			break;
		}
		case MSG_MOUSEDOWN:
		{
			break;
		}
		case MSG_DRAW:
		{
			al_draw_bitmap(_palette_bitmap, d->x, d->y, 0);
			break;
		}
	}
	return D_O_K;
}
