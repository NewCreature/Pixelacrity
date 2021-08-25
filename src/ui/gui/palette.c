#include "t3f/t3f.h"
#include "t3gui/t3gui.h"
#include "instance.h"
#include "palette.h"

static ALLEGRO_BITMAP * _palette_bitmap = NULL;
static int mouse_button = 0;

static ALLEGRO_BITMAP * pa_create_palette(int width, int height)
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

void pa_resize_palette(int width, int height)
{
	if(_palette_bitmap)
	{
		al_destroy_bitmap(_palette_bitmap);
	}
	_palette_bitmap = pa_create_palette(width, height);
}

int pa_gui_palette_proc(int msg, T3GUI_ELEMENT * d, int c)
{
	switch(msg)
	{
		case MSG_START:
		{
			pa_resize_palette(d->w, d->h);
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
			mouse_button = c;
			if(d->dp)
			{
				if(t3gui_get_mouse_x() - d->x >= 0 && t3gui_get_mouse_x() - d->x < al_get_bitmap_width(_palette_bitmap) && t3gui_get_mouse_y() - d->y >= 0 && t3gui_get_mouse_y() - d->y < al_get_bitmap_height(_palette_bitmap))
				{
					if(c == 1)
					{
						*(ALLEGRO_COLOR *)d->dp = al_get_pixel(_palette_bitmap, t3gui_get_mouse_x() - d->x, t3gui_get_mouse_y() - d->y);
					}
					else
					{
						*(ALLEGRO_COLOR *)d->dp2 = al_get_pixel(_palette_bitmap, t3gui_get_mouse_x() - d->x, t3gui_get_mouse_y() - d->y);
					}
				}
			}
			break;
		}
		case MSG_MOUSEUP:
		{
			d->flags &= ~D_TRACKMOUSE;
			mouse_button = 0;
			break;
		}
		case MSG_MOUSEMOVE:
		{
			if(mouse_button)
			{
				pa_gui_palette_proc(MSG_MOUSEDOWN, d, mouse_button);
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
