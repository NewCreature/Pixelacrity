#include "instance.h"
#include "canvas.h"
#include "undo.h"

int quixel_menu_edit_undo(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;
	char undo_path[4096];

	if(app->canvas_editor->undo_count > 0)
	{
		strcpy(undo_path, quixel_get_undo_path("undo", app->canvas_editor->undo_count - 1));
		quixel_apply_undo(app->canvas_editor, undo_path, false);
		al_remove_filename(undo_path);
		app->canvas_editor->undo_count--;
		t3f_refresh_menus();
	}

	return 0;
}

int quixel_menu_edit_redo(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;
	char redo_path[4096];

	if(app->canvas_editor->redo_count > 0)
	{
		strcpy(redo_path, quixel_get_undo_path("redo", app->canvas_editor->redo_count - 1));
		quixel_apply_redo(app->canvas_editor, redo_path);
		al_remove_filename(redo_path);
		app->canvas_editor->redo_count--;
		t3f_refresh_menus();
	}

	return 0;
}

int quixel_menu_edit_cut(int id, void * data)
{
	return 0;
}

int quixel_menu_edit_copy(int id, void * data)
{
	return 0;
}

int quixel_menu_edit_paste(int id, void * data)
{
	return 0;
}

int quixel_menu_edit_delete_layer(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	if(!quixel_remove_canvas_layer(app->canvas, app->canvas_editor->current_layer))
	{
		printf("Failed to remove layer!\n");
	}
	if(app->canvas_editor->current_layer >= app->canvas->layer_max)
	{
		app->canvas_editor->current_layer = app->canvas->layer_max - 1;
	}
	if(app->canvas_editor->current_layer < 0)
	{
		app->canvas_editor->current_layer = 0;
	}
	return 0;
}
