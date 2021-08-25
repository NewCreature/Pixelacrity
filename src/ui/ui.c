#include "t3f/t3f.h"
#include "t3gui/t3gui.h"
#include "ui.h"
#include "window.h"
#include "menu/menu.h"
#include "menu/file_proc.h"
#include "menu/edit_proc.h"
#include "gui/button_proc.h"
#include "modules/mouse_cursor.h"
#include "modules/canvas/canvas.h"
#include "modules/canvas/canvas_file.h"
#include "modules/canvas/canvas_helpers.h"
#include "gui/color.h"
#include "gui/palette.h"
#include "gui/canvas_editor/element.h"
#include "modules/ega_palette.h"
#include "gui/list_proc.h"
#include "gui/layer_list_proc.h"
#include "ui/canvas_editor/undo.h"

static ALLEGRO_BITMAP * make_checkerboard_bitmap(ALLEGRO_COLOR c1, ALLEGRO_COLOR c2)
{
	ALLEGRO_STATE old_state;
	ALLEGRO_TRANSFORM identity;
	ALLEGRO_BITMAP * bp;

	al_store_state(&old_state, ALLEGRO_STATE_NEW_BITMAP_PARAMETERS | ALLEGRO_STATE_TARGET_BITMAP | ALLEGRO_STATE_TRANSFORM);
	al_set_new_bitmap_flags(0);
	bp = al_create_bitmap(2, 2);
	if(bp)
	{
		al_set_target_bitmap(bp);
		al_identity_transform(&identity);
		al_use_transform(&identity);
		al_lock_bitmap(bp, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_WRITEONLY);
		al_put_pixel(0, 0, c1);
		al_put_pixel(0, 1, c2);
		al_put_pixel(1, 0, c2);
		al_put_pixel(1, 1, c1);
		al_unlock_bitmap(bp);
	}
	al_restore_state(&old_state);
	return bp;
}

static bool add_color_picker(PA_UI * uip, PA_CANVAS_EDITOR * cep, T3GUI_DIALOG * dp, int x, int y)
{
	int i, pos_x = x;
	int left_panel_width;

	left_panel_width = PA_COLOR_PICKER_SHADES * PA_COLOR_PICKER_SCALE + PA_COLOR_PICKER_SCALE;
	for(i = 0; i < PA_COLOR_PICKER_SHADES; i++)
	{
		uip->color_picker_element[i] = t3gui_dialog_add_element(dp, NULL, pa_gui_color_proc, 0, 0, 0, 0, 0, 0, 0, 0, &cep->pick_color[i], &cep->left_color.color, &cep->right_color.color);
		if(i <= 0 || i >= PA_COLOR_PICKER_SHADES - 2)
		{
			pos_x += PA_COLOR_PICKER_SCALE + PA_COLOR_PICKER_SCALE / 2;
		}
		else
		{
			pos_x += PA_COLOR_PICKER_SCALE;
		}
	}

	return true;
}

static bool add_color_palette(PA_UI * uip, PA_CANVAS_EDITOR * cep, T3GUI_DIALOG * dp, int x, int y)
{
	int i, j, pos_x = x;
	int left_panel_width;

	left_panel_width = PA_COLOR_PICKER_SHADES * PA_COLOR_PICKER_SCALE + PA_COLOR_PICKER_SCALE;
	for(i = 0; i < 9; i++)
	{
		uip->palette_color_element[i] = t3gui_dialog_add_element(dp, NULL, pa_gui_color_proc, 0, 0, 0, 0, 0, 0, 0, 0, &cep->palette[i], &cep->left_color.base_color, &cep->right_color.base_color);
		pos_x += PA_COLOR_PICKER_SCALE;
	}
	y += PA_COLOR_PICKER_SCALE;
	for(i = 0; i < 8; i++)
	{
		pos_x = x;
		for(j = 0; j < 8; j++)
		{
			uip->palette_color_element[i * 8 + j + 9] = t3gui_dialog_add_element(dp, NULL, pa_gui_color_proc, 0, 0, 0, 0, 0, 0, 0, 0, &cep->palette[i * 8 + j + 9], &cep->left_color.base_color, &cep->right_color.base_color);
			pos_x += PA_COLOR_PICKER_SCALE;
		}
		y += PA_COLOR_PICKER_SCALE;
	}

	return true;
}

