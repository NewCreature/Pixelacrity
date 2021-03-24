#include "t3f/t3f.h"
#include "ui.h"
#include "menu.h"
#include "menu_file_proc.h"
#include "menu_edit_proc.h"
#include "button_proc.h"
#include "canvas.h"
#include "canvas_file.h"
#include "canvas_helpers.h"
#include "palette.h"
#include "gui_color.h"
#include "gui_palette.h"
#include "gui_canvas_editor.h"
#include "ega_palette.h"

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

static bool add_color_picker(QUIXEL_CANVAS_EDITOR * cep, T3GUI_DIALOG * dp, int x, int y)
{
	int i, pos_x = x;
	int left_panel_width;

	left_panel_width = QUIXEL_COLOR_PICKER_SHADES * QUIXEL_COLOR_PICKER_SCALE + QUIXEL_COLOR_PICKER_SCALE;
	for(i = 0; i < QUIXEL_COLOR_PICKER_SHADES; i++)
	{
		t3gui_dialog_add_element(dp, NULL, quixel_gui_color_proc, pos_x, y, QUIXEL_COLOR_PICKER_SCALE, QUIXEL_COLOR_PICKER_SCALE, 0, 0, 0, 0, &cep->pick_color[i], &cep->left_color, NULL);
		if(i <= 0 || i >= QUIXEL_COLOR_PICKER_SHADES - 2)
		{
			pos_x += QUIXEL_COLOR_PICKER_SCALE + QUIXEL_COLOR_PICKER_SCALE / 2;
		}
		else
		{
			pos_x += QUIXEL_COLOR_PICKER_SCALE;
		}
	}

	return true;
}

static bool add_color_palette(QUIXEL_CANVAS_EDITOR * cep, T3GUI_DIALOG * dp, int x, int y)
{
	int i, j, pos_x = x;
	int left_panel_width;

	left_panel_width = QUIXEL_COLOR_PICKER_SHADES * QUIXEL_COLOR_PICKER_SCALE + QUIXEL_COLOR_PICKER_SCALE;
	for(i = 0; i < 9; i++)
	{
		t3gui_dialog_add_element(dp, NULL, quixel_gui_color_proc, pos_x, y, QUIXEL_COLOR_PICKER_SCALE, QUIXEL_COLOR_PICKER_SCALE, 0, 0, 0, 0, &cep->palette[i], &cep->base_color, NULL);
		pos_x += QUIXEL_COLOR_PICKER_SCALE;
	}
	y += QUIXEL_COLOR_PICKER_SCALE;
	for(i = 0; i < 8; i++)
	{
		pos_x = x;
		for(j = 0; j < 8; j++)
		{
			t3gui_dialog_add_element(dp, NULL, quixel_gui_color_proc, pos_x, y, QUIXEL_COLOR_PICKER_SCALE, QUIXEL_COLOR_PICKER_SCALE, 0, 0, 0, 0, &cep->palette[i * 8 + j + 9], &cep->base_color, NULL);
			pos_x += QUIXEL_COLOR_PICKER_SCALE;
		}
		y += QUIXEL_COLOR_PICKER_SCALE;
	}

	return true;
}

