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

QUIXEL_UI * quixel_create_ui(QUIXEL_CANVAS_EDITOR * cep)
{
	QUIXEL_UI * uip;
	T3GUI_THEME * default_theme;
	int left_panel_width;
	int pos_y;
	int status_height;

	uip = malloc(sizeof(QUIXEL_UI));
	if(uip)
	{
		memset(uip, 0, sizeof(QUIXEL_UI));

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
		t3gui_dialog_add_element(uip->dialog[QUIXEL_UI_DIALOG_MAIN], NULL, t3gui_box_proc, 640 - 64, 0, 64, 480, 0, 0, 0, 0, NULL, NULL, NULL);
		pos_y = 0;
		t3gui_dialog_add_element(uip->dialog[QUIXEL_UI_DIALOG_MAIN], NULL, t3gui_push_button_proc, 640 - 64, pos_y, 64, 32, 0, 0, 0, 0, "Pixel", quixel_tool_pixel_button_proc, NULL);
		pos_y += 32;
		t3gui_dialog_add_element(uip->dialog[QUIXEL_UI_DIALOG_MAIN], NULL, t3gui_push_button_proc, 640 - 64, pos_y, 64, 32, 0, 0, 0, 0, "Line", quixel_tool_line_button_proc, NULL);
		pos_y += 32;
		t3gui_dialog_add_element(uip->dialog[QUIXEL_UI_DIALOG_MAIN], NULL, t3gui_push_button_proc, 640 - 64, pos_y, 64, 32, 0, 0, 0, 0, "Rectangle", quixel_tool_rectangle_button_proc, NULL);
		pos_y += 32;
		t3gui_dialog_add_element(uip->dialog[QUIXEL_UI_DIALOG_MAIN], NULL, t3gui_push_button_proc, 640 - 64, pos_y, 64, 32, 0, 0, 0, 0, "FRectangle", quixel_tool_filled_rectangle_button_proc, NULL);
		pos_y += 32;
		t3gui_dialog_add_element(uip->dialog[QUIXEL_UI_DIALOG_MAIN], NULL, t3gui_button_proc, 640 - 64, pos_y, 64, 32, 0, 0, 0, 0, "Oval", NULL, NULL);
		pos_y += 32;
		t3gui_dialog_add_element(uip->dialog[QUIXEL_UI_DIALOG_MAIN], NULL, t3gui_button_proc, 640 - 64, pos_y, 64, 32, 0, 0, 0, 0, "FOval", NULL, NULL);
		pos_y += 32;
		t3gui_dialog_add_element(uip->dialog[QUIXEL_UI_DIALOG_MAIN], NULL, t3gui_push_button_proc, 640 - 64, pos_y, 64, 32, 0, 0, 0, 0, "Dropper", quixel_tool_dropper_button_proc, NULL);

		left_panel_width = QUIXEL_COLOR_PICKER_SHADES * QUIXEL_COLOR_PICKER_SCALE + QUIXEL_COLOR_PICKER_SCALE;
		t3gui_dialog_add_element(uip->dialog[QUIXEL_UI_DIALOG_MAIN], NULL, t3gui_box_proc, 0, 0, left_panel_width, 480, 0, 0, 0, 0, NULL, NULL, NULL);
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
	t3gui_render();
}
