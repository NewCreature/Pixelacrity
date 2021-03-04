#include "instance.h"
#include "t3gui/t3gui.h"
#include "canvas_editor.h"

int quixel_tool_pixel_button_proc(T3GUI_ELEMENT * d, void *dp3)
{
	APP_INSTANCE * app = (APP_INSTANCE *)dp3;

	app->canvas_editor->current_tool = QUIXEL_TOOL_PIXEL;

	return 0;
}

int quixel_tool_line_button_proc(T3GUI_ELEMENT * d, void *dp3)
{
	APP_INSTANCE * app = (APP_INSTANCE *)dp3;

	app->canvas_editor->current_tool = QUIXEL_TOOL_LINE;

	return 0;
}

int quixel_tool_rectangle_button_proc(T3GUI_ELEMENT * d, void *dp3)
{
	APP_INSTANCE * app = (APP_INSTANCE *)dp3;

	app->canvas_editor->current_tool = QUIXEL_TOOL_RECTANGLE;

	return 0;
}

int quixel_tool_filled_rectangle_button_proc(T3GUI_ELEMENT * d, void *dp3)
{
	APP_INSTANCE * app = (APP_INSTANCE *)dp3;

	app->canvas_editor->current_tool = QUIXEL_TOOL_FILLED_RECTANGLE;

	return 0;
}

int quixel_tool_oval_button_proc(T3GUI_ELEMENT * d, void *dp3)
{
	APP_INSTANCE * app = (APP_INSTANCE *)dp3;

	app->canvas_editor->current_tool = QUIXEL_TOOL_OVAL;

	return 0;
}

int quixel_tool_dropper_button_proc(T3GUI_ELEMENT * d, void *dp3)
{
	APP_INSTANCE * app = (APP_INSTANCE *)dp3;

	app->canvas_editor->current_tool = QUIXEL_TOOL_DROPPER;

	return 0;
}
