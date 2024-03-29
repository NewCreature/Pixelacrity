#include "t3f/t3f.h"
#include "t3f/file_utils.h"
#include "instance.h"
#include "ui/ui.h"
#include "ui/window.h"
#include "modules/canvas/canvas_file.h"
#include "modules/canvas/canvas_helpers.h"
#include "modules/bitmap.h"
#include "file.h"
#include "file_proc.h"
#include "ui/canvas_editor/undo/undo.h"
#include "ui/canvas_editor/undo/import.h"
#include "defines.h"

static int close_canvas(APP_INSTANCE * app)
{
	int ret = 0;
	int button;

	t3f_debug_message("Enter close_canvas()\n");
	if(app->canvas)
	{
		/* insert code to check for changes and ask to save */
		if(app->canvas_editor->modified)
		{
			al_stop_timer(t3f_timer);
			button = al_show_native_message_box(t3f_display, "Unsaved Work", "", "You have unsaved work. What do you want to do?", "Save|Discard|Cancel", ALLEGRO_MESSAGEBOX_QUESTION);
			switch(button)
			{
				case 1:
				{
					ret = pa_menu_file_save(0, app);
					break;
				}
				case 2:
				{
					break;
				}
				case 3:
				{
					ret = 1;
					break;
				}
			}
			al_start_timer(t3f_timer);
		}
	}
	t3f_debug_message("Exit close_canvas()\n");
	return ret;
}

int pa_menu_file_new(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	t3f_debug_message("Enter pa_menu_file_new()\n");
	if(!close_canvas(app))
	{
		if(app->canvas)
		{
			pa_destroy_canvas(app->canvas);
		}
		app->canvas = pa_create_canvas(2048);
		if(!app->canvas)
		{
			t3f_debug_message("Error creating new canvas!\n");
			t3f_exit();
			return 0;
		}
		if(!pa_add_canvas_layer(app->canvas, -1))
		{
			t3f_debug_message("Error adding layer to new canvas!\n");
			t3f_exit();
			return 0;
		}
		if(app->canvas_editor)
		{
			pa_resave_canvas_editor_state(app->canvas_editor);
			pa_reset_canvas_editor(app->canvas_editor);
			app->canvas_editor->canvas = app->canvas;
			pa_set_window_message(NULL);
			pa_center_canvas_editor(app->canvas_editor, 0);
		}
	}
	t3f_debug_message("Exit pa_menu_file_new()\n");
	return 0;
}

static PA_CANVAS * canvas_from_image(const char * fn)
{
	PA_CANVAS * cp = NULL;
	ALLEGRO_BITMAP * bp = NULL;
	int cx, cy;
	int x, y;

	t3f_debug_message("Enter canvas_from_image()\n");
	cp = pa_create_canvas(2048);
	if(!cp)
	{
		goto fail;
	}
	if(!pa_add_canvas_layer(cp, -1))
	{
		goto fail;
	}
	bp = al_load_bitmap_flags(fn, ALLEGRO_NO_PREMULTIPLIED_ALPHA);
	if(!bp)
	{
		goto fail;
	}
	cx = 2048 / 2;
	cy = 2048 / 2;
	x = cx - al_get_bitmap_width(bp) / 2;
	y = cy - al_get_bitmap_height(bp) / 2;
	pa_import_bitmap_to_canvas(cp, bp, 0, x, y);
	while(x < 0)
	{
		x += cp->bitmap_size;
	}
	while(y < 0)
	{
		y += cp->bitmap_size;
	}
	pa_add_canvas_frame(cp, "Frame 1", x, y, al_get_bitmap_width(bp), al_get_bitmap_height(bp));
	al_destroy_bitmap(bp);

	t3f_debug_message("Exit canvas_from_image()\n");
	return cp;

	fail:
	{
		t3f_debug_message("Failed to load canvas from image\n");
		if(bp)
		{
			al_destroy_bitmap(bp);
		}
		if(cp)
		{
			pa_destroy_canvas(cp);
		}
		return NULL;
	}
}

static void remove_recent_file(int item)
{
	const char * val;
	char buf[256];
	int i, c;

	val = al_get_config_value(t3f_config, "Settings", "max_recent_files");
	if(val)
	{
		c = atoi(val);
		for(i = item; i < c; i++)
		{
			sprintf(buf, "recent_file_%d", i + 1);
			val = al_get_config_value(t3f_config, "App Data", buf);
			sprintf(buf, "recent_file_%d", i);
			if(val)
			{
				al_set_config_value(t3f_config, "App Data", buf, val ? val : "");
			}
			else
			{
				al_remove_config_key(t3f_config, "App Data", buf);
			}
		}
	}
}