QUIXEL_UI * quixel_create_ui(QUIXEL_CANVAS_EDITOR * cep)
{
	QUIXEL_UI * uip;
	T3GUI_THEME * default_theme;
	int left_panel_width;
	int pos_y;
	int status_height;
	int i;

	uip = malloc(sizeof(QUIXEL_UI));
	if(uip)
	{
		memset(uip, 0, sizeof(QUIXEL_UI));

		uip->bitmap[QUIXEL_UI_BITMAP_BG] = make_checkerboard_bitmap(t3f_color_white, al_map_rgba_f(0.9, 0.9, 0.9, 1.0));
		if(!uip->bitmap[QUIXEL_UI_BITMAP_BG])
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
			cep->palette[9 + i] = quixel_get_ega_palette_color(i);
		}

		if(!quixel_setup_menus(uip))
		{
			goto fail;
		}
		t3f_attach_menu(uip->menu[QUIXEL_UI_MENU_MAIN]);

		uip->dialog[QUIXEL_UI_DIALOG_MAIN] = t3gui_create_dialog();
		if(!uip->dialog[QUIXEL_UI_DIALOG_MAIN])
		{
			goto fail;
		}
		t3gui_dialog_add_element(uip->dialog[QUIXEL_UI_DIALOG_MAIN], NULL, t3gui_box_proc, t3f_default_view->width - 64, 0, 64, t3f_default_view->height, 0, 0, 0, 0, NULL, NULL, NULL);
		pos_y = 0;
		t3gui_dialog_add_element(uip->dialog[QUIXEL_UI_DIALOG_MAIN], NULL, t3gui_push_button_proc, t3f_default_view->width - 64, pos_y, 64, 32, 0, 0, 0, 0, "Pixel", quixel_tool_pixel_button_proc, NULL);
		pos_y += 32;
		t3gui_dialog_add_element(uip->dialog[QUIXEL_UI_DIALOG_MAIN], NULL, t3gui_push_button_proc, t3f_default_view->width - 64, pos_y, 64, 32, 0, 0, 0, 0, "Line", quixel_tool_line_button_proc, NULL);
		pos_y += 32;
		t3gui_dialog_add_element(uip->dialog[QUIXEL_UI_DIALOG_MAIN], NULL, t3gui_push_button_proc, t3f_default_view->width - 64, pos_y, 64, 32, 0, 0, 0, 0, "Rectangle", quixel_tool_rectangle_button_proc, NULL);
		pos_y += 32;
		t3gui_dialog_add_element(uip->dialog[QUIXEL_UI_DIALOG_MAIN], NULL, t3gui_push_button_proc, t3f_default_view->width - 64, pos_y, 64, 32, 0, 0, 0, 0, "FRectangle", quixel_tool_filled_rectangle_button_proc, NULL);
		pos_y += 32;
		t3gui_dialog_add_element(uip->dialog[QUIXEL_UI_DIALOG_MAIN], NULL, t3gui_push_button_proc, t3f_default_view->width - 64, pos_y, 64, 32, 0, 0, 0, 0, "Oval", quixel_tool_oval_button_proc, NULL);
		pos_y += 32;
		t3gui_dialog_add_element(uip->dialog[QUIXEL_UI_DIALOG_MAIN], NULL, t3gui_push_button_proc, t3f_default_view->width - 64, pos_y, 64, 32, 0, 0, 0, 0, "FOval", quixel_tool_filled_oval_button_proc, NULL);
		pos_y += 32;
		t3gui_dialog_add_element(uip->dialog[QUIXEL_UI_DIALOG_MAIN], NULL, t3gui_push_button_proc, t3f_default_view->width - 64, pos_y, 64, 32, 0, 0, 0, 0, "Dropper", quixel_tool_dropper_button_proc, NULL);
		t3gui_dialog_add_element(uip->dialog[QUIXEL_UI_DIALOG_MAIN], NULL, t3gui_push_button_proc, t3f_default_view->width - 64, pos_y, 64, 32, 0, 0, 0, 0, "Selection", quixel_tool_selection_button_proc, NULL);

		left_panel_width = QUIXEL_COLOR_PICKER_SHADES * QUIXEL_COLOR_PICKER_SCALE + QUIXEL_COLOR_PICKER_SCALE;
		t3gui_dialog_add_element(uip->dialog[QUIXEL_UI_DIALOG_MAIN], NULL, t3gui_box_proc, 0, 0, left_panel_width, t3f_default_view->height, 0, 0, 0, 0, NULL, NULL, NULL);
		pos_y = 0;
		t3gui_dialog_add_element(uip->dialog[QUIXEL_UI_DIALOG_MAIN], NULL, quixel_gui_color_proc, 0, pos_y, left_panel_width / 2, 32, 0, 0, 0, 0, &cep->left_color, NULL, NULL);
		t3gui_dialog_add_element(uip->dialog[QUIXEL_UI_DIALOG_MAIN], NULL, quixel_gui_color_proc, left_panel_width / 2, 0, left_panel_width / 2, 32, 0, 0, 0, 0, &cep->right_color, NULL, NULL);
		pos_y += 32;
		add_color_picker(cep, uip->dialog[QUIXEL_UI_DIALOG_MAIN], 0, pos_y);
		pos_y += QUIXEL_COLOR_PICKER_SCALE;
		cep->shade_slider_element = t3gui_dialog_add_element(uip->dialog[QUIXEL_UI_DIALOG_MAIN], NULL, t3gui_slider_proc, 0, pos_y, QUIXEL_COLOR_PICKER_SHADES * QUIXEL_COLOR_PICKER_SCALE + QUIXEL_COLOR_PICKER_SCALE - QUIXEL_COLOR_PICKER_SCALE, QUIXEL_COLOR_PICKER_SCALE, 0, 0, 1000, 0, NULL, NULL, NULL);
		t3gui_dialog_add_element(uip->dialog[QUIXEL_UI_DIALOG_MAIN], NULL, quixel_gui_color_proc, QUIXEL_COLOR_PICKER_SHADES * QUIXEL_COLOR_PICKER_SCALE + QUIXEL_COLOR_PICKER_SCALE - QUIXEL_COLOR_PICKER_SCALE, pos_y, QUIXEL_COLOR_PICKER_SCALE, QUIXEL_COLOR_PICKER_SCALE, 0, 0, 0, 0, &cep->shade_color, &cep->left_color, NULL);
		pos_y += QUIXEL_COLOR_PICKER_SCALE;
		cep->alpha_slider_element = t3gui_dialog_add_element(uip->dialog[QUIXEL_UI_DIALOG_MAIN], NULL, t3gui_slider_proc, 0, pos_y, QUIXEL_COLOR_PICKER_SHADES * QUIXEL_COLOR_PICKER_SCALE + QUIXEL_COLOR_PICKER_SCALE - QUIXEL_COLOR_PICKER_SCALE, QUIXEL_COLOR_PICKER_SCALE, 0, 0, 1000, 0, NULL, NULL, NULL);
		t3gui_dialog_add_element(uip->dialog[QUIXEL_UI_DIALOG_MAIN], NULL, quixel_gui_color_proc, QUIXEL_COLOR_PICKER_SHADES * QUIXEL_COLOR_PICKER_SCALE + QUIXEL_COLOR_PICKER_SCALE - QUIXEL_COLOR_PICKER_SCALE, pos_y, QUIXEL_COLOR_PICKER_SCALE, QUIXEL_COLOR_PICKER_SCALE, 0, 0, 0, 0, &cep->alpha_color, &cep->left_color, NULL);
		pos_y += QUIXEL_COLOR_PICKER_SCALE;
		add_color_palette(cep, uip->dialog[QUIXEL_UI_DIALOG_MAIN], 0, pos_y);
		pos_y += QUIXEL_COLOR_PICKER_SCALE * 9;
		t3gui_dialog_add_element(uip->dialog[QUIXEL_UI_DIALOG_MAIN], NULL, quixel_gui_palette_proc, 0, pos_y, left_panel_width, t3f_default_view->height - pos_y, 0, 0, 0, 0, &cep->base_color, NULL, NULL);

		default_theme = t3gui_get_default_theme();
		if(!default_theme)
		{
			goto fail;
		}
		status_height = al_get_font_line_height(default_theme->state[0].font[0]) + QUIXEL_UI_ELEMENT_SPACE * 2;
		t3gui_dialog_add_element(uip->dialog[QUIXEL_UI_DIALOG_MAIN], NULL, t3gui_box_proc, 0, t3f_default_view->height - status_height, t3f_default_view->width, status_height, 0, 0, 0, 0, NULL, NULL, NULL);
		t3gui_dialog_add_element(uip->dialog[QUIXEL_UI_DIALOG_MAIN], NULL, t3gui_text_proc, QUIXEL_UI_ELEMENT_SPACE, t3f_default_view->height - status_height + QUIXEL_UI_ELEMENT_SPACE, t3f_default_view->width - QUIXEL_UI_ELEMENT_SPACE * 2, status_height - QUIXEL_UI_ELEMENT_SPACE * 2, 0, 0, 0, 0, uip->status_left_message, NULL, NULL);
		t3gui_dialog_add_element(uip->dialog[QUIXEL_UI_DIALOG_MAIN], NULL, t3gui_text_proc, QUIXEL_UI_ELEMENT_SPACE, t3f_default_view->height - status_height + QUIXEL_UI_ELEMENT_SPACE, t3f_default_view->width - QUIXEL_UI_ELEMENT_SPACE * 2, status_height - QUIXEL_UI_ELEMENT_SPACE * 2, 0, 0, 0, 0, uip->status_middle_message, NULL, NULL);
		t3gui_dialog_add_element(uip->dialog[QUIXEL_UI_DIALOG_MAIN], NULL, t3gui_text_proc, QUIXEL_UI_ELEMENT_SPACE, t3f_default_view->height - status_height + QUIXEL_UI_ELEMENT_SPACE, t3f_default_view->width - QUIXEL_UI_ELEMENT_SPACE * 2, status_height - QUIXEL_UI_ELEMENT_SPACE * 2, 0, 0, 0, 0, uip->status_right_message, NULL, NULL);

		cep->editor_element = t3gui_dialog_add_element(uip->dialog[QUIXEL_UI_DIALOG_MAIN], NULL, quixel_gui_canvas_editor_proc, left_panel_width, 0, t3f_default_view->width - left_panel_width - 64, t3f_default_view->height - status_height, 0, 0, 0, 0, cep, NULL, NULL);
	}
	return uip;

	fail:
	{
		quixel_destroy_ui(uip);
		return NULL;
	}
}

