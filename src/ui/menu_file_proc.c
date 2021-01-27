#include "t3f/t3f.h"
#include "instance.h"
#include "ui.h"

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

int quixel_menu_file_load(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	if(close_canvas(app))
	{

	}
	return 0;
}

int quixel_menu_file_save(int id, void * data)
{
	return 0;
}

int quixel_menu_file_save_as(int id, void * data)
{
	return 0;
}

int quixel_menu_file_import(int id, void * data)
{
	return 0;
}

int quixel_menu_file_export(int id, void * data)
{
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
