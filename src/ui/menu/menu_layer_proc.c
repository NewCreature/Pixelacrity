#include "instance.h"
#include "modules/canvas/canvas.h"
#include "ui/canvas_editor/undo.h"
#include "ui/canvas_editor/undo_layer.h"

int pa_menu_layer_add(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;
	char undo_path[1024];

	t3f_debug_message("Enter pa_menu_layer_add()\n");
	pa_get_undo_path("undo", app->canvas_editor->undo_count, undo_path, 1024);
	if(pa_make_add_layer_undo(app->canvas_editor, undo_path))
	{
		app->canvas_editor->undo_count++;
		app->canvas_editor->redo_count = 0;
		pa_update_undo_name(app->canvas_editor);
		pa_update_redo_name(app->canvas_editor);
	}
	if(pa_add_canvas_layer(app->canvas, -1))
	{
		app->canvas_editor->current_layer = app->canvas->layer_max - 1;
		app->canvas_editor->modified++;
		app->canvas_editor->update_title = true;
		t3f_refresh_menus();
	}
	t3f_debug_message("Exit pa_menu_layer_add()\n");
	return 0;
}

int pa_menu_layer_delete(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;
	char undo_path[1024];

	t3f_debug_message("Enter pa_menu_layer_delte()\n");
	pa_get_undo_path("undo", app->canvas_editor->undo_count, undo_path, 1024);
	if(pa_make_remove_layer_undo(app->canvas_editor, app->canvas_editor->current_layer, undo_path))
	{
		app->canvas_editor->undo_count++;
		app->canvas_editor->redo_count = 0;
		pa_update_undo_name(app->canvas_editor);
		pa_update_redo_name(app->canvas_editor);
	}
	if(app->canvas->layer_max > 1 && app->canvas_editor->current_layer < app->canvas->layer_max)
	{
		pa_remove_canvas_layer(app->canvas, app->canvas_editor->current_layer);
		if(app->canvas_editor->current_layer >= app->canvas->layer_max)
		{
			app->canvas_editor->current_layer = app->canvas->layer_max - 1;
			if(app->canvas_editor->current_layer < 0)
			{
				app->canvas_editor->current_layer = 0;
			}
		}
		app->canvas_editor->modified++;
		app->canvas_editor->update_title = true;
		t3f_refresh_menus();
	}
	t3f_debug_message("Exit pa_menu_layer_delete()\n");
	return 0;
}

int pa_menu_layer_move_up(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;
	PA_CANVAS_LAYER * old_layer;

	if(app->canvas_editor->current_layer < app->canvas->layer_max - 1)
	{
		old_layer = app->canvas->layer[app->canvas_editor->current_layer];
		app->canvas->layer[app->canvas_editor->current_layer] = app->canvas->layer[app->canvas_editor->current_layer + 1];
		app->canvas->layer[app->canvas_editor->current_layer + 1] = old_layer;
		app->canvas_editor->current_layer++;
		t3f_refresh_menus();
	}

	return 0;
}

int pa_menu_layer_move_down(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;
	PA_CANVAS_LAYER * old_layer;

	if(app->canvas_editor->current_layer > 0)
	{
		old_layer = app->canvas->layer[app->canvas_editor->current_layer];
		app->canvas->layer[app->canvas_editor->current_layer] = app->canvas->layer[app->canvas_editor->current_layer - 1];
		app->canvas->layer[app->canvas_editor->current_layer - 1] = old_layer;
		app->canvas_editor->current_layer--;
		t3f_refresh_menus();
	}
	return 0;
}

int pa_menu_layer_previous(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	t3f_debug_message("Enter pa_menu_layer_previous()\n");
	app->canvas_editor->current_layer--;
	if(app->canvas_editor->current_layer < 0)
	{
		app->canvas_editor->current_layer = app->canvas->layer_max - 1;
		if(app->canvas_editor->current_layer < 0)
		{
			app->canvas_editor->current_layer = 0;
		}
	}
	t3f_debug_message("Exit pa_menu_layer_previous()\n");
	return 0;
}

int pa_menu_layer_next(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	t3f_debug_message("Enter pa_menu_layer_next()\n");
	app->canvas_editor->current_layer++;
	if(app->canvas_editor->current_layer >= app->canvas->layer_max)
	{
		app->canvas_editor->current_layer = 0;
	}
	t3f_debug_message("Exit pa_menu_layer_next()\n");
	return 0;
}