void quixel_destroy_ui(QUIXEL_UI * uip)
{
	int i;

	for(i = 0; i < QUIXEL_UI_MAX_BITMAPS; i++)
	{
		if(uip->bitmap[i])
		{
			al_destroy_bitmap(uip->bitmap[i]);
		}
	}
	quixel_destroy_menus(uip);
	for(i = 0; i < QUIXEL_UI_MAX_DIALOGS; i++)
	{
		if(uip->dialog[i])
		{
			t3gui_destroy_dialog(uip->dialog[i]);
		}
	}
	for(i = 0; i < QUIXEL_UI_MAX_THEMES; i++)
	{
		if(uip->theme[i])
		{
			t3gui_destroy_theme(uip->theme[i]);
		}
	}
	free(uip);
}

void quixel_process_ui(QUIXEL_UI * uip)
{
	t3gui_logic();
}

void quixel_render_ui(QUIXEL_UI * uip)
{
	int i, j;
	int tw, th;

	tw = t3f_default_view->width / QUIXEL_UI_BG_SCALE + 1;
	th = t3f_default_view->height / QUIXEL_UI_BG_SCALE + 1;
	for(i = 0; i < th; i++)
	{
		for(j = 0; j < tw; j++)
		{
			t3f_draw_scaled_bitmap(uip->bitmap[QUIXEL_UI_BITMAP_BG], t3f_color_white, j * QUIXEL_UI_BG_SCALE, i * QUIXEL_UI_BG_SCALE, 0, QUIXEL_UI_BG_SCALE, QUIXEL_UI_BG_SCALE, 0);
		}
	}
	t3gui_render();
}
