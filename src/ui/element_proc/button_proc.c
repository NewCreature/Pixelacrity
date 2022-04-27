#include "instance.h"
#include "t3gui/t3gui.h"
#include "ui/canvas_editor/canvas_editor.h"
#include "modules/canvas/canvas_helpers.h"
#include "modules/primitives.h"
#include "ui/menu/layer_proc.h"
#include "ui/menu/file_proc.h"
#include "ui/menu/edit_proc.h"

int pa_toolbar_new_button_proc(T3GUI_ELEMENT * d, void * dp3)
{
	pa_menu_file_new(0, dp3);
	return 0;
}

int pa_toolbar_open_button_proc(T3GUI_ELEMENT * d, void * dp3)
{
	pa_menu_file_load(0, dp3);
	return 0;
}

int pa_toolbar_save_button_proc(T3GUI_ELEMENT * d, void * dp3)
{
	pa_menu_file_save(0, dp3);
	return 0;
}

int pa_toolbar_import_button_proc(T3GUI_ELEMENT * d, void * dp3)
{
	pa_menu_file_import(0, dp3);
	return 0;
}

int pa_toolbar_export_button_proc(T3GUI_ELEMENT * d, void * dp3)
{
	pa_menu_file_export(0, dp3);
	return 0;
}

int pa_toolbar_undo_button_proc(T3GUI_ELEMENT * d, void * dp3)
{
	pa_menu_edit_undo(0, dp3);
	return 0;
}

int pa_toolbar_redo_button_proc(T3GUI_ELEMENT * d, void * dp3)
{
	pa_menu_edit_redo(0, dp3);
	return 0;
}

int pa_toolbar_cut_button_proc(T3GUI_ELEMENT * d, void * dp3)
{
	pa_menu_edit_cut(0, dp3);
	return 0;
}

int pa_toolbar_copy_button_proc(T3GUI_ELEMENT * d, void * dp3)
{
	pa_menu_edit_copy(0, dp3);
	return 0;
}

int pa_toolbar_paste_button_proc(T3GUI_ELEMENT * d, void * dp3)
{
	pa_menu_edit_paste(0, dp3);
	return 0;
}

int pa_toolbar_turn_cw_button_proc(T3GUI_ELEMENT * d, void * dp3)
{
	pa_menu_edit_turn_clockwise(0, dp3);
	return 0;
}

int pa_toolbar_turn_ccw_button_proc(T3GUI_ELEMENT * d, void * dp3)
{
	pa_menu_edit_turn_counter_clockwise(0, dp3);
	return 0;
}

int pa_toolbar_flip_h_button_proc(T3GUI_ELEMENT * d, void * dp3)
{
	pa_menu_edit_flip_horizontal(0, dp3);
	return 0;
}

int pa_toolbar_flip_v_button_proc(T3GUI_ELEMENT * d, void * dp3)
{
	pa_menu_edit_flip_vertical(0, dp3);
	return 0;
}

int pa_tool_composite_button_proc(T3GUI_ELEMENT * d, void * dp3)
{
	APP_INSTANCE * app = (APP_INSTANCE *)dp3;

	return 0;
}

int pa_tool_outline_button_proc(T3GUI_ELEMENT * d, void * dp3)
{
	APP_INSTANCE * app = (APP_INSTANCE *)dp3;

	app->canvas_editor->tool_outline = !app->canvas_editor->tool_outline;

	return 0;
}

int pa_tool_pixel_button_proc(T3GUI_ELEMENT * d, void * dp3)
{
	APP_INSTANCE * app = (APP_INSTANCE *)dp3;

	t3f_debug_message("Enter pa_tool_pixel_button_proc()\n");
	pa_select_canvas_editor_tool(app->canvas_editor, PA_TOOL_PIXEL);
	t3f_debug_message("Exit pa_tool_pixel_button_proc()\n");

	return 0;
}

int pa_tool_line_button_proc(T3GUI_ELEMENT * d, void * dp3)
{
	APP_INSTANCE * app = (APP_INSTANCE *)dp3;

	t3f_debug_message("Enter pa_tool_line_button_proc()\n");
	pa_select_canvas_editor_tool(app->canvas_editor, PA_TOOL_LINE);
	t3f_debug_message("Exit pa_tool_line_button_proc()\n");

	return 0;
}

int pa_tool_rectangle_button_proc(T3GUI_ELEMENT * d, void * dp3)
{
	APP_INSTANCE * app = (APP_INSTANCE *)dp3;

	t3f_debug_message("Enter pa_tool_rectangle_button_proc()\n");
	pa_select_canvas_editor_tool(app->canvas_editor, PA_TOOL_RECTANGLE);
	t3f_debug_message("Exit pa_tool_rectangle_button_proc()\n");

	return 0;
}

