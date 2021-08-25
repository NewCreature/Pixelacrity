#include "t3f/t3f.h"
#include "t3gui/t3gui.h"
#include "modules/canvas/canvas_helpers.h"
#include "ui/canvas_editor/canvas_editor.h"

void pa_canvas_editor_MSG_WHEEL(T3GUI_ELEMENT * d, int c)
{
	PA_CANVAS_EDITOR * canvas_editor = (PA_CANVAS_EDITOR *)d->dp;

	pa_update_mouse_variables(canvas_editor);

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
