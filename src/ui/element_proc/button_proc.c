#include "instance.h"
#include "t3gui/t3gui.h"
#include "ui/canvas_editor/canvas_editor.h"
#include "modules/canvas/canvas_helpers.h"
#include "modules/primitives.h"
#include "ui/menu/layer_proc.h"
#include "ui/menu/file_proc.h"
#include "ui/menu/edit_proc.h"
#include "ui/element/color.h"
#include "ui/dialog/color_popup_dialog.h"
#include "modules/color.h"

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

int pa_color_okay_button_proc(T3GUI_ELEMENT * d, void * dp3)
{
	PA_UI * uip = (PA_UI *)dp3;
	PA_CANVAS_EDITOR * cep = (PA_CANVAS_EDITOR *)pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_CANVAS_EDITOR)->dp;
	PA_GUI_COLOR_DATA * color_data = (PA_GUI_COLOR_DATA *)(uip->color_popup_dialog->element[PA_COLOR_DIALOG_ELEMENT_COLOR]->dp);

	color_data->color_info->base_color = color_data->color_info->color;

	return 0;
}

int pa_color_cancel_button_proc(T3GUI_ELEMENT * d, void * dp3)
{
	PA_UI * uip = (PA_UI *)dp3;
	PA_CANVAS_EDITOR * cep = (PA_CANVAS_EDITOR *)pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_CANVAS_EDITOR)->dp;
	PA_GUI_COLOR_DATA * color_data = (PA_GUI_COLOR_DATA *)(uip->color_popup_dialog->element[PA_COLOR_DIALOG_ELEMENT_COLOR]->dp);

	color_data->color_info->color = cep->last_color;

	return 0;
}

int pa_color_r_up_button_proc(T3GUI_ELEMENT * d, void * dp3)
{
	PA_UI * uip = (PA_UI *)dp3;
	int i;

	i = atoi(uip->color_popup_dialog->edit_text[0]);
	i++;
	if(i > 255)
	{
		i = 255;
	}
	sprintf(uip->color_popup_dialog->edit_text[0], "%d", i);

	return 0;
}

int pa_color_r_down_button_proc(T3GUI_ELEMENT * d, void * dp3)
{
	PA_UI * uip = (PA_UI *)dp3;
	int i;

	i = atoi(uip->color_popup_dialog->edit_text[0]);
	i--;
	if(i < 0)
	{
		i = 0;
	}
	sprintf(uip->color_popup_dialog->edit_text[0], "%d", i);

	return 0;
}

int pa_color_g_up_button_proc(T3GUI_ELEMENT * d, void * dp3)
{
	PA_UI * uip = (PA_UI *)dp3;
	int i;

	i = atoi(uip->color_popup_dialog->edit_text[1]);
	i++;
	if(i > 255)
	{
		i = 255;
	}
	sprintf(uip->color_popup_dialog->edit_text[1], "%d", i);

	return 0;
}

int pa_color_g_down_button_proc(T3GUI_ELEMENT * d, void * dp3)
{
	PA_UI * uip = (PA_UI *)dp3;
	int i;

	i = atoi(uip->color_popup_dialog->edit_text[1]);
	i--;
	if(i < 0)
	{
		i = 0;
	}
	sprintf(uip->color_popup_dialog->edit_text[1], "%d", i);

	return 0;
}

int pa_color_b_up_button_proc(T3GUI_ELEMENT * d, void * dp3)
{
	PA_UI * uip = (PA_UI *)dp3;
	int i;

	i = atoi(uip->color_popup_dialog->edit_text[2]);
	i++;
	if(i > 255)
	{
		i = 255;
	}
	sprintf(uip->color_popup_dialog->edit_text[2], "%d", i);

	return 0;
}

int pa_color_b_down_button_proc(T3GUI_ELEMENT * d, void * dp3)
{
	PA_UI * uip = (PA_UI *)dp3;
	int i;

	i = atoi(uip->color_popup_dialog->edit_text[2]);
	i--;
	if(i < 0)
	{
		i = 0;
	}
	sprintf(uip->color_popup_dialog->edit_text[2], "%d", i);

	return 0;
}

int pa_color_a_up_button_proc(T3GUI_ELEMENT * d, void * dp3)
{
	PA_UI * uip = (PA_UI *)dp3;
	int i;

	i = atoi(uip->color_popup_dialog->edit_text[3]);
	i++;
	if(i > 255)
	{
		i = 255;
	}
	sprintf(uip->color_popup_dialog->edit_text[3], "%d", i);

	return 0;
}

int pa_color_a_down_button_proc(T3GUI_ELEMENT * d, void * dp3)
{
	PA_UI * uip = (PA_UI *)dp3;
	int i;

	i = atoi(uip->color_popup_dialog->edit_text[3]);
	i--;
	if(i < 0)
	{
		i = 0;
	}
	sprintf(uip->color_popup_dialog->edit_text[3], "%d", i);

	return 0;
}