int pa_tool_filled_rectangle_button_proc(T3GUI_ELEMENT * d, void * dp3)
{
	APP_INSTANCE * app = (APP_INSTANCE *)dp3;

	t3f_debug_message("Enter pa_tool_filled_rectangle_button_proc()\n");
	pa_select_canvas_editor_tool(app->canvas_editor, PA_TOOL_FILLED_RECTANGLE);
	t3f_debug_message("Exit pa_tool_filled_rectangle_button_proc()\n");

	return 0;
}

int pa_tool_oval_button_proc(T3GUI_ELEMENT * d, void * dp3)
{
	APP_INSTANCE * app = (APP_INSTANCE *)dp3;

	t3f_debug_message("Enter pa_tool_oval_button_proc()\n");
	pa_select_canvas_editor_tool(app->canvas_editor, PA_TOOL_OVAL);
	t3f_debug_message("Exit pa_tool_oval_button_proc()\n");

	return 0;
}

int pa_tool_filled_oval_button_proc(T3GUI_ELEMENT * d, void * dp3)
{
	APP_INSTANCE * app = (APP_INSTANCE *)dp3;

	t3f_debug_message("Enter pa_tool_filled_oval_button_proc()\n");
	pa_select_canvas_editor_tool(app->canvas_editor, PA_TOOL_FILLED_OVAL);
	t3f_debug_message("Exit pa_tool_filled_oval_button_proc()\n");

	return 0;
}

int pa_tool_flood_fill_button_proc(T3GUI_ELEMENT * d, void * dp3)
{
	APP_INSTANCE * app = (APP_INSTANCE *)dp3;

	t3f_debug_message("Enter pa_tool_flood_fill_button_proc()\n");
	pa_select_canvas_editor_tool(app->canvas_editor, PA_TOOL_FLOOD_FILL);
	t3f_debug_message("Exit pa_tool_flood_fill_button_proc()\n");

	return 0;
}

int pa_tool_replace_button_proc(T3GUI_ELEMENT * d, void * dp3)
{
	APP_INSTANCE * app = (APP_INSTANCE *)dp3;

	t3f_debug_message("Enter pa_tool_replace_button_proc()\n");
	pa_select_canvas_editor_tool(app->canvas_editor, PA_TOOL_REPLACE);
	t3f_debug_message("Exit pa_tool_replace_button_proc()\n");

	return 0;
}

int pa_tool_dropper_button_proc(T3GUI_ELEMENT * d, void * dp3)
{
	APP_INSTANCE * app = (APP_INSTANCE *)dp3;

	t3f_debug_message("Enter pa_tool_dropper_button_proc()\n");
	pa_select_canvas_editor_tool(app->canvas_editor, PA_TOOL_DROPPER);
	t3f_debug_message("Exit pa_tool_dropper_button_proc()\n");

	return 0;
}

int pa_tool_eraser_button_proc(T3GUI_ELEMENT * d, void * dp3)
{
	APP_INSTANCE * app = (APP_INSTANCE *)dp3;

	t3f_debug_message("Enter pa_tool_eraser_button_proc()\n");
	pa_select_canvas_editor_tool(app->canvas_editor, PA_TOOL_ERASER);
	t3f_debug_message("Exit pa_tool_eraser_button_proc()\n");

	return 0;
}

int pa_tool_selection_button_proc(T3GUI_ELEMENT * d, void * dp3)
{
	APP_INSTANCE * app = (APP_INSTANCE *)dp3;

	t3f_debug_message("Enter pa_tool_selection_button_proc()\n");
	pa_select_canvas_editor_tool(app->canvas_editor, PA_TOOL_SELECTION);
	t3f_debug_message("Exit pa_tool_selection_button_proc()\n");

	return 0;
}

int pa_tool_frame_button_proc(T3GUI_ELEMENT * d, void * dp3)
{
	APP_INSTANCE * app = (APP_INSTANCE *)dp3;

	t3f_debug_message("Enter pa_tool_selection_button_proc()\n");
	pa_select_canvas_editor_tool(app->canvas_editor, PA_TOOL_FRAME);
	t3f_debug_message("Exit pa_tool_selection_button_proc()\n");

	return 0;
}

int pa_layer_add_button_proc(T3GUI_ELEMENT * d, void * dp3)
{
	pa_menu_layer_add(0, dp3);

	return 0;
}

int pa_layer_remove_button_proc(T3GUI_ELEMENT * d, void * dp3)
{
	pa_menu_layer_delete(0, dp3);

	return 0;
}