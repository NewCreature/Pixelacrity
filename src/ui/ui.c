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

static void select_button(PA_UI * uip, int button)
{
	pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_BUTTON_PIXEL)->flags &= ~D_SELECTED;
	pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_BUTTON_LINE)->flags &= ~D_SELECTED;
	pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_BUTTON_RECTANGLE)->flags &= ~D_SELECTED;
	pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_BUTTON_FILLED_RECTANGLE)->flags &= ~D_SELECTED;
	pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_BUTTON_OVAL)->flags &= ~D_SELECTED;
	pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_BUTTON_FILLED_OVAL)->flags &= ~D_SELECTED;
	pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_BUTTON_ERASER)->flags &= ~D_SELECTED;
	pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_BUTTON_FLOOD_FILL)->flags &= ~D_SELECTED;
	pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_BUTTON_REPLACE)->flags &= ~D_SELECTED;
	pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_BUTTON_DROPPER)->flags &= ~D_SELECTED;
	pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_BUTTON_SELECTION)->flags &= ~D_SELECTED;
	pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_BUTTON_FRAME)->flags &= ~D_SELECTED;
	if(button >= 0)
	{
		pa_get_dialog_element(uip->main_dialog, button)->flags |= D_SELECTED;
	}
}

static void update_toolbar_flags(PA_UI * uip, PA_CANVAS_EDITOR * cep)
{
	if(!pa_can_export(cep))
	{
		pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_BUTTON_EXPORT)->flags |= D_DISABLED;
	}
	else
	{
		pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_BUTTON_EXPORT)->flags &= ~D_DISABLED;
	}
	if(!cep->undo_count)
	{
		pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_BUTTON_UNDO)->flags |= D_DISABLED;
		pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_BUTTON_UNDO)->flags &= ~(D_SELECTED | D_GOTFOCUS | D_GOTMOUSE);
	}
	else
	{
		pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_BUTTON_UNDO)->flags &= ~D_DISABLED;
	}
	if(!cep->redo_count)
	{
		pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_BUTTON_REDO)->flags |= D_DISABLED;
		pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_BUTTON_REDO)->flags &= ~(D_SELECTED | D_GOTFOCUS | D_GOTMOUSE);
	}
	else
	{
		pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_BUTTON_REDO)->flags &= ~D_DISABLED;
	}
	if(cep->selection.box.width > 0 && cep->selection.box.height > 0)
	{
		pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_BUTTON_CUT)->flags &= ~D_DISABLED;
		pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_BUTTON_COPY)->flags &= ~D_DISABLED;
		pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_BUTTON_TURN_CW)->flags &= ~D_DISABLED;
		pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_BUTTON_TURN_CCW)->flags &= ~D_DISABLED;
		pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_BUTTON_FLIP_HORIZONTAL)->flags &= ~D_DISABLED;
		pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_BUTTON_FLIP_VERTICAL)->flags &= ~D_DISABLED;
	}
	else
	{
		pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_BUTTON_CUT)->flags |= D_DISABLED;
		pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_BUTTON_CUT)->flags &= ~(D_SELECTED | D_GOTFOCUS | D_GOTMOUSE);
		pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_BUTTON_COPY)->flags |= D_DISABLED;
		pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_BUTTON_COPY)->flags &= ~(D_SELECTED | D_GOTFOCUS | D_GOTMOUSE);
		pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_BUTTON_TURN_CW)->flags |= D_DISABLED;
		pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_BUTTON_TURN_CW)->flags &= ~(D_SELECTED | D_GOTFOCUS | D_GOTMOUSE);
		pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_BUTTON_TURN_CCW)->flags |= D_DISABLED;
		pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_BUTTON_TURN_CCW)->flags &= ~(D_SELECTED | D_GOTFOCUS | D_GOTMOUSE);
		pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_BUTTON_FLIP_HORIZONTAL)->flags |= D_DISABLED;
		pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_BUTTON_FLIP_HORIZONTAL)->flags &= ~(D_SELECTED | D_GOTFOCUS | D_GOTMOUSE);
		pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_BUTTON_FLIP_VERTICAL)->flags |= D_DISABLED;
		pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_BUTTON_FLIP_VERTICAL)->flags &= ~(D_SELECTED | D_GOTFOCUS | D_GOTMOUSE);
	}
	if(cep->clipboard.bitmap_stack)
	{
		pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_BUTTON_PASTE)->flags &= ~D_DISABLED;
	}
	else
	{
		pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_BUTTON_PASTE)->flags |= D_DISABLED;
	}

	if(cep->tool_outline)
	{
		pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_BUTTON_TOOL_MODE_OUTLINE)->flags |= D_SELECTED;
	}
	else
	{
		pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_BUTTON_TOOL_MODE_OUTLINE)->flags &= ~D_SELECTED;
	}
}

static void update_layer_list(PA_UI * uip, PA_CANVAS_EDITOR * cep)
{
	T3GUI_ELEMENT * d = pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_LAYER_LIST);
	int entry_height = al_get_font_line_height(d->theme->state[0].font[0]) + d->theme->state[0].top_margin + d->theme->state[0].bottom_margin;
  int visible_elements = d->h / entry_height;

	while(d->d1 < d->d2)
	{
		d->d2 -= visible_elements;
		if(d->d2 < 0)
		{
			d->d2 = 0;
			break;
		}
	}
	while(d->d1 >= d->d2 + visible_elements)
	{
		d->d2 += visible_elements;
		if(d->d2 + visible_elements > cep->canvas->layer_max)
		{
			d->d2 = cep->canvas->layer_max - visible_elements;
			break;
		}
	}
}