int pa_brush_square_proc(T3GUI_ELEMENT * d, void * dp3)
{
	PA_UI * uip = (PA_UI *)dp3;
	PA_CANVAS_EDITOR * cep = (PA_CANVAS_EDITOR *)pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_CANVAS_EDITOR)->dp;
	ALLEGRO_STATE old_state;
	ALLEGRO_TRANSFORM identity;
	int brush_size = cep->brush_size > 0 ? cep->brush_size : 1;

	if(cep->brush)
	{
		al_destroy_bitmap(cep->brush);
	}
	cep->brush = al_create_bitmap(brush_size, brush_size);
	if(cep->brush)
	{
		al_store_state(&old_state, ALLEGRO_STATE_TARGET_BITMAP | ALLEGRO_STATE_TRANSFORM);
		al_identity_transform(&identity);
		al_set_target_bitmap(cep->brush);
		al_use_transform(&identity);
		al_clear_to_color(t3f_color_white);
		al_restore_state(&old_state);
	}

	return 0;
}

int pa_brush_circle_proc(T3GUI_ELEMENT * d, void * dp3)
{
	PA_UI * uip = (PA_UI *)dp3;
	PA_CANVAS_EDITOR * cep = (PA_CANVAS_EDITOR *)pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_CANVAS_EDITOR)->dp;
	ALLEGRO_STATE old_state;
	ALLEGRO_TRANSFORM identity;
	int brush_size = cep->brush_size > 2 ? cep->brush_size : 3;
	ALLEGRO_BITMAP * pixel;

	if(cep->brush)
	{
		al_destroy_bitmap(cep->brush);
	}
	cep->brush = al_create_bitmap(brush_size, brush_size);
	if(cep->brush)
	{
		al_store_state(&old_state, ALLEGRO_STATE_TARGET_BITMAP | ALLEGRO_STATE_TRANSFORM);
		al_identity_transform(&identity);
		al_set_target_bitmap(cep->brush);
		al_use_transform(&identity);
		al_clear_to_color(al_map_rgba_f(0.0, 0.0, 0.0, 0.0));
		pa_draw_filled_oval(0, 0, brush_size - 1, brush_size - 1, NULL, t3f_color_white, NULL);
		al_restore_state(&old_state);
	}

	return 0;
}

int pa_brush_vline_proc(T3GUI_ELEMENT * d, void * dp3)
{
	PA_UI * uip = (PA_UI *)dp3;
	PA_CANVAS_EDITOR * cep = (PA_CANVAS_EDITOR *)pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_CANVAS_EDITOR)->dp;
	ALLEGRO_STATE old_state;
	ALLEGRO_TRANSFORM identity;
	int brush_size = cep->brush_size > 2 ? cep->brush_size : 3;

	if(cep->brush)
	{
		al_destroy_bitmap(cep->brush);
	}
	cep->brush = al_create_bitmap(brush_size, brush_size);
	if(cep->brush)
	{
		al_store_state(&old_state, ALLEGRO_STATE_TARGET_BITMAP | ALLEGRO_STATE_TRANSFORM);
		al_identity_transform(&identity);
		al_set_target_bitmap(cep->brush);
		al_use_transform(&identity);
		al_clear_to_color(al_map_rgba_f(0.0, 0.0, 0.0, 0.0));
		pa_draw_line(brush_size / 2 + 0.5, 0, brush_size / 2 + 0.5, brush_size - 1 + 0.5, NULL, t3f_color_white, NULL);
		al_restore_state(&old_state);
	}

	return 0;
}

int pa_brush_dline_ur_proc(T3GUI_ELEMENT * d, void * dp3)
{
	PA_UI * uip = (PA_UI *)dp3;
	PA_CANVAS_EDITOR * cep = (PA_CANVAS_EDITOR *)pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_CANVAS_EDITOR)->dp;
	ALLEGRO_STATE old_state;
	ALLEGRO_TRANSFORM identity;
	int brush_size = cep->brush_size > 2 ? cep->brush_size : 3;

	if(cep->brush)
	{
		al_destroy_bitmap(cep->brush);
	}
	cep->brush = al_create_bitmap(brush_size, brush_size);
	if(cep->brush)
	{
		al_store_state(&old_state, ALLEGRO_STATE_TARGET_BITMAP | ALLEGRO_STATE_TRANSFORM);
		al_identity_transform(&identity);
		al_set_target_bitmap(cep->brush);
		al_use_transform(&identity);
		al_clear_to_color(al_map_rgba_f(0.0, 0.0, 0.0, 0.0));
		pa_draw_line(0 + 0.5, brush_size - 2 + 0.5, brush_size - 2 + 0.5, 0 + 0.5, NULL, t3f_color_white, NULL);
		pa_draw_line(0 + 0.5, brush_size - 1 + 0.5, brush_size - 1 + 0.5, 0 + 0.5, NULL, t3f_color_white, NULL);
		pa_draw_line(1 + 0.5, brush_size - 1 + 0.5, brush_size - 1 + 0.5, 1 + 0.5, NULL, t3f_color_white, NULL);
		al_restore_state(&old_state);
	}

	return 0;
}

