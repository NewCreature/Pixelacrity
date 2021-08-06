#include "t3f/t3f.h"
#include "t3gui/t3gui.h"
#include "modules/canvas/canvas_helpers.h"
#include "ui/canvas_editor/canvas_editor.h"
#include "ui/canvas_editor/tools/tool_pixel.h"
#include "ui/canvas_editor/tools/tool_line.h"
#include "ui/canvas_editor/tools/tool_rectangle.h"
#include "ui/canvas_editor/tools/tool_filled_rectangle.h"
#include "ui/canvas_editor/tools/tool_oval.h"
#include "ui/canvas_editor/tools/tool_filled_oval.h"
#include "ui/canvas_editor/tools/tool_dropper.h"
#include "ui/canvas_editor/tools/tool_selection.h"
#include "ui/canvas_editor/selection.h"

void pa_canvas_editor_MSG_MOUSEMOVE(T3GUI_ELEMENT * d, int c)
{
	PA_CANVAS_EDITOR * canvas_editor = (PA_CANVAS_EDITOR *)d->dp;

	pa_update_tool_variables(canvas_editor);
	switch(canvas_editor->current_tool)
	{
		case PA_TOOL_PIXEL:
		case PA_TOOL_ERASER:
		{
			if(canvas_editor->tool_state == PA_TOOL_STATE_DRAWING || canvas_editor->tool_state == PA_TOOL_STATE_EDITING)
			{
				pa_tool_pixel_logic(canvas_editor);
			}
			break;
		}
		case PA_TOOL_DROPPER:
		{
			if(canvas_editor->tool_state == PA_TOOL_STATE_DRAWING || canvas_editor->tool_state == PA_TOOL_STATE_EDITING)
			{
				pa_tool_dropper_logic(canvas_editor, canvas_editor->click_button);
			}
			break;
		}
		case PA_TOOL_LINE:
		{
			if(canvas_editor->tool_state == PA_TOOL_STATE_DRAWING || canvas_editor->tool_state == PA_TOOL_STATE_EDITING)
			{
				pa_tool_line_logic(canvas_editor);
			}
			break;
		}
		case PA_TOOL_RECTANGLE:
		{
			if(canvas_editor->tool_state == PA_TOOL_STATE_DRAWING || canvas_editor->tool_state == PA_TOOL_STATE_EDITING)
			{
				pa_tool_rectangle_logic(canvas_editor);
			}
			break;
		}
		case PA_TOOL_FILLED_RECTANGLE:
		{
			if(canvas_editor->tool_state == PA_TOOL_STATE_DRAWING || canvas_editor->tool_state == PA_TOOL_STATE_EDITING)
			{
				pa_tool_filled_rectangle_logic(canvas_editor);
			}
			break;
		}
		case PA_TOOL_OVAL:
		{
			if(canvas_editor->tool_state == PA_TOOL_STATE_DRAWING || canvas_editor->tool_state == PA_TOOL_STATE_EDITING)
			{
				pa_tool_oval_logic(canvas_editor);
			}
			break;
		}
		case PA_TOOL_FILLED_OVAL:
		{
			if(canvas_editor->tool_state == PA_TOOL_STATE_DRAWING || canvas_editor->tool_state == PA_TOOL_STATE_EDITING)
			{
				pa_tool_filled_oval_logic(canvas_editor);
			}
			break;
		}
		case PA_TOOL_SELECTION:
		{
			pa_update_selection(canvas_editor, d);
			break;
		}
	}
}
