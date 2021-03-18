#include "t3f/t3f.h"
#include "t3f/file_utils.h"
#include "instance.h"
#include "ui.h"
#include "canvas_file.h"
#include "canvas_helpers.h"
#include "menu_file_proc.h"

static bool close_canvas(APP_INSTANCE * app)
{
	if(app->canvas)
	{
		/* insert code to check for changes and ask to save */

		quixel_destroy_canvas(app->canvas);
		app->canvas = NULL;
		return true;
	}
	return true;
}

int quixel_menu_file_new(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	if(close_canvas(app))
	{
		app->canvas = quixel_create_canvas(2048);
	}
	return 0;
}

static QUIXEL_CANVAS * canvas_from_image(const char * fn)
{
	QUIXEL_CANVAS * cp = NULL;
	ALLEGRO_BITMAP * bp = NULL;
	int cx, cy;
	int x, y;

	cp = quixel_create_canvas(2048);
	if(!cp)
	{
		goto fail;
	}
	bp = al_load_bitmap_flags(fn, ALLEGRO_NO_PREMULTIPLIED_ALPHA);
	if(!bp)
	{
		goto fail;
	}
	cx = (QUIXEL_CANVAS_MAX_WIDTH * 2048) / 2;
	cy = (QUIXEL_CANVAS_MAX_HEIGHT * 2048) / 2;
	x = cx - al_get_bitmap_width(bp) / 2;
	y = cy - al_get_bitmap_height(bp) / 2;
	quixel_import_bitmap_to_canvas(cp, bp, 0, x, y);
	quixel_add_canvas_frame(cp, "Frame 1", x, y, al_get_bitmap_width(bp), al_get_bitmap_height(bp));
	al_destroy_bitmap(bp);

	return cp;

	fail:
	{
		if(bp)
		{
			al_destroy_bitmap(bp);
		}
		if(cp)
		{
			quixel_destroy_canvas(cp);
		}
		return NULL;
	}
}

int quixel_menu_file_load(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;
	ALLEGRO_FILECHOOSER * file_chooser;
	const char * file_path;
	const char * extension;
	bool import_image = false;

	if(close_canvas(app))
	{
		file_chooser = al_create_native_file_dialog(NULL, "Choose canvas or image file...", "*.qcanvas;*.png;*.tga;*.pcx;*.bmp;*.jpg", ALLEGRO_FILECHOOSER_FILE_MUST_EXIST);
		if(file_chooser)
		{
			al_stop_timer(t3f_timer);
			if(al_show_native_file_dialog(t3f_display, file_chooser))
			{
				if(al_get_native_file_dialog_count(file_chooser) > 0)
				{
					file_path = al_get_native_file_dialog_path(file_chooser, 0);
					if(file_path)
					{
						extension = t3f_get_path_extension(file_path);
						if(extension)
						{
							if(strcasecmp(extension, ".qcanvas"))
							{
								import_image = true;
							}
						}
						if(import_image)
						{
							app->canvas = canvas_from_image(file_path);
						}
						else
						{
							app->canvas = quixel_load_canvas(file_path, 2048);
						}
						if(app->canvas)
						{
							app->canvas_editor->canvas = app->canvas;
							quixel_center_canvas_editor(app->canvas_editor, 0);
							strcpy(app->canvas_editor->canvas_path, file_path);
							app->canvas_editor->update_title = true;
						}
						else
						{
							printf("Unable to load!\n");
							app->canvas = quixel_create_canvas(2048);
							if(app->canvas)
							{
								app->canvas_editor->canvas = app->canvas;
							}
						}
					}
				}
			}
			al_destroy_native_file_dialog(file_chooser);
			al_start_timer(t3f_timer);
		}
	}
	return 0;
}

static bool resave_allowed(QUIXEL_CANVAS_EDITOR * cep)
{
	int offset_x, offset_y, width, height;
	const char * extension = t3f_get_path_extension(cep->canvas_path);

	if(!strcasecmp(extension, ".qcanvas"))
	{
		return false;
	}
	if(cep->canvas->frame_max == 1 && cep->canvas->layer_max < 2)
	{
		if(!strcasecmp(extension, ".png") || !strcasecmp(extension, ".tga"))
		{
			quixel_get_canvas_dimensions(cep->canvas, &offset_x, &offset_y, &width, &height, 0);
			if(offset_x >= cep->canvas->frame[0]->box.start_x && offset_x + width <= cep->canvas->frame[0]->box.start_x + cep->canvas->frame[0]->box.width && offset_y >= cep->canvas->frame[0]->box.start_y && offset_y + height <= cep->canvas->frame[0]->box.start_y + cep->canvas->frame[0]->box.height)
			{
				return true;
			}
		}
	}
	return false;
}