static void resize_element(T3GUI_ELEMENT * ep, int x, int y, int w, int h)
{
	ep->x = x;
	ep->y = y;
	ep->w = w;
	ep->h = h;
}

static void resize_color_picker(PA_UI * uip, int x, int y)
{
	int i, pos_x = x;
	int left_panel_width;

	left_panel_width = PA_COLOR_PICKER_SHADES * PA_COLOR_PICKER_SCALE + PA_COLOR_PICKER_SCALE;
	for(i = 0; i < PA_COLOR_PICKER_SHADES; i++)
	{
		resize_element(uip->color_picker_element[i], pos_x, y, PA_COLOR_PICKER_SCALE, PA_COLOR_PICKER_SCALE);
		if(i <= 0 || i >= PA_COLOR_PICKER_SHADES - 2)
		{
			pos_x += PA_COLOR_PICKER_SCALE + PA_COLOR_PICKER_SCALE / 2;
		}
		else
		{
			pos_x += PA_COLOR_PICKER_SCALE;
		}
	}
}

static void resize_color_palette(PA_UI * uip, int x, int y)
{
	int i, j, pos_x = x;
	int left_panel_width;

	left_panel_width = PA_COLOR_PICKER_SHADES * PA_COLOR_PICKER_SCALE + PA_COLOR_PICKER_SCALE;
	for(i = 0; i < 9; i++)
	{
		resize_element(uip->palette_color_element[i], pos_x, y, PA_COLOR_PICKER_SCALE, PA_COLOR_PICKER_SCALE);
		pos_x += PA_COLOR_PICKER_SCALE;
	}
	y += PA_COLOR_PICKER_SCALE;
	for(i = 0; i < 8; i++)
	{
		pos_x = x;
		for(j = 0; j < 8; j++)
		{
			resize_element(uip->palette_color_element[i * 8 + j + 9], pos_x, y, PA_COLOR_PICKER_SCALE, PA_COLOR_PICKER_SCALE);
			pos_x += PA_COLOR_PICKER_SCALE;
		}
		y += PA_COLOR_PICKER_SCALE;
	}
}

