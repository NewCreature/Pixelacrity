#include "instance.h"
#include "modules/canvas/canvas.h"
#include "ui/canvas_editor/undo.h"

int quixel_menu_edit_undo(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;
	char undo_path[1024];

	if(app->canvas_editor->undo_count > 0)
	{
		quixel_get_undo_path("undo", app->canvas_editor->undo_count - 1, undo_path, 1024);
		quixel_apply_undo(app->canvas_editor, undo_path, false);
		al_remove_filename(undo_path);
		app->canvas_editor->undo_count--;
		quixel_update_undo_name(app->canvas_editor);
		quixel_update_redo_name(app->canvas_editor);
		t3f_refresh_menus();
	}

	return 0;
}

int quixel_menu_edit_redo(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;
	char redo_path[1024];

	if(app->canvas_editor->redo_count > 0)
	{
		quixel_get_undo_path("redo", app->canvas_editor->redo_count - 1, redo_path, 1024);
		quixel_apply_redo(app->canvas_editor, redo_path);
		al_remove_filename(redo_path);
		app->canvas_editor->redo_count--;
		quixel_update_undo_name(app->canvas_editor);
		quixel_update_redo_name(app->canvas_editor);
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