int quixel_menu_file_save(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;
	ALLEGRO_BITMAP * bp;

	if(resave_allowed(app->canvas_editor))
	{
		bp = quixel_get_bitmap_from_canvas(app->canvas, 0, app->canvas->layer_max, 0);
		if(bp)
		{
			al_save_bitmap(app->canvas_editor->canvas_path, bp);
			al_destroy_bitmap(bp);
			app->canvas_editor->modified = false;
			app->canvas_editor->update_title = true;
		}
	}
	else
	{
		if(strcasecmp(t3f_get_path_extension(app->canvas_editor->canvas_path), ".qcanvas"))
		{
			quixel_menu_file_save_as(id, data);
		}
		else
		{
			if(quixel_save_canvas(app->canvas, app->canvas_editor->canvas_path, ".png", QUIXEL_CANVAS_SAVE_AUTO))
			{
				app->canvas_editor->modified = false;
				app->canvas_editor->update_title = true;
			}
		}
	}
	return 0;
}

int quixel_menu_file_save_as(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;
	ALLEGRO_FILECHOOSER * file_chooser;
	const char * file_path;
	ALLEGRO_PATH * path;

	file_chooser = al_create_native_file_dialog(NULL, "Save canvas as...", "*.qcanvas", ALLEGRO_FILECHOOSER_SAVE);
	if(file_chooser)
	{
		al_stop_timer(t3f_timer);
		if(al_show_native_file_dialog(t3f_display, file_chooser))
		{
			if(al_get_native_file_dialog_count(file_chooser) > 0)
			{
				file_path = al_get_native_file_dialog_path(file_chooser, 0);
				if(file_path)
				{
					path = al_create_path(file_path);
					if(path)
					{
						al_set_path_extension(path, ".qcanvas");
						strcpy(app->canvas_editor->canvas_path, al_path_cstr(path, '/'));
						quixel_menu_file_save(id, data);
						al_destroy_path(path);
					}
				}
			}
		}
		al_destroy_native_file_dialog(file_chooser);
		al_start_timer(t3f_timer);
	}
	return 0;
}

int quixel_menu_file_import(int id, void * data)
{
	return 0;
}

int quixel_menu_file_export(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;
	ALLEGRO_FILECHOOSER * file_chooser;
	const char * file_path;
	const char * extension;
	ALLEGRO_PATH * path;
	ALLEGRO_BITMAP * bp;

	file_chooser = al_create_native_file_dialog(NULL, "Export canvas to image file...", "*.png;*.tga;*.pcx;*.bmp;*.jpg", ALLEGRO_FILECHOOSER_SAVE);
	if(file_chooser)
	{
		al_stop_timer(t3f_timer);
		if(al_show_native_file_dialog(t3f_display, file_chooser))
		{
			if(al_get_native_file_dialog_count(file_chooser) > 0)
			{
				file_path = al_get_native_file_dialog_path(file_chooser, 0);
				if(file_path)
				{
					path = al_create_path(file_path);
					if(path)
					{
						extension = al_get_path_extension(path);
						if(strcasecmp(extension, ".png") && strcasecmp(extension, ".pcx") && strcasecmp(extension, ".tga") && strcasecmp(extension, ".bmp") && strcasecmp(extension, ".jpg"))
						{
							al_set_path_extension(path, ".png");
						}
						bp = quixel_get_bitmap_from_canvas(app->canvas, 0, app->canvas->layer_max, 0);
						if(bp)
						{
							al_save_bitmap(al_path_cstr(path, '/'), bp);
							al_destroy_bitmap(bp);
						}
						al_destroy_path(path);
					}
				}
			}
		}
		al_destroy_native_file_dialog(file_chooser);
		al_start_timer(t3f_timer);
	}
	return 0;
}

int quixel_menu_file_exit(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	if(close_canvas(app))
	{
		t3f_exit();
	}
	return 0;
}
