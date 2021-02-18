#include "t3f/t3f.h"
#include "ui.h"
#include "menu.h"
#include "menu_file_proc.h"
#include "menu_edit_proc.h"
#include "canvas.h"
#include "canvas_file.h"
#include "canvas_helpers.h"
#include "palette.h"
#include "gui_color_picker.h"
#include "gui_color.h"
#include "gui_palette.h"

QUIXEL_UI * quixel_create_ui(QUIXEL_CANVAS_EDITOR * cep)
{
	QUIXEL_UI * uip;
	int left_panel_width;

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
		t3gui_dialog_add_element(uip->dialog[QUIXEL_UI_DIALOG_MAIN], NULL, t3gui_button_proc, 640 - 64, 0, 64, 32, 0, 0, 0, 0, "Pixel", NULL, NULL);
		t3gui_dialog_add_element(uip->dialog[QUIXEL_UI_DIALOG_MAIN], NULL, t3gui_button_proc, 640 - 64, 32, 64, 32, 0, 0, 0, 0, "Line", NULL, NULL);
		t3gui_dialog_add_element(uip->dialog[QUIXEL_UI_DIALOG_MAIN], NULL, t3gui_button_proc, 640 - 64, 64, 64, 32, 0, 0, 0, 0, "Rectangle", NULL, NULL);
		t3gui_dialog_add_element(uip->dialog[QUIXEL_UI_DIALOG_MAIN], NULL, t3gui_button_proc, 640 - 64, 96, 64, 32, 0, 0, 0, 0, "FRectangle", NULL, NULL);
		t3gui_dialog_add_element(uip->dialog[QUIXEL_UI_DIALOG_MAIN], NULL, t3gui_button_proc, 640 - 64, 128, 64, 32, 0, 0, 0, 0, "Oval", NULL, NULL);
		t3gui_dialog_add_element(uip->dialog[QUIXEL_UI_DIALOG_MAIN], NULL, t3gui_button_proc, 640 - 64, 160, 64, 32, 0, 0, 0, 0, "FOval", NULL, NULL);

		left_panel_width = QUIXEL_COLOR_PICKER_SHADES * QUIXEL_COLOR_PICKER_SCALE + QUIXEL_COLOR_PICKER_SCALE;
		t3gui_dialog_add_element(uip->dialog[QUIXEL_UI_DIALOG_MAIN], NULL, t3gui_box_proc, 0, 0, left_panel_width, 480, 0, 0, 0, 0, NULL, NULL, NULL);
		t3gui_dialog_add_element(uip->dialog[QUIXEL_UI_DIALOG_MAIN], NULL, quixel_gui_color_proc, 0, 0, left_panel_width / 2, 32, 0, 0, 0, 0, &cep->left_color, NULL, NULL);
		t3gui_dialog_add_element(uip->dialog[QUIXEL_UI_DIALOG_MAIN], NULL, quixel_gui_color_proc, left_panel_width / 2, 0, left_panel_width / 2, 32, 0, 0, 0, 0, &cep->right_color, NULL, NULL);
		t3gui_dialog_add_element(uip->dialog[QUIXEL_UI_DIALOG_MAIN], NULL, quixel_gui_color_picker_proc, 0, 32, QUIXEL_COLOR_PICKER_SHADES * QUIXEL_COLOR_PICKER_SCALE + QUIXEL_COLOR_PICKER_SCALE, QUIXEL_COLOR_PICKER_SCALE, 0, 0, 0, 0, NULL, NULL, NULL);
		t3gui_dialog_add_element(uip->dialog[QUIXEL_UI_DIALOG_MAIN], NULL, t3gui_slider_proc, 0, 32 + QUIXEL_COLOR_PICKER_SCALE, QUIXEL_COLOR_PICKER_SHADES * QUIXEL_COLOR_PICKER_SCALE + QUIXEL_COLOR_PICKER_SCALE - QUIXEL_COLOR_PICKER_SCALE, QUIXEL_COLOR_PICKER_SCALE, 0, 0, 1000, 0, NULL, NULL, NULL);
		t3gui_dialog_add_element(uip->dialog[QUIXEL_UI_DIALOG_MAIN], NULL, t3gui_slider_proc, 0, 32 + QUIXEL_COLOR_PICKER_SCALE + QUIXEL_COLOR_PICKER_SCALE, QUIXEL_COLOR_PICKER_SHADES * QUIXEL_COLOR_PICKER_SCALE + QUIXEL_COLOR_PICKER_SCALE - QUIXEL_COLOR_PICKER_SCALE, QUIXEL_COLOR_PICKER_SCALE, 0, 0, 1000, 0, NULL, NULL, NULL);
		t3gui_dialog_add_element(uip->dialog[QUIXEL_UI_DIALOG_MAIN], NULL, quixel_gui_palette_proc, 0, 100, left_panel_width, t3f_default_view->height, 0, 0, 0, 0, NULL, NULL, NULL);
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
