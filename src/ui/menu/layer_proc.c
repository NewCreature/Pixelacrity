#include "instance.h"
#include "modules/canvas/canvas.h"
#include "modules/canvas/canvas_helpers.h"
#include "ui/canvas_editor/undo/undo.h"
#include "ui/canvas_editor/undo/layer.h"
#include "ui/window.h"

int pa_menu_layer_add(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;
	char undo_path[1024];
	bool expand_selection_stack = false;
	bool expand_clipboard_stack = false;

	t3f_debug_message("Enter pa_menu_layer_add()\n");
	pa_get_undo_path("undo", app->canvas_editor->undo_count, undo_path, 1024);
	if(pa_make_add_layer_undo(app->canvas_editor, undo_path))
	{
		pa_finalize_undo(app->canvas_editor);
	}
	if(app->canvas_editor->selection.bitmap_stack && app->canvas_editor->selection.bitmap_stack->layers == app->canvas->layer_max)
	{
		expand_selection_stack = true;
	}
	if(app->canvas_editor->clipboard.bitmap_stack && app->canvas_editor->clipboard.bitmap_stack->layers == app->canvas->layer_max)
	{
		expand_clipboard_stack = true;
	}
	if(pa_add_canvas_layer(app->canvas, -1))
	{
		if(expand_selection_stack)
		{
			pa_add_layer_to_bitmap_stack(app->canvas_editor->selection.bitmap_stack, NULL, -1);
			app->canvas_editor->selection.layer_max++;
		}
		if(expand_clipboard_stack)
		{
			pa_add_layer_to_bitmap_stack(app->canvas_editor->clipboard.bitmap_stack, NULL, -1);
			app->canvas_editor->clipboard.layer_max--;
		}
		pa_select_canvas_editor_layer(app->canvas_editor, app->canvas_editor->canvas->layer_max - 1);
		app->canvas_editor->modified++;
		pa_set_window_message(NULL);
		t3f_refresh_menus();
	}
	t3f_debug_message("Exit pa_menu_layer_add()\n");
	return 0;
}

int pa_menu_layer_delete(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;
	char undo_path[1024];
	bool contract_selection_stack = false;
	bool contract_clipboard_stack = false;

	t3f_debug_message("Enter pa_menu_layer_delte()\n");
	if(app->canvas->layer_max > 1 && app->canvas_editor->current_layer < app->canvas->layer_max)
	{
		if(app->canvas_editor->selection.bitmap_stack && app->canvas_editor->selection.bitmap_stack->layers == app->canvas->layer_max)
		{
			contract_selection_stack = true;
		}
		if(app->canvas_editor->clipboard.bitmap_stack && app->canvas_editor->clipboard.bitmap_stack->layers == app->canvas->layer_max)
		{
			contract_clipboard_stack = true;
		}
		pa_get_undo_path("undo", app->canvas_editor->undo_count, undo_path, 1024);
		if(pa_make_remove_layer_undo(app->canvas_editor, app->canvas_editor->current_layer, undo_path))
		{
			pa_finalize_undo(app->canvas_editor);
		}
		pa_remove_canvas_layer(app->canvas, app->canvas_editor->current_layer);
		if(contract_selection_stack)
		{
			pa_remove_layer_from_bitmap_stack(app->canvas_editor->selection.bitmap_stack, app->canvas_editor->current_layer);
			app->canvas_editor->selection.layer_max--;
		}
		if(contract_clipboard_stack)
		{
			pa_remove_layer_from_bitmap_stack(app->canvas_editor->clipboard.bitmap_stack, app->canvas_editor->current_layer);
			app->canvas_editor->clipboard.layer_max--;
		}
		if(app->canvas_editor->current_layer >= app->canvas->layer_max)
		{
			pa_select_canvas_editor_layer(app->canvas_editor, app->canvas_editor->canvas->layer_max - 1);
			if(app->canvas_editor->current_layer < 0)
			{
				pa_select_canvas_editor_layer(app->canvas_editor, app->canvas_editor->canvas->layer_max - 1);
			}
		}
		app->canvas_editor->modified++;
		app->canvas_editor->preview->update = true;
		pa_set_window_message(NULL);
		t3f_refresh_menus();
	}
	t3f_debug_message("Exit pa_menu_layer_delete()\n");
	return 0;
}

int pa_menu_layer_move_up(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;
	char undo_path[1024];

	printf("up %d %d\n", app->canvas_editor->current_layer, app->canvas->layer_max - 1);
	if(app->canvas_editor->current_layer < app->canvas->layer_max - 1)
	{
		pa_get_undo_path("undo", app->canvas_editor->undo_count, undo_path, 1024);
		if(pa_make_swap_layer_undo(app->canvas_editor, app->canvas_editor->current_layer, app->canvas_editor->current_layer + 1, undo_path))
		{
			pa_finalize_undo(app->canvas_editor);
		}
		pa_swap_canvas_layer(app->canvas, app->canvas_editor->current_layer, app->canvas_editor->current_layer + 1);
		app->canvas_editor->current_layer++;
		t3f_refresh_menus();
	}

	return 0;
}

int pa_menu_layer_move_down(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;
	char undo_path[1024];

	if(app->canvas_editor->current_layer > 0)
	{
		pa_get_undo_path("undo", app->canvas_editor->undo_count, undo_path, 1024);
		if(pa_make_swap_layer_undo(app->canvas_editor, app->canvas_editor->current_layer - 1, app->canvas_editor->current_layer, undo_path))
		{
			pa_finalize_undo(app->canvas_editor);
		}
		pa_swap_canvas_layer(app->canvas, app->canvas_editor->current_layer, app->canvas_editor->current_layer - 1);
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
		pa_select_canvas_editor_layer(app->canvas_editor, app->canvas_editor->canvas->layer_max - 1);
		if(app->canvas_editor->current_layer < 0)
		{
			pa_select_canvas_editor_layer(app->canvas_editor, 0);
		}
	}
	t3f_refresh_menus();
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
		pa_select_canvas_editor_layer(app->canvas_editor, 0);
	}
	t3f_refresh_menus();
	t3f_debug_message("Exit pa_menu_layer_next()\n");
	return 0;
}

int pa_menu_layer_toggle_visibility(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	t3f_debug_message("Enter pa_menu_layer_toggle_visibility()\n");
	if(app->canvas->layer[app->canvas_editor->current_layer]->flags & PA_CANVAS_FLAG_HIDDEN)
	{
		app->canvas->layer[app->canvas_editor->current_layer]->flags &= ~PA_CANVAS_FLAG_HIDDEN;
	}
	else
	{
		app->canvas->layer[app->canvas_editor->current_layer]->flags |= PA_CANVAS_FLAG_HIDDEN;
	}
	t3f_refresh_menus();
	t3f_debug_message("Exit pa_menu_layer_toggle_visibility()\n");
	return 0;
}
