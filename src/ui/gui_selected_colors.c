#include "t3f/t3f.h"
#include "t3gui/t3gui.h"
#include "instance.h"
#include "gui_color_picker.h"

int quixel_gui_selected_colors_proc(int msg, T3GUI_ELEMENT * d, int c)
{
	APP_INSTANCE * app = (APP_INSTANCE *)d->user_data;
	ALLEGRO_COLOR color[2];
	int color_width;
	int i;

	color_width = d->w / 2;
	color[0] = app->canvas_editor->left_color;
	color[1] = app->canvas_editor->right_color;
	switch(msg)
	{
		case MSG_DRAW:
		{
			for(i = 0; i < 2; i++)
			{
				al_draw_filled_rectangle(d->x + i * color_width, d->y, d->x + i * color_width + color_width, d->y + d->h, color[i]);
			}
			break;
		}
	}
	return D_O_K;
}
