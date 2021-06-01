#include "instance.h"
#include "modules/canvas/canvas.h"
#include "ui/canvas_editor/undo.h"
#include "ui/canvas_editor/undo_layer.h"

int quixel_menu_layer_add(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;
	char undo_path[1024];

	quixel_get_undo_path("undo", app->canvas_editor->undo_count, undo_path, 1024);
	if(quixel_make_add_layer_undo(app->canvas_editor, undo_path))
	{
		app->canvas_editor->undo_count++;
		app->canvas_editor->redo_count = 0;
		quixel_update_undo_name(app->canvas_editor);
		quixel_update_redo_name(app->canvas_editor);
	}
	if(quixel_add_canvas_layer(app->canvas, -1))
	{
		app->canvas_editor->current_layer = app->canvas->layer_max - 1;
		t3f_refresh_menus();
	}
	return 0;
}

int quixel_menu_layer_delete(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;
	char undo_path[1024];

	quixel_get_undo_path("undo", app->canvas_editor->undo_count, undo_path, 1024);
	if(quixel_make_remove_layer_undo(app->canvas_editor, app->canvas_editor->current_layer, undo_path))
	{
		app->canvas_editor->undo_count++;
		app->canvas_editor->redo_count = 0;
		quixel_update_undo_name(app->canvas_editor);
		quixel_update_redo_name(app->canvas_editor);
	}
	if(app->canvas->layer_max > 1 && app->canvas_editor->current_layer < app->canvas->layer_max)
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
		t3f_refresh_menus();
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
