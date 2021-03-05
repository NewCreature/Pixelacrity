#include "instance.h"
#include "t3gui/t3gui.h"
#include "canvas_editor.h"
#include "canvas_helpers.h"
#include "primitives.h"

static void unfloat_selection(QUIXEL_CANVAS_EDITOR * cep)
{
	quixel_draw_primitive_to_canvas(cep->canvas, cep->current_layer, cep->selection.x, cep->selection.y, cep->selection.x + cep->selection.width, cep->selection.y + cep->selection.height, cep->scratch_bitmap, al_map_rgba_f(0, 0, 0, 0), quixel_draw_quad);
	cep->selection.width = 0;
	cep->selection.height = 0;
}

static void change_tool(QUIXEL_CANVAS_EDITOR * cep, int tool)
{
	if(cep->current_tool == QUIXEL_TOOL_SELECTION && tool != QUIXEL_TOOL_SELECTION)
	{
		unfloat_selection(cep);
	}
	cep->current_tool = tool;
}

int quixel_tool_pixel_button_proc(T3GUI_ELEMENT * d, void *dp3)
{
	APP_INSTANCE * app = (APP_INSTANCE *)dp3;

	change_tool(app->canvas_editor, QUIXEL_TOOL_PIXEL);

	return 0;
}

int quixel_tool_line_button_proc(T3GUI_ELEMENT * d, void *dp3)
{
	APP_INSTANCE * app = (APP_INSTANCE *)dp3;

	change_tool(app->canvas_editor, QUIXEL_TOOL_LINE);

	return 0;
}

int quixel_tool_rectangle_button_proc(T3GUI_ELEMENT * d, void *dp3)
{
	APP_INSTANCE * app = (APP_INSTANCE *)dp3;

	change_tool(app->canvas_editor, QUIXEL_TOOL_RECTANGLE);

	return 0;
}

int quixel_tool_filled_rectangle_button_proc(T3GUI_ELEMENT * d, void *dp3)
{
	APP_INSTANCE * app = (APP_INSTANCE *)dp3;

	change_tool(app->canvas_editor, QUIXEL_TOOL_FILLED_RECTANGLE);

	return 0;
}

int quixel_tool_oval_button_proc(T3GUI_ELEMENT * d, void *dp3)
{
	APP_INSTANCE * app = (APP_INSTANCE *)dp3;

	change_tool(app->canvas_editor, QUIXEL_TOOL_OVAL);

	return 0;
}

int quixel_tool_filled_oval_button_proc(T3GUI_ELEMENT * d, void *dp3)
{
	APP_INSTANCE * app = (APP_INSTANCE *)dp3;

	change_tool(app->canvas_editor, QUIXEL_TOOL_FILLED_OVAL);

	return 0;
}

int quixel_tool_dropper_button_proc(T3GUI_ELEMENT * d, void *dp3)
{
	APP_INSTANCE * app = (APP_INSTANCE *)dp3;

	change_tool(app->canvas_editor, QUIXEL_TOOL_DROPPER);

	return 0;
}

int quixel_tool_selection_button_proc(T3GUI_ELEMENT * d, void *dp3)
{
	APP_INSTANCE * app = (APP_INSTANCE *)dp3;

	change_tool(app->canvas_editor, QUIXEL_TOOL_SELECTION);

	return 0;
}
