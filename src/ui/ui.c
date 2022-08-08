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
#include "modules/pixel_shader.h"
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
#include "modules/color.h"

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

static void clear_color_highlights(PA_UI * uip)
{
	int i;

	for(i = 0; i < uip->main_dialog->dialog->elements; i++)
	{
		if(uip->main_dialog->dialog->element[i].proc == pa_gui_color_proc)
		{
			uip->main_dialog->dialog->element[i].flags &= ~D_SELECTED;
		}
	}
}

static void handle_color_drag_and_drop(PA_UI * uip)
{
	int color_size = pa_get_theme_int(uip->main_dialog->theme, "color_size", 12);
	int line_thickness = pa_get_theme_int(uip->main_dialog->theme, "box_line_thickness", 2);

	color_size -= line_thickness * 2;
	uip->floating_ep = pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_FLOATING_COLOR);
	uip->click_ep = t3gui_get_click_element();
	uip->hover_ep = t3gui_get_hover_element(pa_gui_color_proc);
	clear_color_highlights(uip);
	if(uip->click_ep && uip->click_ep->proc == pa_gui_color_proc && (uip->click_ep->flags & D_TRACKMOUSE) && uip->click_ep->d2 == 1 && uip->click_ep->flags & D_USER)
	{
		uip->floating_ep->dp = uip->click_ep->dp;
		uip->floating_ep->x = t3gui_get_mouse_x() - color_size / 2;
		uip->floating_ep->y = t3gui_get_mouse_y() - color_size / 2;
		uip->floating_ep->w = color_size;
		uip->floating_ep->h = color_size;
		uip->floating_ep->flags &= ~D_DISABLED;
		if(uip->hover_ep && uip->hover_ep->d1)
		{
			uip->hover_ep->flags |= D_SELECTED;
		}
	}
	else
	{
		/* if we weren't disabled, that means we were holding a color, see if we
		   need to drop or swap the color */
		if(!(uip->floating_ep->flags & D_DISABLED))
		{
			if(uip->hover_ep && uip->hover_ep->proc == pa_gui_color_proc)
			{
				if(uip->hover_ep->dp && uip->hover_ep->d1 && uip->floating_ep->dp)
				{
					pa_drop_or_swap_gui_color_data((PA_GUI_COLOR_DATA *)uip->floating_ep->dp, (PA_GUI_COLOR_DATA *)uip->hover_ep->dp);
				}
			}

			/* disable the floating element */
			uip->floating_ep->dp = NULL;
			uip->floating_ep->flags |= D_DISABLED;
		}
	}
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
	handle_color_drag_and_drop(uip);
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
		cep->last_color = cep->left_color.color;
		uip->color_popup_dialog = pa_create_color_editor_popup_dialog(&cep->left_color.color, pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_LEFT_COLOR)->x - pa_get_theme_int(uip->main_dialog->theme, "edge_left_space", 8), pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_LEFT_COLOR)->y - pa_get_theme_int(uip->main_dialog->theme, "edge_right_space", 8));
		al_store_state(&old_state, ALLEGRO_STATE_TARGET_BITMAP);
		al_set_target_bitmap(al_get_backbuffer(uip->color_popup_dialog->display));
		t3gui_show_dialog(uip->color_popup_dialog->dialog, t3f_queue, T3GUI_PLAYER_NO_ESCAPE, uip);
		al_restore_state(&old_state);
		pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_LEFT_COLOR)->id1 = 0;
	}
	if(pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_RIGHT_COLOR)->id1)
	{
		cep->last_color = cep->right_color.color;
		uip->color_popup_dialog = pa_create_color_editor_popup_dialog(&cep->right_color.color, pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_RIGHT_COLOR)->x - pa_get_theme_int(uip->main_dialog->theme, "edge_left_space", 8), pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_RIGHT_COLOR)->y - pa_get_theme_int(uip->main_dialog->theme, "edge_right_space", 8));
		al_store_state(&old_state, ALLEGRO_STATE_TARGET_BITMAP);
		al_set_target_bitmap(al_get_backbuffer(uip->color_popup_dialog->display));
		t3gui_show_dialog(uip->color_popup_dialog->dialog, t3f_queue, T3GUI_PLAYER_NO_ESCAPE, uip);
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

static void render_color_drag_and_drop_helpers(PA_UI * uip)
{
	int line_thickness = pa_get_theme_int(uip->main_dialog->theme, "box_line_thickness", 2);

	if(uip->click_ep && uip->hover_ep && uip->click_ep != uip->hover_ep && uip->hover_ep != uip->floating_ep && uip->click_ep->flags & D_TRACKMOUSE && uip->hover_ep->d1 && uip->click_ep->d2 == 1)
	{
		al_draw_line(uip->click_ep->x + uip->click_ep->w / 2 + 0.5, uip->click_ep->y + uip->click_ep->h / 2 + 0.5, uip->hover_ep->x + uip->hover_ep->w / 2 + 0.5, uip->hover_ep->y + uip->hover_ep->h / 2 + 0.5, t3f_color_white, line_thickness);
	}
}

void pa_render_ui(PA_UI * uip)
{
	PA_CANVAS_EDITOR * cep = (PA_CANVAS_EDITOR *)pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_CANVAS_EDITOR)->dp;
	t3gui_render(t3f_display);
	render_color_drag_and_drop_helpers(uip);
	if(uip->brush_popup_dialog)
	{
		al_set_target_bitmap(al_get_backbuffer(uip->brush_popup_dialog->display));
		t3gui_render(uip->brush_popup_dialog->display);
		al_flip_display();
		al_set_target_bitmap(al_get_backbuffer(t3f_display));
	}
	al_use_shader(cep->standard_shader);
}
