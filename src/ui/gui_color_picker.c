#include "t3f/t3f.h"
#include "t3gui/t3gui.h"
#include "instance.h"

static ALLEGRO_COLOR pick_color[9];

int quixel_gui_color_picker_proc(int msg, T3GUI_ELEMENT * d, int c)
{
	APP_INSTANCE * app = (APP_INSTANCE *)d->user_data;
	int i;
	float h, s, l, new_l, step;
	float r, g, b;

	/* fill out color grid */
	step = 1.0 / 8.0;
	al_unmap_rgb_f(app->canvas_editor->base_color, &r, &g, &b);
	al_color_rgb_to_hsl(r, g, b, &h, &s, &l);
	for(i = 0; i < 9; i++)
	{
		new_l = step * (float)i;
		pick_color[i] = al_color_hsl(h, s, new_l);
	}

	switch(msg)
	{
		case MSG_MOUSEDOWN:
		{
			i = (t3f_mouse_x - d->x) / 8;
			if(i > 8)
			{
				i = 8;
			}
			app->canvas_editor->left_color = pick_color[i];
			break;
		}
		case MSG_DRAW:
		{
			for(i = 0; i < 9; i++)
			{
				al_draw_filled_rectangle(d->x + i * 8, d->y, d->x + i * 8 + 8, d->y + 8, pick_color[i]);
			}
			break;
		}
	}
	return D_O_K;
}