int pa_brush_hline_proc(T3GUI_ELEMENT * d, void * dp3)
{
	PA_UI * uip = (PA_UI *)dp3;
	PA_CANVAS_EDITOR * cep = (PA_CANVAS_EDITOR *)pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_CANVAS_EDITOR)->dp;
	ALLEGRO_STATE old_state;
	ALLEGRO_TRANSFORM identity;
	int brush_size = cep->brush_size > 2 ? cep->brush_size : 3;

	if(cep->brush)
	{
		al_destroy_bitmap(cep->brush);
	}
	cep->brush = al_create_bitmap(brush_size, brush_size);
	if(cep->brush)
	{
		al_store_state(&old_state, ALLEGRO_STATE_TARGET_BITMAP | ALLEGRO_STATE_TRANSFORM);
		al_identity_transform(&identity);
		al_set_target_bitmap(cep->brush);
		al_use_transform(&identity);
		al_clear_to_color(al_map_rgba_f(0.0, 0.0, 0.0, 0.0));
		pa_draw_line(0 + 0.5, brush_size / 2 + 0.5, brush_size - 1 + 0.5, brush_size / 2 + 0.5, NULL, t3f_color_white, NULL);
		al_restore_state(&old_state);
	}

	return 0;
}

int pa_brush_dline_dr_proc(T3GUI_ELEMENT * d, void * dp3)
{
	PA_UI * uip = (PA_UI *)dp3;
	PA_CANVAS_EDITOR * cep = (PA_CANVAS_EDITOR *)pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_CANVAS_EDITOR)->dp;
	ALLEGRO_STATE old_state;
	ALLEGRO_TRANSFORM identity;
	int brush_size = cep->brush_size > 2 ? cep->brush_size : 3;

	if(cep->brush)
	{
		al_destroy_bitmap(cep->brush);
	}
	cep->brush = al_create_bitmap(brush_size, brush_size);
	if(cep->brush)
	{
		al_store_state(&old_state, ALLEGRO_STATE_TARGET_BITMAP | ALLEGRO_STATE_TRANSFORM);
		al_identity_transform(&identity);
		al_set_target_bitmap(cep->brush);
		al_use_transform(&identity);
		al_clear_to_color(al_map_rgba_f(0.0, 0.0, 0.0, 0.0));
		pa_draw_line(1 + 0.5, 0 + 0.5, brush_size - 1 + 0.5, brush_size - 2 + 0.5, NULL, t3f_color_white, NULL);
		pa_draw_line(0 + 0.5, 0 + 0.5, brush_size - 1 + 0.5, brush_size - 1 + 0.5, NULL, t3f_color_white, NULL);
		pa_draw_line(0 + 0.5, 1 + 0.5, brush_size - 2 + 0.5, brush_size - 1 + 0.5, NULL, t3f_color_white, NULL);
		al_restore_state(&old_state);
	}

	return 0;
}

int pa_brush_diamond_proc(T3GUI_ELEMENT * d, void * dp3)
{
	PA_UI * uip = (PA_UI *)dp3;
	PA_CANVAS_EDITOR * cep = (PA_CANVAS_EDITOR *)pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_CANVAS_EDITOR)->dp;
	ALLEGRO_STATE old_state;
	ALLEGRO_TRANSFORM identity;
	int brush_size = cep->brush_size > 2 ? cep->brush_size : 3;
	int i, ni;

	if(cep->brush)
	{
		al_destroy_bitmap(cep->brush);
	}
	cep->brush = al_create_bitmap(brush_size, brush_size);
	if(cep->brush)
	{
		al_store_state(&old_state, ALLEGRO_STATE_TARGET_BITMAP | ALLEGRO_STATE_TRANSFORM);
		al_identity_transform(&identity);
		al_set_target_bitmap(cep->brush);
		al_use_transform(&identity);
		al_clear_to_color(al_map_rgba_f(0.0, 0.0, 0.0, 0.0));
		for(i = 0; i < brush_size / 2; i++)
		{
			pa_draw_line(brush_size / 2 - i + 0.5, i + 0.5, brush_size / 2 + i + 0.5, i + 0.5, NULL, t3f_color_white, NULL);
		}
		for(i = brush_size / 2; i < brush_size; i++)
		{
			ni = i - brush_size / 2;
			pa_draw_line(ni + 0.5, i, brush_size - 1 - ni + 0.5, i, NULL, t3f_color_white, NULL);
		}
		al_restore_state(&old_state);
	}

	return 0;
}

int pa_brush_okay_button_proc(T3GUI_ELEMENT * d, void * dp3)
{
	PA_UI * uip = (PA_UI *)dp3;
	PA_CANVAS_EDITOR * cep = (PA_CANVAS_EDITOR *)pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_CANVAS_EDITOR)->dp;

	return 0;
}

int pa_brush_cancel_button_proc(T3GUI_ELEMENT * d, void * dp3)
{
	PA_UI * uip = (PA_UI *)dp3;
	PA_CANVAS_EDITOR * cep = (PA_CANVAS_EDITOR *)pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_CANVAS_EDITOR)->dp;

	return 0;
}
