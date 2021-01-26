#include "t3f/t3f.h"
#include "instance.h"
#include "ui.h"

static bool close_canvas(QUIXEL_UI * uip)
{
	if(uip->canvas)
	{
		/* insert code to check for changes and ask to save */

		quixel_destroy_canvas(uip->canvas);
		uip->canvas = NULL;
		return true;
	}
	return true;
}

int quixel_menu_file_new_helper(int id, void * data)
{
	QUIXEL_UI * uip = (QUIXEL_UI *)data;

	if(close_canvas(uip))
	{
		uip->canvas = quixel_create_canvas(2048);
	}
	return 0;
}

int quixel_menu_file_new(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	return quixel_menu_file_new_helper(id, app->ui);
}

int quixel_menu_file_load(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	if(close_canvas(app->ui))
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
	t3f_exit();
	return 0;
}
