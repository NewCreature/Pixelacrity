#include "instance.h"
#include "modules/canvas/canvas.h"
#include "modules/canvas/canvas_helpers.h"
#include "ui/window.h"

int pa_menu_palette_reset(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	pa_load_palette(app->canvas_editor->palette, "data/default.pxp");

	return 0;
}

int pa_menu_palette_load(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;
	ALLEGRO_FILECHOOSER * file_chooser;
	const char * file_path;
	ALLEGRO_PATH * pp;

	file_chooser = al_create_native_file_dialog(NULL, "Load palette...", "*.pxp", ALLEGRO_FILECHOOSER_FILE_MUST_EXIST);
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
						al_set_config_value(t3f_config, "App Data", "last_palette_path", al_path_cstr(pp, '/'));
						pa_load_palette(app->canvas_editor->palette, al_path_cstr(pp, '/'));
						al_destroy_path(pp);
					}
				}
			}
		}
		al_destroy_native_file_dialog(file_chooser);
		al_start_timer(t3f_timer);
	}
	return 0;
}

int pa_menu_palette_save(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;
	ALLEGRO_FILECHOOSER * file_chooser;
	const char * file_path;
	ALLEGRO_PATH * pp;

	file_chooser = al_create_native_file_dialog(NULL, "Save palette...", "*.pxp", ALLEGRO_FILECHOOSER_SAVE);
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
						al_set_path_extension(pp, ".pxp");
						al_set_config_value(t3f_config, "App Data", "last_palette_path", al_path_cstr(pp, '/'));
						pa_save_palette(app->canvas_editor->palette, al_path_cstr(pp, '/'));
						al_destroy_path(pp);
					}
				}
			}
		}
		al_destroy_native_file_dialog(file_chooser);
		al_start_timer(t3f_timer);
	}

	return 0;
}
