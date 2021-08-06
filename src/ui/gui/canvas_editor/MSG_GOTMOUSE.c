#include "t3f/t3f.h"
#include "t3gui/t3gui.h"
#include "modules/canvas/canvas_helpers.h"
#include "ui/canvas_editor/canvas_editor.h"

void pa_canvas_editor_MSG_GOTMOUSE(T3GUI_ELEMENT * d, int c)
{
	PA_CANVAS_EDITOR * canvas_editor = (PA_CANVAS_EDITOR *)d->dp;

	canvas_editor->hover_color = pa_get_canvas_pixel(canvas_editor->canvas, canvas_editor->current_layer, canvas_editor->hover_x, canvas_editor->hover_y);
	d->flags |= D_TRACKMOUSE;
}
