#include "t3f/t3f.h"
#include "t3gui/t3gui.h"
#include "modules/canvas/canvas_helpers.h"
#include "ui/canvas_editor/canvas_editor.h"
#include "ui/canvas_editor/preview.h"

static void render_map(PA_CANVAS_EDITOR * cep, int x, int y, int width, int height)
{
	int cx, cy, cwidth, cheight;
	float scale;
	float scale2;
	int left, top, right, bottom;
	int offset_x, offset_y;
	int i;

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
		for(i = 0; i < cep->canvas->frame_max; i++)
		{
			left = (float)(cep->canvas->frame[i]->box.start_x - cx) * scale + offset_x;
			top = (float)(cep->canvas->frame[i]->box.start_y - cy) * scale + offset_y;
			right = left + (float)cep->canvas->frame[i]->box.width * scale;
			bottom = top + (float)cep->canvas->frame[i]->box.height * scale;
			al_draw_rectangle(x + left, y + top, x + right, y + bottom, t3f_color_black, 1.0);
		}
	}
}

int pa_gui_map_proc(int msg, T3GUI_ELEMENT * d, int c)
{
	PA_CANVAS_EDITOR * cep = (PA_CANVAS_EDITOR *)d->dp;
	int cx, cy, cwidth, cheight;
	float scale, scale2;
	int left, top;

	switch(msg)
	{
		case MSG_MOUSEUP:
		{
			d->flags &= ~D_TRACKMOUSE;
			break;
		}
		case MSG_MOUSEDOWN:
		{
			d->flags |= D_TRACKMOUSE;
		}
		case MSG_MOUSEMOVE:
		{
			if(d->flags & D_TRACKMOUSE)
			{
				pa_get_canvas_dimensions(cep->canvas, &cx, &cy, &cwidth, &cheight, 0, false);
				if(cwidth > 0 && cheight > 0)
				{
					scale = (float)d->w / (float)cwidth;
					scale2 = (float)d->h / (float)cheight;
					left = d->w / 2 - (int)((float)cwidth * scale) / 2;
					top = d->h / 2 - (int)((float)cheight * scale) / 2;
					if(scale > scale2)
					{
						scale = scale2;
					}
					pa_center_canvas_editor_at(cep, (t3gui_get_mouse_x() - left - d->x) / scale + cx, (t3gui_get_mouse_y() - top - d->y) / scale + cy);
				}
			}
			break;
		}
		case MSG_DRAW:
		{
			if(cep->preview->bitmap)
			{
				al_draw_bitmap(cep->preview->bitmap, d->x, d->y, 0);
			}
//			render_map(cep, d->x, d->y, d->w, d->h);
			break;
		}
		case MSG_IDLE:
		{
			if(cep->preview->update)
			{
				pa_update_canvas_editor_preview(cep->preview, cep->canvas, cep->premultiplied_alpha_shader);
				cep->preview->update = false;
			}
			break;
		}
	}
	return D_O_K;
}
