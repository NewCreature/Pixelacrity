#include "t3f/t3f.h"
#include "t3gui/t3gui.h"
#include "ui/canvas_editor/canvas_editor.h"

void pa_canvas_editor_MSG_LOSTMOUSE(T3GUI_ELEMENT * d, int c)
{
	PA_CANVAS_EDITOR * canvas_editor = (PA_CANVAS_EDITOR *)d->dp;

	if(canvas_editor->tool_state == PA_TOOL_STATE_OFF)
	{
		canvas_editor->hover_color = al_map_rgba_f(0.0, 0.0, 0.0, 0.0);
		d->flags &= ~D_TRACKMOUSE;
	}
}