static void add_recent_file(const char * fn)
{
	const char * val;
	char buf[256];
	int i, c;

	val = al_get_config_value(t3f_config, "Settings", "max_recent_files");
	if(val)
	{
		c = atoi(val);
		for(i = c - 1; i > 0; i--)
		{
			sprintf(buf, "recent_file_%d", i - 1);
			val = al_get_config_value(t3f_config, "App Data", buf);
			sprintf(buf, "recent_file_%d", i);
			if(val)
			{
				al_set_config_value(t3f_config, "App Data", buf, val);
			}
		}
		al_set_config_value(t3f_config, "App Data", "recent_file_0", fn);
	}
}

static void update_recent_list(const char * fn)
{
	const char * val;
	char buf[256];
	int i, c;

	val = al_get_config_value(t3f_config, "Settings", "max_recent_files");
	if(val)
	{
		c = atoi(val);
		/* remove fn from list if it exists */
		for(i = c - 1; i >= 0; i--)
		{
			sprintf(buf, "recent_file_%d", i);
			val = al_get_config_value(t3f_config, "App Data", buf);
			if(val && !strcmp(val, fn))
			{
				remove_recent_file(i);
			}
		}
		/* add file to top of list */
		add_recent_file(fn);
	}
}

bool pa_handle_load_canvas(APP_INSTANCE * app, const char * file_path)
{
	const char * extension;
	bool import_image = false;
	PA_CANVAS * new_canvas = NULL;
	ALLEGRO_PATH * pp;

	extension = t3f_get_path_extension(file_path);
	if(extension)
	{
		if(strcasecmp(extension, PA_CANVAS_FILE_EXTENSION))
		{
			import_image = true;
		}
	}
	if(import_image)
	{
		new_canvas = canvas_from_image(file_path);
	}
	else
	{
		new_canvas = pa_load_canvas(file_path, 2048);
	}
	if(new_canvas)
	{
		pa_resave_canvas_editor_state(app->canvas_editor);
		if(app->canvas)
		{
			pa_destroy_canvas(app->canvas);
		}
		app->canvas = new_canvas;
		app->canvas_editor->canvas = app->canvas;
		app->canvas_editor->modified = 0;
		app->canvas_editor->preview->update = true;
		pa_reset_canvas_editor(app->canvas_editor);
		pa_center_canvas_editor(app->canvas_editor, 0);
		strcpy(app->canvas_editor->canvas_path, file_path);
		pa_set_window_message(NULL);
		pp = al_create_path(file_path);
		if(pp)
		{
			al_set_path_extension(pp, ".ini");
			if(!import_image)
			{
				pa_load_canvas_editor_state(app->canvas_editor, al_path_cstr(pp, '/'));
			}
			al_set_path_filename(pp, "");
			al_set_config_value(t3f_config, "App Data", "last_canvas_path", al_path_cstr(pp, '/'));
			al_destroy_path(pp);
		}
		pa_optimize_canvas(app->canvas_editor, 0, 0);
		update_recent_list(file_path);
		pa_update_recent_menu(app->ui);
		t3f_refresh_menus();
		return true;
	}
	return false;
}

int pa_menu_file_load(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;
	ALLEGRO_FILECHOOSER * file_chooser;
	const char * file_path;
	const char * val;
	char * recent_file_path;

	t3f_debug_message("Enter pa_menu_file_load()\n");
	val = al_get_config_value(t3f_config, "App Data", "last_canvas_path");
	if(!close_canvas(app))
	{
		if(app->ui->load_recent_fn)
		{
			recent_file_path = strdup(app->ui->load_recent_fn);
			if(recent_file_path)
			{
				if(!pa_handle_load_canvas(app, recent_file_path))
				{
					printf("unable to load\n");
				}
				free(recent_file_path);
			}
			app->ui->load_recent_fn = NULL;
		}
		else
		{
			file_chooser = al_create_native_file_dialog(val, "Choose canvas or image file...", "*" PA_CANVAS_FILE_EXTENSION ";*.png;*.tga;*.pcx;*.bmp;*.jpg", ALLEGRO_FILECHOOSER_FILE_MUST_EXIST);
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
							if(!pa_handle_load_canvas(app, file_path))
							{
								printf("unable to load\n");
							}
						}
					}
				}
				al_destroy_native_file_dialog(file_chooser);
				al_start_timer(t3f_timer);
			}
		}
	}
	t3f_debug_message("Exit pa_menu_file_load()\n");
	return 0;
}

