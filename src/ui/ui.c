#include "t3f/t3f.h"
#include "t3gui/t3gui.h"
#include "ui.h"
#include "window.h"
#include "menu/menu.h"
#include "menu/file.h"
#include "menu/file_proc.h"
#include "menu/edit_proc.h"
#include "element_proc/button_proc.h"
#include "modules/mouse_cursor.h"
#include "modules/canvas/canvas.h"
#include "modules/canvas/canvas_file.h"
#include "modules/canvas/canvas_helpers.h"
#include "modules/bitmap.h"
#include "element/color.h"
#include "element/palette.h"
#include "element/map.h"
#include "element/canvas_editor/element.h"
#include "modules/ega_palette.h"
#include "element/list.h"
#include "element_proc/layer_list_proc.h"
#include "element/brush.h"
#include "ui/canvas_editor/undo/undo.h"
#include "defines.h"
#include "dialog.h"
#include "dialog/color_popup_dialog.h"
#include "dialog/main_init.h"
#include "dialog/main.h"

static bool create_gui(PA_UI * uip, PA_CANVAS_EDITOR * cep)
{
	uip->main_dialog = pa_create_main_dialog(cep);
	if(!uip->main_dialog)
	{
		return false;
	}

	return true;
}

PA_UI * pa_create_ui(PA_CANVAS_EDITOR * cep)
{
	PA_UI * uip;

	uip = malloc(sizeof(PA_UI));
	if(uip)
	{
		memset(uip, 0, sizeof(PA_UI));

		if(!create_gui(uip, cep))
		{
			goto fail;
		}

		if(!pa_setup_menus(uip))
		{
			goto fail;
		}
		pa_update_undo_name(cep);
		pa_update_redo_name(cep);
		t3f_attach_menu(uip->menu[PA_UI_MENU_MAIN]);

		pa_set_color(&cep->left_color, al_map_rgba_f(1.0, 0.0, 0.0, 1.0));
		pa_set_color(&cep->right_color, al_map_rgba_f(0.0, 0.0, 0.0, 1.0));
		pa_canvas_editor_update_pick_colors(cep);
		cep->box_line_thickness = pa_get_theme_int(uip->main_dialog->theme, "box_line_thickness", 2);
		cep->grid_thickness = pa_get_theme_float(uip->main_dialog->theme, "grid_thickness", 0.0);
		cep->checkerboard_scale = pa_get_theme_int(uip->main_dialog->theme, "checkerboard_scale", 24);
		cep->grid_offset = pa_get_theme_float(uip->main_dialog->theme, "grid_offset", 0.5);
		cep->grid_color = pa_get_theme_color(uip->main_dialog->theme, "grid_color", al_map_rgba_f(0.0, 0.0, 0.0, 0.25));
	}
	return uip;

	fail:
	{
		pa_destroy_ui(uip);
		return NULL;
	}
}

void pa_destroy_ui(PA_UI * uip)
{
	pa_destroy_menus(uip);
	pa_close_dialog(uip->main_dialog);
	free(uip);
}

void pa_process_ui(PA_UI * uip)
{
	PA_CANVAS_EDITOR * cep = (PA_CANVAS_EDITOR *)pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_CANVAS_EDITOR)->dp;
	ALLEGRO_STATE old_state;

	if(uip->main_dialog)
	{
		pa_main_dialog_pre_logic(uip->main_dialog, cep);
	}
	if(uip->color_popup_dialog)
	{
		pa_color_dialog_pre_logic(uip->color_popup_dialog);
	}
	t3gui_logic();
	if(uip->main_dialog)
	{
		pa_main_dialog_post_logic(uip->main_dialog, cep);
	}
	if(pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_BRUSH)->id1)
	{
		uip->brush_popup_dialog = pa_create_dialog(NULL, al_get_config_value(t3f_config, "App Data", "theme"), 640, 480, NULL);
		t3gui_show_dialog(uip->brush_popup_dialog->dialog, t3f_queue, T3GUI_PLAYER_CLEAR, NULL);
		pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_BRUSH)->id1 = 0;
	}
	if(pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_LEFT_COLOR)->id1)
	{
		uip->color_popup_dialog = pa_create_color_editor_popup_dialog(&cep->left_color.color, uip->main_dialog->theme);
		al_store_state(&old_state, ALLEGRO_STATE_TARGET_BITMAP);
//		al_set_target_bitmap(al_get_backbuffer(uip->color_popup_dialog->display));
		t3gui_show_dialog(uip->color_popup_dialog->dialog, t3f_queue, 0, NULL);
		al_restore_state(&old_state);
		pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_LEFT_COLOR)->id1 = 0;
	}
	if(pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_RIGHT_COLOR)->id1)
	{
		uip->color_popup_dialog = pa_create_color_editor_popup_dialog(&cep->right_color.color, uip->main_dialog->theme);
		al_store_state(&old_state, ALLEGRO_STATE_TARGET_BITMAP);
//		al_set_target_bitmap(al_get_backbuffer(uip->color_popup_dialog->display));
		t3gui_show_dialog(uip->color_popup_dialog->dialog, t3f_queue, T3GUI_PLAYER_CLEAR, NULL);
		al_restore_state(&old_state);
		pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_RIGHT_COLOR)->id1 = 0;
	}
	pa_update_window_title(cep);
	if(uip->color_popup_dialog)
	{
		if(t3gui_get_active_dialogs() > 1)
		{
			pa_color_dialog_post_logic(uip->color_popup_dialog);
		}
		else
		{
			pa_close_dialog(uip->color_popup_dialog);
			uip->color_popup_dialog = NULL;
		}
	}
}

void pa_render_ui(PA_UI * uip)
{
	PA_CANVAS_EDITOR * cep = (PA_CANVAS_EDITOR *)pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_CANVAS_EDITOR)->dp;
	t3gui_render();
	if(uip->brush_popup_dialog)
	{
		al_set_target_bitmap(al_get_backbuffer(uip->brush_popup_dialog->display));
		al_flip_display();
		al_set_target_bitmap(al_get_backbuffer(t3f_display));
	}
/*	if(uip->color_popup_dialog)
	{
		al_set_target_bitmap(al_get_backbuffer(uip->color_popup_dialog->display));
		al_flip_display();
		al_set_target_bitmap(al_get_backbuffer(t3f_display));
	} */
	al_use_shader(cep->standard_shader);
}
