#include "instance.h"
#include "t3gui/t3gui.h"
#include "ui/canvas_editor/canvas_editor.h"
#include "modules/canvas/canvas_helpers.h"
#include "modules/primitives.h"
#include "ui/menu/menu_layer_proc.h"

int quixel_tool_pixel_button_proc(T3GUI_ELEMENT * d, void *dp3)
{
	APP_INSTANCE * app = (APP_INSTANCE *)dp3;

	quixel_select_canvas_editor_tool(app->canvas_editor, QUIXEL_TOOL_PIXEL);

	return 0;
}

int quixel_tool_line_button_proc(T3GUI_ELEMENT * d, void *dp3)
{
	APP_INSTANCE * app = (APP_INSTANCE *)dp3;

	quixel_select_canvas_editor_tool(app->canvas_editor, QUIXEL_TOOL_LINE);

	return 0;
}

int quixel_tool_rectangle_button_proc(T3GUI_ELEMENT * d, void *dp3)
{
	APP_INSTANCE * app = (APP_INSTANCE *)dp3;

	quixel_select_canvas_editor_tool(app->canvas_editor, QUIXEL_TOOL_RECTANGLE);

	return 0;
}

int quixel_tool_filled_rectangle_button_proc(T3GUI_ELEMENT * d, void *dp3)
{
	APP_INSTANCE * app = (APP_INSTANCE *)dp3;

	quixel_select_canvas_editor_tool(app->canvas_editor, QUIXEL_TOOL_FILLED_RECTANGLE);

	return 0;
}

int quixel_tool_oval_button_proc(T3GUI_ELEMENT * d, void *dp3)
{
	APP_INSTANCE * app = (APP_INSTANCE *)dp3;

	quixel_select_canvas_editor_tool(app->canvas_editor, QUIXEL_TOOL_OVAL);

	return 0;
}

int quixel_tool_filled_oval_button_proc(T3GUI_ELEMENT * d, void *dp3)
{
	APP_INSTANCE * app = (APP_INSTANCE *)dp3;

	quixel_select_canvas_editor_tool(app->canvas_editor, QUIXEL_TOOL_FILLED_OVAL);

	return 0;
}

int quixel_tool_flood_fill_button_proc(T3GUI_ELEMENT * d, void *dp3)
{
	APP_INSTANCE * app = (APP_INSTANCE *)dp3;

	quixel_select_canvas_editor_tool(app->canvas_editor, QUIXEL_TOOL_FLOOD_FILL);

	return 0;
}

int quixel_tool_dropper_button_proc(T3GUI_ELEMENT * d, void *dp3)
{
	APP_INSTANCE * app = (APP_INSTANCE *)dp3;

	quixel_select_canvas_editor_tool(app->canvas_editor, QUIXEL_TOOL_DROPPER);

	return 0;
}

int quixel_tool_selection_button_proc(T3GUI_ELEMENT * d, void *dp3)
{
	APP_INSTANCE * app = (APP_INSTANCE *)dp3;

	quixel_select_canvas_editor_tool(app->canvas_editor, QUIXEL_TOOL_SELECTION);

	return 0;
}

int quixel_layer_add_button_proc(T3GUI_ELEMENT * d, void * dp3)
{
	quixel_menu_layer_add(0, dp3);

	return 0;
}

int quixel_layer_remove_button_proc(T3GUI_ELEMENT * d, void * dp3)
{
	quixel_menu_layer_delete(0, dp3);

	return 0;
}
