#include "t3f/t3f.h"
#include "t3gui/t3gui.h"
#include "modules/canvas/canvas_helpers.h"
#include "ui/canvas_editor/canvas_editor.h"

void pa_canvas_editor_MSG_WHEEL(T3GUI_ELEMENT * d, int c)
{
	PA_CANVAS_EDITOR * canvas_editor = (PA_CANVAS_EDITOR *)d->dp;

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
				pa_set_canvas_editor_zoom(canvas_editor, canvas_editor->view_zoom - 1);
			}
		}
		else if(c > 0)
		{
			pa_set_canvas_editor_zoom(canvas_editor, canvas_editor->view_zoom + 1);
		}
	}
}
