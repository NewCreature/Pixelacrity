#include "t3f/t3f.h"
#include "ui.h"
#include "canvas.h"
#include "canvas_file.h"

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

static int get_config_val(ALLEGRO_CONFIG * cp, const char * section, const char * key, int default_val)
{
	const char * val;

	val = al_get_config_value(cp, section, key);
	if(val)
	{
		return atoi(val);
	}
	return default_val;
}

QUIXEL_UI * quixel_create_ui(void)
{
	QUIXEL_UI * uip;
	char buf[1024];

	uip = malloc(sizeof(QUIXEL_UI));
	if(uip)
	{
		memset(uip, 0, sizeof(QUIXEL_UI));

		uip->bitmap[QUIXEL_UI_BITMAP_BG] = make_checkerboard_bitmap(t3f_color_white, al_map_rgba_f(0.9, 0.9, 0.9, 1.0));
		if(!uip->bitmap[QUIXEL_UI_BITMAP_BG])
		{
			goto error;
		}

		uip->dialog[QUIXEL_UI_DIALOG_MAIN] = t3gui_create_dialog();
		if(!uip->dialog[QUIXEL_UI_DIALOG_MAIN])
		{
			goto error;
		}
		t3gui_dialog_add_element(uip->dialog[QUIXEL_UI_DIALOG_MAIN], NULL, t3gui_box_proc, 640 - 64, 0, 64, 480, 0, 0, 0, 0, NULL, NULL, NULL);
		t3gui_dialog_add_element(uip->dialog[QUIXEL_UI_DIALOG_MAIN], NULL, t3gui_button_proc, 640 - 64, 0, 64, 32, 0, 0, 0, 0, "Pixel", NULL, NULL);
		t3gui_dialog_add_element(uip->dialog[QUIXEL_UI_DIALOG_MAIN], NULL, t3gui_button_proc, 640 - 64, 32, 64, 32, 0, 0, 0, 0, "Line", NULL, NULL);
		t3gui_dialog_add_element(uip->dialog[QUIXEL_UI_DIALOG_MAIN], NULL, t3gui_button_proc, 640 - 64, 64, 64, 32, 0, 0, 0, 0, "Rectangle", NULL, NULL);
		t3gui_dialog_add_element(uip->dialog[QUIXEL_UI_DIALOG_MAIN], NULL, t3gui_button_proc, 640 - 64, 96, 64, 32, 0, 0, 0, 0, "FRectangle", NULL, NULL);
		t3gui_dialog_add_element(uip->dialog[QUIXEL_UI_DIALOG_MAIN], NULL, t3gui_button_proc, 640 - 64, 128, 64, 32, 0, 0, 0, 0, "Oval", NULL, NULL);
		t3gui_dialog_add_element(uip->dialog[QUIXEL_UI_DIALOG_MAIN], NULL, t3gui_button_proc, 640 - 64, 160, 64, 32, 0, 0, 0, 0, "FOval", NULL, NULL);
		t3gui_show_dialog(uip->dialog[QUIXEL_UI_DIALOG_MAIN], NULL, T3GUI_PLAYER_NO_ESCAPE, uip);

		t3f_get_filename(t3f_data_path, "last.qcanvas", buf, 1024);
		uip->canvas = quixel_load_canvas(buf, 2048);
		if(!uip->canvas)
		{
			printf("failed to load previous work\n");
			uip->canvas = quixel_create_canvas(2048);
		}
		if(!uip->canvas)
		{
			goto error;
		}
		uip->current_layer = get_config_val(uip->canvas->config, "state", "current_layer", 0);
		uip->view_x = get_config_val(uip->canvas->config, "state", "view_x", uip->canvas->bitmap_size * 8 + 1024);
		uip->view_y = get_config_val(uip->canvas->config, "state", "view_y", uip->canvas->bitmap_size * 8 + 1024);
		uip->view_zoom = get_config_val(uip->canvas->config, "state", "view_zoom", 8);

		uip->bg_scale = 24;
	}
	return uip;

	error:
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
	for(i = 0; i < QUIXEL_UI_MAX_MENUS; i++)
	{
		if(uip->menu[i])
		{
			al_destroy_menu(uip->menu[i]);
		}
	}
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
	quixel_destroy_canvas(uip->canvas);
	free(uip);
}