int pa_menu_file_save(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;
	int ret = -1;

	t3f_debug_message("Enter pa_menu_file_save()\n");
	if(strcasecmp(t3f_get_path_extension(app->canvas_editor->canvas_path), PA_CANVAS_FILE_EXTENSION))
	{
		ret = pa_menu_file_save_as(id, data);
	}
	else
	{
		pa_set_window_message("Saving...");
		if(pa_save_canvas(app->canvas, app->canvas_editor->canvas_path, ".png", PA_CANVAS_SAVE_AUTO))
		{
			app->canvas_editor->modified = 0;
		}
		ret = 0;
	}
	pa_resave_canvas_editor_state(app->canvas_editor);
	t3f_debug_message("Exit pa_menu_file_save()\n");
	return ret;
}

int pa_menu_file_save_as(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;
	ALLEGRO_FILECHOOSER * file_chooser;
	const char * file_path;
	ALLEGRO_PATH * path;
	const char * val;
	int ret = -1;

	t3f_debug_message("Enter pa_menu_file_save_as()\n");
	val = al_get_config_value(t3f_config, "App Data", "last_canvas_path");
	file_chooser = al_create_native_file_dialog(val, "Save canvas as...", "*" PA_CANVAS_FILE_EXTENSION, ALLEGRO_FILECHOOSER_SAVE);
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
						al_set_path_extension(path, PA_CANVAS_FILE_EXTENSION);
						strcpy(app->canvas_editor->canvas_path, al_path_cstr(path, '/'));
						pa_menu_file_save(id, data);
						al_set_path_filename(path, "");
						al_set_config_value(t3f_config, "App Data", "last_canvas_path", al_path_cstr(path, '/'));
						update_recent_list(app->canvas_editor->canvas_path);
						pa_update_recent_menu(app->ui);
						al_destroy_path(path);
					}
				}
				ret = 0;
			}
		}
		al_destroy_native_file_dialog(file_chooser);
		al_start_timer(t3f_timer);
	}
	t3f_debug_message("Exit pa_menu_file_save_as()\n");
	return ret;
}

static bool export(PA_CANVAS * cp, int x, int y, int width, int height, const char * fn, ALLEGRO_SHADER * shader)
{
	ALLEGRO_BITMAP * bp;
	ALLEGRO_STATE old_state;

	al_store_state(&old_state, ALLEGRO_STATE_NEW_BITMAP_PARAMETERS);
//	al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
	bp = al_create_bitmap(width, height);
	al_restore_state(&old_state);
	if(bp)
	{
		pa_render_canvas_to_bitmap(cp, 0, cp->layer_max, x, y, width, height, 0, bp, shader);
		pa_unpremultiply_bitmap_alpha(bp);
		al_save_bitmap(fn, bp);
		al_destroy_bitmap(bp);
		return true;
	}
	else
	{
		printf("Error exporting bitmap!\n");
	}
	return false;
}

int pa_menu_file_resave_image(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;
	int x, y, w, h;
	bool save_frame = false;

	if(pa_resave_allowed(app->canvas_editor))
	{
		pa_get_canvas_dimensions(app->canvas, &x, &y, &w, &h, 0, true);
		if(app->canvas->frame_max == 1)
		{
			if(w == 0 || h == 0)
			{
				save_frame = true;
			}
			else if(x >= app->canvas->frame[0]->box.start_x && y >= app->canvas->frame[0]->box.start_y && x + w <= app->canvas->frame[0]->box.start_x + app->canvas->frame[0]->box.width && y + h <= app->canvas->frame[0]->box.start_y + app->canvas->frame[0]->box.height)
			{
				save_frame = true;
			}
			if(save_frame)
			{
				x = app->canvas->frame[0]->box.start_x;
				y = app->canvas->frame[0]->box.start_y;
				w = app->canvas->frame[0]->box.width;
				h = app->canvas->frame[0]->box.height;
			}
		}
		pa_set_window_message("Saving...");
		if(export(app->canvas, x, y, w, h, app->canvas_editor->canvas_path, app->canvas_editor->premultiplied_alpha_shader))
		{
			app->canvas_editor->modified = 0;
		}
	}
	return 0;
}

static bool make_import_undo(PA_CANVAS_EDITOR * cep, const char * fn)
{
	char undo_path[1024];

	pa_get_undo_path("undo", cep->undo_count, undo_path, 1024);
	if(pa_make_import_undo(cep, cep->current_layer, fn, undo_path))
	{
		return true;
	}
	return false;
}

