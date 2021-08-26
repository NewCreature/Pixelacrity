#include "t3f/t3f.h"
#include "t3gui/t3gui.h"
#include "modules/canvas/canvas_helpers.h"
#include "ui/canvas_editor/canvas_editor.h"

static void render_map(PA_CANVAS_EDITOR * cep, int x, int y, int width, int height)
{
	int cx, cy, cwidth, cheight;
	float scale;
	float scale2;
	int left, top, right, bottom;
	int offset_x, offset_y;

	pa_get_canvas_dimensions(cep->canvas, &cx, &cy, &cwidth, &cheight, 0, false);
	if(cwidth > 0 && cheight > 0)
	{
		scale = (float)width / (float)cwidth;
		scale2 = (float)height / (float)cheight;
		if(scale > scale2)
		{
			scale = scale2;
		}
		left = width / 2 - (int)((float)cwidth * scale) / 2;
		top = height / 2 - (int)((float)cheight * scale) / 2;
		right = width / 2 + (int)((float)cwidth * scale) / 2 - 1;
		bottom = height / 2 + (int)((float)cheight * scale) / 2 - 1;
		al_clear_to_color(al_map_rgba_f(0.5, 0.5, 0.5, 1.0));
		al_draw_filled_rectangle(x + left, y + top, x + right, y + bottom, al_map_rgba_f(0.75, 0.75, 0.75, 1.0));
		offset_x = left;
		offset_y = top;
		left = (float)(cep->view_x - cx) * scale + offset_x;
		top = (float)(cep->view_y - cy) * scale + offset_y;
		right = left + (float)cep->view_width * scale;
		bottom = top + (float)cep->view_height * scale;
		al_draw_filled_rectangle(x + left, y + top, x + right, y + bottom, al_map_rgba_f(1.0, 1.0, 1.0, 0.5));
	}
}

int pa_gui_map_proc(int msg, T3GUI_ELEMENT * d, int c)
{
	PA_CANVAS_EDITOR * cep = (PA_CANVAS_EDITOR *)d->dp;
	switch(msg)
	{
		case MSG_DRAW:
		{
			render_map(cep, d->x, d->y, d->w, d->h);
			break;
		}
	}
	return D_O_K;
}