void pa_resize_ui(PA_UI * uip)
{
	PA_CANVAS_EDITOR * canvas_editor = NULL;
	T3GUI_THEME * default_theme;
	int left_pane_width;
	int right_pane_width;
	int pos_y;
	int status_height;

	right_pane_width = 96;
	resize_element(uip->element[PA_UI_ELEMENT_RIGHT_PANE], t3f_default_view->width - right_pane_width, 0, right_pane_width, t3f_default_view->height);

	pos_y = 0;
	resize_element(uip->element[PA_UI_ELEMENT_BUTTON_PIXEL], t3f_default_view->width - right_pane_width, pos_y, right_pane_width, 32);
	pos_y += 32;
	resize_element(uip->element[PA_UI_ELEMENT_BUTTON_LINE], t3f_default_view->width - right_pane_width, pos_y, right_pane_width, 32);
	pos_y += 32;
	resize_element(uip->element[PA_UI_ELEMENT_BUTTON_RECTANGLE], t3f_default_view->width - right_pane_width, pos_y, right_pane_width, 32);
	pos_y += 32;
	resize_element(uip->element[PA_UI_ELEMENT_BUTTON_FILLED_RECTANGLE], t3f_default_view->width - right_pane_width, pos_y, right_pane_width, 32);
	pos_y += 32;
	resize_element(uip->element[PA_UI_ELEMENT_BUTTON_OVAL], t3f_default_view->width - right_pane_width, pos_y, right_pane_width, 32);
	pos_y += 32;
	resize_element(uip->element[PA_UI_ELEMENT_BUTTON_FILLED_OVAL], t3f_default_view->width - right_pane_width, pos_y, right_pane_width, 32);
	pos_y += 32;
	resize_element(uip->element[PA_UI_ELEMENT_BUTTON_FLOOD_FILL], t3f_default_view->width - right_pane_width, pos_y, right_pane_width, 32);
	pos_y += 32;
	resize_element(uip->element[PA_UI_ELEMENT_BUTTON_ERASER], t3f_default_view->width - right_pane_width, pos_y, right_pane_width, 32);
	pos_y += 32;
	resize_element(uip->element[PA_UI_ELEMENT_BUTTON_DROPPER], t3f_default_view->width - right_pane_width, pos_y, right_pane_width, 32);
	pos_y += 32;
	resize_element(uip->element[PA_UI_ELEMENT_BUTTON_SELECTION], t3f_default_view->width - right_pane_width, pos_y, right_pane_width, 32);
	pos_y += 32;
	resize_element(uip->element[PA_UI_ELEMENT_LAYER_LIST], t3f_default_view->width - right_pane_width, pos_y, right_pane_width, 128);
	pos_y += 128;
	resize_element(uip->element[PA_UI_ELEMENT_BUTTON_ADD_LAYER], t3f_default_view->width - right_pane_width, pos_y, right_pane_width / 2, 32);
	resize_element(uip->element[PA_UI_ELEMENT_BUTTON_REMOVE_LAYER], t3f_default_view->width - right_pane_width / 2, pos_y, right_pane_width / 2, 32);

	left_pane_width = PA_COLOR_PICKER_SHADES * PA_COLOR_PICKER_SCALE + PA_COLOR_PICKER_SCALE;
	resize_element(uip->element[PA_UI_ELEMENT_LEFT_PANE], 0, 0, left_pane_width, t3f_default_view->height);
	pos_y = 0;
	resize_element(uip->element[PA_UI_ELEMENT_LEFT_COLOR], 0, pos_y, left_pane_width / 2, 48);
	resize_element(uip->element[PA_UI_ELEMENT_RIGHT_COLOR], left_pane_width / 2, pos_y, left_pane_width / 2, 48);
	pos_y += 48;
	resize_element(uip->element[PA_UI_ELEMENT_LEFT_SHADE_SLIDER], 0, pos_y, left_pane_width / 2, PA_COLOR_PICKER_SCALE);
	resize_element(uip->element[PA_UI_ELEMENT_RIGHT_SHADE_SLIDER], left_pane_width / 2, pos_y, left_pane_width / 2, PA_COLOR_PICKER_SCALE);
	pos_y += PA_COLOR_PICKER_SCALE;
	resize_element(uip->element[PA_UI_ELEMENT_LEFT_ALPHA_SLIDER], 0, pos_y, left_pane_width / 2, PA_COLOR_PICKER_SCALE);
	resize_element(uip->element[PA_UI_ELEMENT_RIGHT_ALPHA_SLIDER], left_pane_width / 2, pos_y, left_pane_width / 2, PA_COLOR_PICKER_SCALE);
	pos_y += PA_COLOR_PICKER_SCALE;
	pos_y += PA_UI_ELEMENT_SPACE;

	resize_color_picker(uip, 0, pos_y);
	pos_y += PA_COLOR_PICKER_SCALE;
	pos_y += PA_UI_ELEMENT_SPACE;

	resize_color_palette(uip, 0, pos_y);
	pos_y += PA_COLOR_PICKER_SCALE * 9;
	pos_y += PA_UI_ELEMENT_SPACE;

	resize_element(uip->element[PA_UI_ELEMENT_PALETTE], 0, pos_y, left_pane_width, t3f_default_view->height - pos_y);
	pa_resize_palette(uip->element[PA_UI_ELEMENT_PALETTE]->w, uip->element[PA_UI_ELEMENT_PALETTE]->h);

	default_theme = t3gui_get_default_theme();
	if(!default_theme)
	{
		return;
	}
	status_height = al_get_font_line_height(default_theme->state[0].font[0]) + PA_UI_ELEMENT_SPACE * 2;
	pos_y = t3f_default_view->height - status_height;
	resize_element(uip->element[PA_UI_ELEMENT_STATUS_BAR], 0, pos_y, t3f_default_view->width, status_height);
	pos_y += PA_UI_ELEMENT_SPACE;
	resize_element(uip->element[PA_UI_ELEMENT_STATUS_LEFT_MESSAGE], PA_UI_ELEMENT_SPACE, pos_y, t3f_default_view->width - PA_UI_ELEMENT_SPACE * 2, status_height);
	resize_element(uip->element[PA_UI_ELEMENT_STATUS_MIDDLE_MESSAGE], PA_UI_ELEMENT_SPACE, pos_y, t3f_default_view->width - PA_UI_ELEMENT_SPACE * 2, status_height);
	resize_element(uip->element[PA_UI_ELEMENT_STATUS_RIGHT_MESSAGE], PA_UI_ELEMENT_SPACE, pos_y, t3f_default_view->width - PA_UI_ELEMENT_SPACE * 2, status_height);

	resize_element(uip->element[PA_UI_ELEMENT_CANVAS_EDITOR], left_pane_width, 0, t3f_default_view->width - left_pane_width - right_pane_width, t3f_default_view->height - status_height);

	canvas_editor = (PA_CANVAS_EDITOR *)uip->element[PA_UI_ELEMENT_CANVAS_EDITOR]->dp;
	pa_set_canvas_editor_zoom(canvas_editor, canvas_editor->view_zoom);
}