void pa_process_ui(PA_UI * uip)
{
	PA_CANVAS_EDITOR * cep = (PA_CANVAS_EDITOR *)pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_CANVAS_EDITOR)->dp;
	int old_layer_d1;
	int old_layer = cep->current_layer;
	ALLEGRO_STATE old_state;

	pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_LAYER_LIST)->d1 = cep->canvas->layer_max - cep->current_layer - 1;
	old_layer_d1 = pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_LAYER_LIST)->d1;
	if(cep->canvas->layer_max > 1)
	{
		pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_BUTTON_REMOVE_LAYER)->flags &= ~D_DISABLED;
	}
	else
	{
		pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_BUTTON_REMOVE_LAYER)->flags |= D_DISABLED;
	}

	if(cep->current_tool != PA_TOOL_SELECTION && cep->current_tool != PA_TOOL_FRAME)
	{
		cep->want_cursor = ALLEGRO_SYSTEM_MOUSE_CURSOR_PRECISION;
	}
	if(t3gui_get_active_dialogs() < 2 && t3gui_get_mouse_x() >= pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_CANVAS_EDITOR)->x && t3gui_get_mouse_x() < pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_CANVAS_EDITOR)->x + pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_CANVAS_EDITOR)->w && t3gui_get_mouse_y() >= pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_CANVAS_EDITOR)->y && t3gui_get_mouse_y() < pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_CANVAS_EDITOR)->y + pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_CANVAS_EDITOR)->h)
	{
		pa_set_mouse_cursor(cep->want_cursor);
	}
	else
	{
		pa_set_mouse_cursor(ALLEGRO_SYSTEM_MOUSE_CURSOR_ARROW);
	}

	update_toolbar_flags(uip, cep);
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

	if(uip->color_popup_dialog)
	{
		pa_color_dialog_pre_logic(uip->color_popup_dialog);
	}
	t3gui_logic();

	/* update button selection */
	switch(cep->current_tool)
	{
		case PA_TOOL_PIXEL:
		{
			select_button(uip, PA_UI_ELEMENT_BUTTON_PIXEL);
			break;
		}
		case PA_TOOL_LINE:
		{
			select_button(uip, PA_UI_ELEMENT_BUTTON_LINE);
			break;
		}
		case PA_TOOL_RECTANGLE:
		{
			select_button(uip, PA_UI_ELEMENT_BUTTON_RECTANGLE);
			break;
		}
		case PA_TOOL_FILLED_RECTANGLE:
		{
			select_button(uip, PA_UI_ELEMENT_BUTTON_FILLED_RECTANGLE);
			break;
		}
		case PA_TOOL_OVAL:
		{
			select_button(uip, PA_UI_ELEMENT_BUTTON_OVAL);
			break;
		}
		case PA_TOOL_FILLED_OVAL:
		{
			select_button(uip, PA_UI_ELEMENT_BUTTON_FILLED_OVAL);
			break;
		}
		case PA_TOOL_FLOOD_FILL:
		{
			select_button(uip, PA_UI_ELEMENT_BUTTON_FLOOD_FILL);
			break;
		}
		case PA_TOOL_REPLACE:
		{
			select_button(uip, PA_UI_ELEMENT_BUTTON_REPLACE);
			break;
		}
		case PA_TOOL_ERASER:
		{
			select_button(uip, PA_UI_ELEMENT_BUTTON_ERASER);
			break;
		}
		case PA_TOOL_DROPPER:
		{
			select_button(uip, PA_UI_ELEMENT_BUTTON_DROPPER);
			break;
		}
		case PA_TOOL_SELECTION:
		{
			select_button(uip, PA_UI_ELEMENT_BUTTON_SELECTION);
			break;
		}
		case PA_TOOL_FRAME:
		{
			select_button(uip, PA_UI_ELEMENT_BUTTON_FRAME);
			break;
		}
		default:
		{
			select_button(uip, -1);
		}
	}
	if(pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_LAYER_LIST)->d1 != old_layer_d1)
	{
		pa_select_canvas_editor_layer(cep, cep->canvas->layer_max - pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_LAYER_LIST)->d1 - 1);
	}
	pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_LAYER_LIST)->d1 = cep->canvas->layer_max - cep->current_layer - 1;
	if(cep->current_layer != old_layer)
	{
		update_layer_list(uip, cep);
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
	int i, j;
	int tw, th;
	int scale = pa_get_theme_int(uip->main_dialog->theme, "checkerboard_scale", 24);
	bool held = al_is_bitmap_drawing_held();

	if(held)
	{
		al_hold_bitmap_drawing(false);
	}
	al_hold_bitmap_drawing(true);
	al_use_shader(cep->premultiplied_alpha_shader);
	tw = pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_CANVAS_EDITOR)->w / scale + 1;
	th = pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_CANVAS_EDITOR)->h / scale + 1;
	for(i = 0; i < th; i++)
	{
		for(j = 0; j < tw; j++)
		{
			t3f_draw_scaled_bitmap(uip->main_dialog->bitmap[PA_UI_BITMAP_BG], t3f_color_white, pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_CANVAS_EDITOR)->x + j * scale, pa_get_dialog_element(uip->main_dialog, PA_UI_ELEMENT_CANVAS_EDITOR)->y + i * scale, 0, scale, scale, 0);
		}
	}
	al_hold_bitmap_drawing(false);
	if(held)
	{
		al_hold_bitmap_drawing(true);
	}
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