static void draw_pixel(QUIXEL_UI * uip, int x, int y, ALLEGRO_COLOR color)
{
	ALLEGRO_TRANSFORM identity;
	ALLEGRO_STATE old_state;

	al_store_state(&old_state, ALLEGRO_STATE_TARGET_BITMAP | ALLEGRO_STATE_TRANSFORM);
	al_set_target_bitmap(uip->canvas->layer[uip->current_layer]->bitmap[y / uip->canvas->bitmap_size][x / uip->canvas->bitmap_size]);
	al_identity_transform(&identity);
	al_draw_pixel(x % uip->canvas->bitmap_size, y % uip->canvas->bitmap_size, color);
	al_restore_state(&old_state);
}

void quixel_process_ui(QUIXEL_UI * uip)
{
	t3gui_logic();
	uip->hover_x = uip->view_x + t3f_mouse_x / uip->view_zoom;
	uip->hover_y = uip->view_y + t3f_mouse_y / uip->view_zoom;
	if(t3f_key[ALLEGRO_KEY_LEFT])
	{
		uip->view_x--;
	}
	if(t3f_key[ALLEGRO_KEY_RIGHT])
	{
		uip->view_x++;
	}
	if(t3f_key[ALLEGRO_KEY_UP])
	{
		uip->view_y--;
	}
	if(t3f_key[ALLEGRO_KEY_DOWN])
	{
		uip->view_y++;
	}
	if(t3f_key[ALLEGRO_KEY_MINUS])
	{
		if(uip->view_zoom > 1)
		{
			uip->view_zoom--;
			uip->view_x -= (uip->view_x + t3f_mouse_x / uip->view_zoom) - uip->hover_x;
			uip->view_y -= (uip->view_y + t3f_mouse_y / uip->view_zoom) - uip->hover_y;
		}
		t3f_key[ALLEGRO_KEY_MINUS] = 0;
	}
	if(t3f_key[ALLEGRO_KEY_EQUALS])
	{
		uip->view_zoom++;
		uip->view_x -= (uip->view_x + t3f_mouse_x / uip->view_zoom) - uip->hover_x;
		uip->view_y -= (uip->view_y + t3f_mouse_y / uip->view_zoom) - uip->hover_y;
		t3f_key[ALLEGRO_KEY_EQUALS] = 0;
	}
	if(t3f_key[ALLEGRO_KEY_PGUP])
	{
		uip->current_layer++;
		t3f_key[ALLEGRO_KEY_PGUP] = 0;
	}
	if(t3f_key[ALLEGRO_KEY_PGDN])
	{
		if(uip->current_layer > 0)
		{
			uip->current_layer--;
		}
		t3f_key[ALLEGRO_KEY_PGDN] = 0;
	}
	if(t3f_key[ALLEGRO_KEY_DELETE])
	{
		if(!quixel_remove_canvas_layer(uip->canvas, uip->current_layer))
		{
			printf("Failed to remove layer!\n");
		}
		if(uip->current_layer >= uip->canvas->layer_max)
		{
			uip->current_layer = uip->canvas->layer_max - 1;
		}
		if(uip->current_layer < 0)
		{
			uip->current_layer = 0;
		}
		t3f_key[ALLEGRO_KEY_DELETE] = 0;
	}
	if(!quixel_expand_canvas(uip->canvas, uip->current_layer, uip->view_x, uip->view_y))
	{
		printf("Tried to expand canvas but failed!\n");
	}
	if(t3f_mouse_button[0])
	{
		draw_pixel(uip, uip->hover_x, uip->hover_y, al_map_rgba_f(uip->current_layer == 0 ? 1.0 : 0.0, uip->current_layer == 1 ? 1.0 : 0.0, uip->current_layer == 2 ? 1.0 : 0.0, 1.0));
	}
}

void quixel_render_ui(QUIXEL_UI * uip)
{
	ALLEGRO_STATE old_state;
	ALLEGRO_TRANSFORM identity;
	int i, j;
	int tw, th;

	tw = al_get_display_width(t3f_display) / uip->bg_scale + 1;
	th = al_get_display_height(t3f_display) / uip->bg_scale + 1;
	al_store_state(&old_state, ALLEGRO_STATE_TRANSFORM);
	al_identity_transform(&identity);
	al_use_transform(&identity);
	for(i = 0; i < th; i++)
	{
		for(j = 0; j < tw; j++)
		{
			t3f_draw_scaled_bitmap(uip->bitmap[QUIXEL_UI_BITMAP_BG], t3f_color_white, j * uip->bg_scale, i * uip->bg_scale, 0, uip->bg_scale, uip->bg_scale, 0);
		}
	}
	quixel_render_canvas(uip->canvas, uip->view_x, uip->view_y, al_get_display_width(t3f_display), al_get_display_height(t3f_display), uip->view_zoom);
	al_restore_state(&old_state);
	t3gui_render();
}