PA_UI * pa_create_ui(PA_CANVAS_EDITOR * cep)
{
	PA_UI * uip;
	int i;

	uip = malloc(sizeof(PA_UI));
	if(uip)
	{
		memset(uip, 0, sizeof(PA_UI));

		uip->bitmap[PA_UI_BITMAP_BG] = make_checkerboard_bitmap(t3f_color_white, al_map_rgba_f(0.9, 0.9, 0.9, 1.0));
		if(!uip->bitmap[PA_UI_BITMAP_BG])
		{
			goto fail;
		}

		cep->palette[0] = al_map_rgb_f(1.0, 0.0, 0.0);
		cep->palette[1] = al_map_rgb_f(0.0, 1.0, 0.0);
		cep->palette[2] = al_map_rgb_f(0.0, 0.0, 1.0);
		cep->palette[3] = al_map_rgb_f(1.0, 1.0, 0.0);
		cep->palette[4] = al_map_rgb_f(1.0, 0.0, 1.0);
		cep->palette[5] = al_map_rgb_f(0.0, 1.0, 1.0);
		cep->palette[6] = al_map_rgb_f(1.0, 1.0, 1.0);
		cep->palette[7] = al_map_rgb_f(0.0, 0.0, 0.0);
		cep->palette[8] = al_map_rgb_f(0.5, 0.5, 0.5);
		for(i = 0; i < 64; i++)
		{
			cep->palette[9 + i] = pa_get_ega_palette_color(i);
		}

		if(!pa_setup_menus(uip))
		{
			goto fail;
		}
		pa_update_undo_name(cep);
		pa_update_redo_name(cep);
		t3f_attach_menu(uip->menu[PA_UI_MENU_MAIN]);

		uip->dialog[PA_UI_DIALOG_MAIN] = t3gui_create_dialog();
		if(!uip->dialog[PA_UI_DIALOG_MAIN])
		{
			goto fail;
		}
		uip->element[PA_UI_ELEMENT_RIGHT_PANE] = t3gui_dialog_add_element(uip->dialog[PA_UI_DIALOG_MAIN], NULL, t3gui_box_proc, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL);
		uip->element[PA_UI_ELEMENT_BUTTON_PIXEL] = t3gui_dialog_add_element(uip->dialog[PA_UI_DIALOG_MAIN], NULL, t3gui_push_button_proc, 0, 0, 0, 0, 0, 0, 0, 0, "Pixel", pa_tool_pixel_button_proc, NULL);
		uip->element[PA_UI_ELEMENT_BUTTON_LINE] = t3gui_dialog_add_element(uip->dialog[PA_UI_DIALOG_MAIN], NULL, t3gui_push_button_proc, 0, 0, 0, 0, 0, 0, 0, 0, "Line", pa_tool_line_button_proc, NULL);
		uip->element[PA_UI_ELEMENT_BUTTON_RECTANGLE] = t3gui_dialog_add_element(uip->dialog[PA_UI_DIALOG_MAIN], NULL, t3gui_push_button_proc, 0, 0, 0, 0, 0, 0, 0, 0, "Rectangle", pa_tool_rectangle_button_proc, NULL);
		uip->element[PA_UI_ELEMENT_BUTTON_FILLED_RECTANGLE] = t3gui_dialog_add_element(uip->dialog[PA_UI_DIALOG_MAIN], NULL, t3gui_push_button_proc, 0, 0, 0, 0, 0, 0, 0, 0, "FRectangle", pa_tool_filled_rectangle_button_proc, NULL);
		uip->element[PA_UI_ELEMENT_BUTTON_OVAL] = t3gui_dialog_add_element(uip->dialog[PA_UI_DIALOG_MAIN], NULL, t3gui_push_button_proc, 0, 0, 0, 0, 0, 0, 0, 0, "Oval", pa_tool_oval_button_proc, NULL);
		uip->element[PA_UI_ELEMENT_BUTTON_FILLED_OVAL] = t3gui_dialog_add_element(uip->dialog[PA_UI_DIALOG_MAIN], NULL, t3gui_push_button_proc, 0, 0, 0, 0, 0, 0, 0, 0, "FOval", pa_tool_filled_oval_button_proc, NULL);
		uip->element[PA_UI_ELEMENT_BUTTON_FLOOD_FILL] = t3gui_dialog_add_element(uip->dialog[PA_UI_DIALOG_MAIN], NULL, t3gui_push_button_proc, 0, 0, 0, 0, 0, 0, 0, 0, "Flood", pa_tool_flood_fill_button_proc, NULL);
		uip->element[PA_UI_ELEMENT_BUTTON_ERASER] = t3gui_dialog_add_element(uip->dialog[PA_UI_DIALOG_MAIN], NULL, t3gui_push_button_proc, 0, 0, 0, 0, 0, 0, 0, 0, "Eraser", pa_tool_eraser_button_proc, NULL);
		uip->element[PA_UI_ELEMENT_BUTTON_DROPPER] = t3gui_dialog_add_element(uip->dialog[PA_UI_DIALOG_MAIN], NULL, t3gui_push_button_proc, 0, 0, 0, 0, 0, 0, 0, 0, "Dropper", pa_tool_dropper_button_proc, NULL);
		uip->element[PA_UI_ELEMENT_BUTTON_SELECTION] = t3gui_dialog_add_element(uip->dialog[PA_UI_DIALOG_MAIN], NULL, t3gui_push_button_proc, 0, 0, 0, 0, 0, 0, 0, 0, "Selection", pa_tool_selection_button_proc, NULL);
		uip->element[PA_UI_ELEMENT_LAYER_LIST] = t3gui_dialog_add_element(uip->dialog[PA_UI_DIALOG_MAIN], NULL, pa_list_proc, 0, 0, 0, 0, 0, D_SETFOCUS, 0, 0, pa_layer_list_proc, NULL, cep);
		uip->element[PA_UI_ELEMENT_BUTTON_ADD_LAYER] = t3gui_dialog_add_element(uip->dialog[PA_UI_DIALOG_MAIN], NULL, t3gui_push_button_proc, 0, 0, 0, 0, 0, 0, 0, 0, "+", pa_layer_add_button_proc, NULL);
		uip->element[PA_UI_ELEMENT_BUTTON_REMOVE_LAYER] = t3gui_dialog_add_element(uip->dialog[PA_UI_DIALOG_MAIN], NULL, t3gui_push_button_proc, 0, 0, 0, 0, 0, 0, 0, 0, "-", pa_layer_remove_button_proc, NULL);

		uip->element[PA_UI_ELEMENT_LEFT_PANE] = t3gui_dialog_add_element(uip->dialog[PA_UI_DIALOG_MAIN], NULL, t3gui_box_proc, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL);
		uip->element[PA_UI_ELEMENT_LEFT_COLOR] = t3gui_dialog_add_element(uip->dialog[PA_UI_DIALOG_MAIN], NULL, pa_gui_color_proc, 0, 0, 0, 0, 0, 0, 0, 0, &cep->left_color.color, NULL, NULL);
		uip->element[PA_UI_ELEMENT_RIGHT_COLOR] = t3gui_dialog_add_element(uip->dialog[PA_UI_DIALOG_MAIN], NULL, pa_gui_color_proc, 0, 0, 0, 0, 0, 0, 0, 0, &cep->right_color.color, NULL, NULL);
		cep->left_color.shade_slider_element = t3gui_dialog_add_element(uip->dialog[PA_UI_DIALOG_MAIN], NULL, t3gui_slider_proc, 0, 0, 0, 0, 0, 0, 1000, 0, NULL, NULL, NULL);
		uip->element[PA_UI_ELEMENT_LEFT_SHADE_SLIDER] = cep->left_color.shade_slider_element;
		cep->right_color.shade_slider_element = t3gui_dialog_add_element(uip->dialog[PA_UI_DIALOG_MAIN], NULL, t3gui_slider_proc, 0, 0, 0, 0, 0, 0, 1000, 0, NULL, NULL, NULL);
		uip->element[PA_UI_ELEMENT_RIGHT_SHADE_SLIDER] = cep->right_color.shade_slider_element;
		cep->left_color.alpha_slider_element = t3gui_dialog_add_element(uip->dialog[PA_UI_DIALOG_MAIN], NULL, t3gui_slider_proc, 0, 0, 0, 0, 0, 0, 1000, 0, NULL, NULL, NULL);
		uip->element[PA_UI_ELEMENT_LEFT_ALPHA_SLIDER] = cep->left_color.alpha_slider_element;
		cep->right_color.alpha_slider_element = t3gui_dialog_add_element(uip->dialog[PA_UI_DIALOG_MAIN], NULL, t3gui_slider_proc, 0, 0, 0, 0, 0, 0, 1000, 0, NULL, NULL, NULL);
		uip->element[PA_UI_ELEMENT_RIGHT_ALPHA_SLIDER] = cep->right_color.alpha_slider_element;

		add_color_picker(uip, cep, uip->dialog[PA_UI_DIALOG_MAIN], 0, 0);

		add_color_palette(uip, cep, uip->dialog[PA_UI_DIALOG_MAIN], 0, 0);

		uip->element[PA_UI_ELEMENT_PALETTE] = t3gui_dialog_add_element(uip->dialog[PA_UI_DIALOG_MAIN], NULL, pa_gui_palette_proc, 0, 0, 0, 0, 0, 0, 0, 0, &cep->left_color.base_color, &cep->right_color.base_color, NULL);

		uip->element[PA_UI_ELEMENT_STATUS_BAR] = t3gui_dialog_add_element(uip->dialog[PA_UI_DIALOG_MAIN], NULL, t3gui_box_proc, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL);
		uip->element[PA_UI_ELEMENT_STATUS_LEFT_MESSAGE] = t3gui_dialog_add_element(uip->dialog[PA_UI_DIALOG_MAIN], NULL, t3gui_text_proc, 0, 0, 0, 0, 0, 0, 0, 0, uip->status_left_message, NULL, NULL);
		uip->element[PA_UI_ELEMENT_STATUS_MIDDLE_MESSAGE] = t3gui_dialog_add_element(uip->dialog[PA_UI_DIALOG_MAIN], NULL, t3gui_text_proc, 0, 0, 0, 0, 0, 0, 0, 0, uip->status_middle_message, NULL, NULL);
		uip->element[PA_UI_ELEMENT_STATUS_RIGHT_MESSAGE] = t3gui_dialog_add_element(uip->dialog[PA_UI_DIALOG_MAIN], NULL, t3gui_text_proc, 0, 0, 0, 0, 0, 0, 0, 0, uip->status_right_message, NULL, NULL);

		cep->editor_element = t3gui_dialog_add_element(uip->dialog[PA_UI_DIALOG_MAIN], NULL, pa_gui_canvas_editor_proc, 0, 0, 0, 0, 0, 0, 0, 0, cep, NULL, NULL);
		uip->element[PA_UI_ELEMENT_CANVAS_EDITOR] = cep->editor_element;
		pa_set_color(&cep->left_color, al_map_rgba_f(1.0, 0.0, 0.0, 1.0));
		pa_set_color(&cep->right_color, al_map_rgba_f(0.0, 0.0, 0.0, 1.0));
		pa_canvas_editor_update_pick_colors(cep);
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
	int i;

	for(i = 0; i < PA_UI_MAX_BITMAPS; i++)
	{
		if(uip->bitmap[i])
		{
			al_destroy_bitmap(uip->bitmap[i]);
		}
	}
	pa_destroy_menus(uip);
	for(i = 0; i < PA_UI_MAX_DIALOGS; i++)
	{
		if(uip->dialog[i])
		{
			t3gui_destroy_dialog(uip->dialog[i]);
		}
	}
	for(i = 0; i < PA_UI_MAX_THEMES; i++)
	{
		if(uip->theme[i])
		{
			t3gui_destroy_theme(uip->theme[i]);
		}
	}
	free(uip);
}

static void select_button(PA_UI * uip, int button)
{
	uip->element[PA_UI_ELEMENT_BUTTON_PIXEL]->flags &= ~D_SELECTED;
	uip->element[PA_UI_ELEMENT_BUTTON_LINE]->flags &= ~D_SELECTED;
	uip->element[PA_UI_ELEMENT_BUTTON_RECTANGLE]->flags &= ~D_SELECTED;
	uip->element[PA_UI_ELEMENT_BUTTON_FILLED_RECTANGLE]->flags &= ~D_SELECTED;
	uip->element[PA_UI_ELEMENT_BUTTON_OVAL]->flags &= ~D_SELECTED;
	uip->element[PA_UI_ELEMENT_BUTTON_FILLED_OVAL]->flags &= ~D_SELECTED;
	uip->element[PA_UI_ELEMENT_BUTTON_ERASER]->flags &= ~D_SELECTED;
	uip->element[PA_UI_ELEMENT_BUTTON_FLOOD_FILL]->flags &= ~D_SELECTED;
	uip->element[PA_UI_ELEMENT_BUTTON_DROPPER]->flags &= ~D_SELECTED;
	uip->element[PA_UI_ELEMENT_BUTTON_SELECTION]->flags &= ~D_SELECTED;
	uip->element[button]->flags |= D_SELECTED;
}

void pa_process_ui(PA_UI * uip)
{
	PA_CANVAS_EDITOR * cep = (PA_CANVAS_EDITOR *)uip->element[PA_UI_ELEMENT_CANVAS_EDITOR]->dp;
	int old_layer_d1;

	uip->element[PA_UI_ELEMENT_LAYER_LIST]->d1 = cep->canvas->layer_max - cep->current_layer - 1;
	old_layer_d1 = uip->element[PA_UI_ELEMENT_LAYER_LIST]->d1;
	if(cep->canvas->layer_max > 1)
	{
		uip->element[PA_UI_ELEMENT_BUTTON_REMOVE_LAYER]->flags = 0;
	}
	else
	{
		uip->element[PA_UI_ELEMENT_BUTTON_REMOVE_LAYER]->flags = D_DISABLED;
	}

	if(t3gui_get_mouse_x() >= uip->element[PA_UI_ELEMENT_CANVAS_EDITOR]->x && t3gui_get_mouse_x() < uip->element[PA_UI_ELEMENT_CANVAS_EDITOR]->x + uip->element[PA_UI_ELEMENT_CANVAS_EDITOR]->w && t3gui_get_mouse_y() >= uip->element[PA_UI_ELEMENT_CANVAS_EDITOR]->y && t3gui_get_mouse_y() < uip->element[PA_UI_ELEMENT_CANVAS_EDITOR]->y + uip->element[PA_UI_ELEMENT_CANVAS_EDITOR]->h)
	{
		pa_set_mouse_cursor(cep->want_cursor);
	}
	else
	{
		pa_set_mouse_cursor(ALLEGRO_SYSTEM_MOUSE_CURSOR_ARROW);
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
	}

	if(uip->element[PA_UI_ELEMENT_LAYER_LIST]->d1 != old_layer_d1)
	{
		pa_select_canvas_editor_layer(cep, cep->canvas->layer_max - uip->element[PA_UI_ELEMENT_LAYER_LIST]->d1 - 1);
	}
	pa_update_window_title(cep);
}

void pa_render_ui(PA_UI * uip)
{
	PA_CANVAS_EDITOR * cep = (PA_CANVAS_EDITOR *)uip->element[PA_UI_ELEMENT_CANVAS_EDITOR]->dp;
	int i, j;
	int tw, th;

	al_use_shader(cep->premultiplied_alpha_shader);
	tw = t3f_default_view->width / PA_UI_BG_SCALE + 1;
	th = t3f_default_view->height / PA_UI_BG_SCALE + 1;
	for(i = 0; i < th; i++)
	{
		for(j = 0; j < tw; j++)
		{
			t3f_draw_scaled_bitmap(uip->bitmap[PA_UI_BITMAP_BG], t3f_color_white, j * PA_UI_BG_SCALE, i * PA_UI_BG_SCALE, 0, PA_UI_BG_SCALE, PA_UI_BG_SCALE, 0);
		}
	}
	t3gui_render();
	al_use_shader(cep->standard_shader);
}
