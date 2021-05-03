#include "instance.h"
#include "modules/canvas/canvas.h"

int quixel_menu_layer_add(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	if(quixel_add_canvas_layer(app->canvas))
	{
		app->canvas_editor->current_layer = app->canvas->layer_max - 1;
		t3f_refresh_menus();
	}
	return 0;
}

int quixel_menu_layer_delete(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	if(app->canvas->layer_max > 0 && app->canvas_editor->current_layer < app->canvas->layer_max)
	{
		quixel_remove_canvas_layer(app->canvas, app->canvas_editor->current_layer);
		if(app->canvas_editor->current_layer >= app->canvas->layer_max)
		{
			app->canvas_editor->current_layer = app->canvas->layer_max - 1;
			if(app->canvas_editor->current_layer < 0)
			{
				app->canvas_editor->current_layer = 0;
			}
		}
	}
	return 0;
}

int quixel_menu_layer_previous(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	app->canvas_editor->current_layer--;
	if(app->canvas_editor->current_layer < 0)
	{
		app->canvas_editor->current_layer = app->canvas->layer_max - 1;
		if(app->canvas_editor->current_layer < 0)
		{
			app->canvas_editor->current_layer = 0;
		}
	}
	return 0;
}

int quixel_menu_layer_next(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	app->canvas_editor->current_layer++;
	if(app->canvas_editor->current_layer >= app->canvas->layer_max)
	{
		app->canvas_editor->current_layer = 0;
	}
	return 0;
}
