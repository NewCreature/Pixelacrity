#include "instance.h"
#include "t3gui/t3gui.h"
#include "ui/canvas_editor/canvas_editor.h"
#include "modules/canvas/canvas_helpers.h"
#include "modules/primitives.h"
#include "ui/menu/menu_layer_proc.h"

int quixel_tool_pixel_button_proc(T3GUI_ELEMENT * d, void *dp3)
{
	APP_INSTANCE * app = (APP_INSTANCE *)dp3;

	t3f_debug_message("Enter quixel_tool_pixel_button_proc()\n");
	quixel_select_canvas_editor_tool(app->canvas_editor, QUIXEL_TOOL_PIXEL);
	t3f_debug_message("Exit quixel_tool_pixel_button_proc()\n");

	return 0;
}

int quixel_tool_line_button_proc(T3GUI_ELEMENT * d, void *dp3)
{
	APP_INSTANCE * app = (APP_INSTANCE *)dp3;

	t3f_debug_message("Enter quixel_tool_line_button_proc()\n");
	quixel_select_canvas_editor_tool(app->canvas_editor, QUIXEL_TOOL_LINE);
	t3f_debug_message("Exit quixel_tool_line_button_proc()\n");

	return 0;
}

int quixel_tool_rectangle_button_proc(T3GUI_ELEMENT * d, void *dp3)
{
	APP_INSTANCE * app = (APP_INSTANCE *)dp3;

	t3f_debug_message("Enter quixel_tool_rectangle_button_proc()\n");
	quixel_select_canvas_editor_tool(app->canvas_editor, QUIXEL_TOOL_RECTANGLE);
	t3f_debug_message("Exit quixel_tool_rectangle_button_proc()\n");

	return 0;
}

int quixel_tool_filled_rectangle_button_proc(T3GUI_ELEMENT * d, void *dp3)
{
	APP_INSTANCE * app = (APP_INSTANCE *)dp3;

	t3f_debug_message("Enter quixel_tool_filled_rectangle_button_proc()\n");
	quixel_select_canvas_editor_tool(app->canvas_editor, QUIXEL_TOOL_FILLED_RECTANGLE);
	t3f_debug_message("Exit quixel_tool_filled_rectangle_button_proc()\n");

	return 0;
}

int quixel_tool_oval_button_proc(T3GUI_ELEMENT * d, void *dp3)
{
	APP_INSTANCE * app = (APP_INSTANCE *)dp3;

	t3f_debug_message("Enter quixel_tool_oval_button_proc()\n");
	quixel_select_canvas_editor_tool(app->canvas_editor, QUIXEL_TOOL_OVAL);
	t3f_debug_message("Exit quixel_tool_oval_button_proc()\n");

	return 0;
}

int quixel_tool_filled_oval_button_proc(T3GUI_ELEMENT * d, void *dp3)
{
	APP_INSTANCE * app = (APP_INSTANCE *)dp3;

	t3f_debug_message("Enter quixel_tool_filled_oval_button_proc()\n");
	quixel_select_canvas_editor_tool(app->canvas_editor, QUIXEL_TOOL_FILLED_OVAL);
	t3f_debug_message("Exit quixel_tool_filled_oval_button_proc()\n");

	return 0;
}

int quixel_tool_flood_fill_button_proc(T3GUI_ELEMENT * d, void *dp3)
{
	APP_INSTANCE * app = (APP_INSTANCE *)dp3;

	t3f_debug_message("Enter quixel_tool_flood_fill_button_proc()\n");
	quixel_select_canvas_editor_tool(app->canvas_editor, QUIXEL_TOOL_FLOOD_FILL);
	t3f_debug_message("Exit quixel_tool_flood_fill_button_proc()\n");

	return 0;
}

int quixel_tool_dropper_button_proc(T3GUI_ELEMENT * d, void *dp3)
{
	APP_INSTANCE * app = (APP_INSTANCE *)dp3;

	t3f_debug_message("Enter quixel_tool_dropper_button_proc()\n");
	quixel_select_canvas_editor_tool(app->canvas_editor, QUIXEL_TOOL_DROPPER);
	t3f_debug_message("Exit quixel_tool_dropper_button_proc()\n");

	return 0;
}

int quixel_tool_eraser_button_proc(T3GUI_ELEMENT * d, void * dp3)
{
	APP_INSTANCE * app = (APP_INSTANCE *)dp3;

	t3f_debug_message("Enter quixel_tool_eraser_button_proc()\n");
	quixel_select_canvas_editor_tool(app->canvas_editor, QUIXEL_TOOL_ERASER);
	t3f_debug_message("Exit quixel_tool_eraser_button_proc()\n");

	return 0;
}

int quixel_tool_selection_button_proc(T3GUI_ELEMENT * d, void *dp3)
{
	APP_INSTANCE * app = (APP_INSTANCE *)dp3;

	t3f_debug_message("Enter quixel_tool_selection_button_proc()\n");
	quixel_select_canvas_editor_tool(app->canvas_editor, QUIXEL_TOOL_SELECTION);
	t3f_debug_message("Exit quixel_tool_selection_button_proc()\n");

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
