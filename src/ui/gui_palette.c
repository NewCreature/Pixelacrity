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
			d->flags |= D_TRACKMOUSE;
			if(d->dp)
			{
				if(t3f_mouse_x - d->x >= 0 && t3f_mouse_x - d->x < al_get_bitmap_width(_palette_bitmap) && t3f_mouse_y - d->y >= 0 && t3f_mouse_y - d->y < al_get_bitmap_height(_palette_bitmap))
				{
					*(ALLEGRO_COLOR *)d->dp = al_get_pixel(_palette_bitmap, t3f_mouse_x - d->x, t3f_mouse_y - d->y);
				}
			}
			break;
		}
		case MSG_MOUSEUP:
		{
			d->flags &= ~D_TRACKMOUSE;
			break;
		}
		case MSG_MOUSEMOVE:
		{
			if(t3f_mouse_button[0])
			{
				quixel_gui_palette_proc(MSG_MOUSEDOWN, d, c);
			}
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
