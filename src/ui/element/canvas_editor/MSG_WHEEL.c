#include "t3f/t3f.h"
#include "t3gui/t3gui.h"
#include "modules/canvas/canvas_helpers.h"
#include "ui/canvas_editor/canvas_editor.h"

static int get_next_step(int step)
{
	int i = 1;
	int f;

	while(i <= step)
	{
		f = i / 4;
		i = i + f + 1;
	}
	return i;
}

static int get_previous_step(int step)
{
	int i = 1;
	int f;
	int previous_step = 1;

	while(i < step)
	{
		previous_step = i;
		f = i / 4;
		i = i + f + 1;
	}
	return previous_step;
}

void pa_canvas_editor_MSG_WHEEL(T3GUI_ELEMENT * d, int c)
{
	PA_CANVAS_EDITOR * canvas_editor = (PA_CANVAS_EDITOR *)d->dp;
	int new_zoom;

	pa_update_mouse_variables(canvas_editor);

	if(t3f_key[ALLEGRO_KEY_LSHIFT] || t3f_key[ALLEGRO_KEY_RSHIFT])
	{
		if(c < 0)
		{
			if(canvas_editor->current_layer > 0)
			{
				pa_select_canvas_editor_layer(canvas_editor, canvas_editor->current_layer - 1);
			}
		}
		else if(c > 0)
		{
			if(canvas_editor->current_layer < canvas_editor->canvas->layer_max - 1)
			{
				pa_select_canvas_editor_layer(canvas_editor, canvas_editor->current_layer + 1);
			}
		}
	}
	else
	{
		if(c < 0)
		{
			if(canvas_editor->view_zoom > 1)
			{
				if(t3f_key[ALLEGRO_KEY_LCTRL] || t3f_key[ALLEGRO_KEY_RCTRL] || t3f_key[ALLEGRO_KEY_COMMAND])
				{
					new_zoom = canvas_editor->view_zoom - 1;
				}
				else
				{
					new_zoom = get_previous_step(canvas_editor->view_zoom);
				}
				pa_set_canvas_editor_zoom(canvas_editor, new_zoom);
			}
		}
		else if(c > 0)
		{
			if(t3f_key[ALLEGRO_KEY_LCTRL] || t3f_key[ALLEGRO_KEY_RCTRL] || t3f_key[ALLEGRO_KEY_COMMAND])
			{
				new_zoom = canvas_editor->view_zoom + 1;
			}
			else
			{
				new_zoom = get_next_step(canvas_editor->view_zoom);
			}
			pa_set_canvas_editor_zoom(canvas_editor, new_zoom);
		}
	}
}