int pa_menu_file_import(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;
	ALLEGRO_FILECHOOSER * file_chooser = NULL;
	const char * file_path;
	ALLEGRO_PATH * pp;
	const char * val;

	t3f_debug_message("Enter pa_menu_file_import()\n");
	val = al_get_config_value(t3f_config, "App Data", "last_image_path");
	file_chooser = al_create_native_file_dialog(val, "Choose image file...", "*.png;*.tga;*.pcx;*.bmp;*.jpg", ALLEGRO_FILECHOOSER_FILE_MUST_EXIST);
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
					if(pa_import_image(app->canvas_editor, file_path))
					{
						if(make_import_undo(app->canvas_editor, file_path))
						{
							pa_finalize_undo(app->canvas_editor);
						}
						pp = al_create_path(file_path);
						if(pp)
						{
							al_set_path_filename(pp, "");
							al_set_config_value(t3f_config, "App Data", "last_image_path", al_path_cstr(pp, '/'));
							al_destroy_path(pp);
						}
						app->canvas_editor->modified++;
						pa_set_window_message(NULL);
					}
				}
			}
		}
		al_destroy_native_file_dialog(file_chooser);
		al_start_timer(t3f_timer);
	}
	t3f_debug_message("Exit pa_menu_file_import()\n");
	return 0;
}

int pa_menu_file_reexport(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;
	int x, y, w, h;
	const char * export_path;

	if(pa_can_reexport(app->canvas_editor))
	{
		if(app->canvas_editor->current_frame < app->canvas->frame_max)
		{
			export_path = app->canvas->frame[app->canvas_editor->current_frame]->export_path;
			x = app->canvas->frame[app->canvas_editor->current_frame]->box.start_x;
			y = app->canvas->frame[app->canvas_editor->current_frame]->box.start_y;
			w = app->canvas->frame[app->canvas_editor->current_frame]->box.width;
			h = app->canvas->frame[app->canvas_editor->current_frame]->box.height;
		}
		else
		{
			export_path = app->canvas_editor->export_path;
			pa_get_canvas_dimensions(app->canvas, &x, &y, &w, &h, 0, true);
		}
		if(export_path)
		{
			export(app->canvas, x, y, w, h, export_path, app->canvas_editor->premultiplied_alpha_shader);
		}
		else
		{
			pa_menu_file_export(id, data);
		}
	}
	return 0;
}

int pa_menu_file_reexport_all(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;
	int i;

	if(pa_can_reexport_all(app->canvas_editor))
	{
		for(i = 0; i < app->canvas->frame_max; i++)
		{
			if(app->canvas->frame[i]->export_path)
			{
				export(app->canvas, app->canvas->frame[i]->box.start_x, app->canvas->frame[i]->box.start_y, app->canvas->frame[i]->box.width, app->canvas->frame[i]->box.height, app->canvas->frame[i]->export_path, app->canvas_editor->premultiplied_alpha_shader);
			}
		}
	}
	return 0;
}

int pa_menu_file_export(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;
	ALLEGRO_FILECHOOSER * file_chooser;
	ALLEGRO_PATH * pp;
	const char * file_path;
	const char * extension;
	const char * val;

	t3f_debug_message("Enter pa_menu_file_export()\n");
	if(!pa_can_export(app->canvas_editor))
	{
		return 0;
	}
	val = al_get_config_value(t3f_config, "App Data", "last_export_path");
	file_chooser = al_create_native_file_dialog(val, "Export canvas to image file...", "*.png;*.tga;*.pcx;*.bmp;*.jpg", ALLEGRO_FILECHOOSER_SAVE);
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
					pp = al_create_path(file_path);
					if(pp)
					{
						extension = al_get_path_extension(pp);
						if(strcasecmp(extension, ".png") && strcasecmp(extension, ".pcx") && strcasecmp(extension, ".tga") && strcasecmp(extension, ".bmp") && strcasecmp(extension, ".jpg"))
						{
							al_set_path_extension(pp, ".png");
						}
						if(app->canvas_editor->current_frame < app->canvas->frame_max)
						{
							app->canvas->frame[app->canvas_editor->current_frame]->export_path = strdup(al_path_cstr(pp, '/'));
						}
						else
						{
							app->canvas_editor->export_path = strdup(al_path_cstr(pp, '/'));
						}
						al_set_path_filename(pp, "");
						al_set_config_value(t3f_config, "App Data", "last_export_path", al_path_cstr(pp, '/'));
						al_destroy_path(pp);
						t3f_refresh_menus();
					}
					pa_menu_file_reexport(id, data);
					pa_resave_canvas_editor_state(app->canvas_editor);
				}
			}
		}
		al_destroy_native_file_dialog(file_chooser);
		al_start_timer(t3f_timer);
	}
	t3f_debug_message("Exit pa_menu_file_export()\n");
	return 0;
}

int pa_menu_file_exit(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	t3f_debug_message("Enter pa_menu_file_exit()\n");
	if(!close_canvas(app))
	{
		t3f_exit();
	}
	t3f_debug_message("Exit pa_menu_file_exit()\n");
	return 0;
}
