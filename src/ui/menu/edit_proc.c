#include "instance.h"
#include "modules/bitmap.h"
#include "modules/canvas/canvas.h"
#include "modules/canvas/canvas_helpers.h"
#include "modules/primitives.h"
#include "modules/dynamic_array.h"
#include "ui/canvas_editor/undo/undo.h"
#include "ui/canvas_editor/undo/tool.h"
#include "ui/canvas_editor/undo/selection/selection.h"
#include "ui/canvas_editor/clipboard.h"
#include "ui/canvas_editor/selection.h"
#include "ui/window.h"

int pa_menu_edit_undo(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;
	char undo_path[1024];

	t3f_debug_message("Enter pa_menu_edit_undo()\n");
	if(app->canvas_editor->undo_count > 0)
	{
		pa_get_undo_path("undo", app->canvas_editor->undo_count - 1, undo_path, 1024);
		pa_apply_undo(app->canvas_editor, undo_path, false);
		al_remove_filename(undo_path);
		app->canvas_editor->undo_count--;
		pa_update_undo_name(app->canvas_editor);
		pa_update_redo_name(app->canvas_editor);
		t3f_refresh_menus();
	}
	t3f_debug_message("Exit pa_menu_edit_undo()\n");

	return 0;
}

int pa_menu_edit_redo(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;
	char redo_path[1024];

	t3f_debug_message("Enter pa_menu_edit_redo()\n");
	if(app->canvas_editor->redo_count > 0)
	{
		pa_get_undo_path("redo", app->canvas_editor->redo_count - 1, redo_path, 1024);
		pa_apply_redo(app->canvas_editor, redo_path);
		al_remove_filename(redo_path);
		app->canvas_editor->redo_count--;
		pa_update_undo_name(app->canvas_editor);
		pa_update_redo_name(app->canvas_editor);
		t3f_refresh_menus();
	}
	t3f_debug_message("Exit pa_menu_edit_redo()\n");

	return 0;
}

int pa_menu_edit_cut(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;
	char undo_path[1024];

	t3f_debug_message("Enter pa_menu_edit_cut()\n");

	if(app->canvas_editor->selection.box.width > 0 && app->canvas_editor->selection.box.height > 0)
	{
		pa_get_undo_path("undo", app->canvas_editor->undo_count, undo_path, 1024);
		if(pa_make_delete_selection_undo(app->canvas_editor, PA_UNDO_TYPE_CUT_SELECTION, "Cut Selection", false, undo_path))
		{
			pa_finalize_undo(app->canvas_editor);
		}
		if(!app->canvas_editor->selection.bitmap_stack)
		{
			pa_copy_canvas_to_clipboard(app->canvas_editor, app->canvas_editor->current_layer, app->canvas_editor->selection.box.start_x, app->canvas_editor->selection.box.start_y, app->canvas_editor->selection.box.width, app->canvas_editor->selection.box.height);
			pa_draw_primitive_to_canvas(app->canvas_editor->canvas, app->canvas_editor->current_layer, app->canvas_editor->selection.box.start_x, app->canvas_editor->selection.box.start_y, app->canvas_editor->selection.box.end_x, app->canvas_editor->selection.box.end_y, NULL, al_map_rgba_f(0.0, 0.0, 0.0, 0.0), NULL, PA_RENDER_COPY, NULL,  pa_draw_filled_rectangle);
			pa_clear_canvas_editor_selection(app->canvas_editor);
		}
		else
		{
			if(pa_copy_bitmap_to_clipboard(app->canvas_editor, app->canvas_editor->selection.bitmap_stack))
			{
				app->canvas_editor->clipboard.x = app->canvas_editor->selection.box.start_x;
				app->canvas_editor->clipboard.y = app->canvas_editor->selection.box.start_y;
				pa_clear_canvas_editor_selection(app->canvas_editor);
			}
		}
		t3f_refresh_menus();
	}
	t3f_debug_message("Exit pa_menu_edit_cut()\n");

	return 0;
}

int pa_menu_edit_copy(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	t3f_debug_message("Enter pa_menu_edit_copy()\n");
	if(app->canvas_editor->selection.box.width > 0 && app->canvas_editor->selection.box.height > 0)
	{
		if(!app->canvas_editor->selection.bitmap_stack)
		{
			pa_copy_canvas_to_clipboard(app->canvas_editor, app->canvas_editor->current_layer, app->canvas_editor->selection.box.start_x, app->canvas_editor->selection.box.start_y, app->canvas_editor->selection.box.width, app->canvas_editor->selection.box.height);
		}
		else
		{
			if(pa_copy_bitmap_to_clipboard(app->canvas_editor, app->canvas_editor->selection.bitmap_stack))
			{
				app->canvas_editor->clipboard.x = app->canvas_editor->selection.box.start_x;
				app->canvas_editor->clipboard.y = app->canvas_editor->selection.box.start_y;
			}
		}
		t3f_refresh_menus();
	}
	t3f_debug_message("Exit pa_menu_edit_copy()\n");

	return 0;
}

int pa_menu_edit_paste(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;
	int pos = 0;
	int ox = 0;
	int oy = 0;

	if(app->canvas_editor->clipboard.bitmap_stack)
	{
		if(id < 0)
		{
			pos = 2;
			ox = -app->ui->element[PA_UI_ELEMENT_CANVAS_EDITOR]->x;
			oy = -app->ui->element[PA_UI_ELEMENT_CANVAS_EDITOR]->y;
		}
		pa_paste_clipboard(app->canvas_editor, pos, ox, oy, false);
	}

	return 0;
}

int pa_menu_edit_paste_in_place(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	if(app->canvas_editor->clipboard.bitmap_stack)
	{
		pa_paste_clipboard(app->canvas_editor, 1, 0, 0, false);
	}

	return 0;
}

int pa_menu_edit_paste_merged(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;
	int pos = 0;
	int ox = 0;
	int oy = 0;

	if(app->canvas_editor->clipboard.bitmap_stack)
	{
		if(id < 0)
		{
			pos = 2;
			ox = -app->ui->element[PA_UI_ELEMENT_CANVAS_EDITOR]->x;
			oy = -app->ui->element[PA_UI_ELEMENT_CANVAS_EDITOR]->y;
		}
		pa_paste_clipboard(app->canvas_editor, pos, ox, oy, true);
	}

	return 0;
}

int pa_menu_edit_flip_horizontal(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	if(app->canvas_editor->selection.box.width > 0 && app->canvas_editor->selection.box.height > 0)
	{
		pa_flip_selection(app->canvas_editor, true, false, false);
	}

	return 0;
}

int pa_menu_edit_flip_vertical(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	if(app->canvas_editor->selection.box.width > 0 && app->canvas_editor->selection.box.height > 0)
	{
		pa_flip_selection(app->canvas_editor, false, true, false);
	}

	return 0;
}

int pa_menu_edit_turn_clockwise(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	if(app->canvas_editor->selection.box.width > 0 && app->canvas_editor->selection.box.height > 0)
	{
		pa_turn_selection(app->canvas_editor, 1, false);
	}

	return 0;
}

int pa_menu_edit_turn_counter_clockwise(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	if(app->canvas_editor->selection.box.width > 0 && app->canvas_editor->selection.box.height > 0)
	{
		pa_turn_selection(app->canvas_editor, -1, false);
	}

	return 0;
}

int pa_menu_edit_delete(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;
	char undo_path[1024];

	t3f_debug_message("Enter pa_menu_edit_delete()\n");
	if(app->canvas_editor->selection.box.width > 0 && app->canvas_editor->selection.box.height > 0)
	{
		pa_get_undo_path("undo", app->canvas_editor->undo_count, undo_path, 1024);
		if(pa_make_delete_selection_undo(app->canvas_editor, PA_UNDO_TYPE_DELETE_SELECTION, "Delete Selection", false, undo_path))
		{
			pa_finalize_undo(app->canvas_editor);
		}
		if(!app->canvas_editor->selection.bitmap_stack)
		{
			pa_draw_primitive_to_canvas(app->canvas_editor->canvas, app->canvas_editor->current_layer, app->canvas_editor->selection.box.start_x, app->canvas_editor->selection.box.start_y, app->canvas_editor->selection.box.end_x, app->canvas_editor->selection.box.end_y, NULL, al_map_rgba_f(0.0, 0.0, 0.0, 0.0), NULL, PA_RENDER_COPY, NULL,  pa_draw_filled_rectangle);
			pa_clear_canvas_editor_selection(app->canvas_editor);
			app->canvas_editor->modified++;
			pa_set_window_message(NULL);
		}
		else
		{
			pa_clear_canvas_editor_selection(app->canvas_editor);
		}
		t3f_refresh_menus();
	}
	t3f_debug_message("Exit pa_menu_edit_delete()\n");

	return 0;
}

int pa_menu_edit_float_selection(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	if(app->canvas_editor->selection.box.width > 0 && app->canvas_editor->selection.box.height > 0)
	{
		pa_float_canvas_editor_selection(app->canvas_editor, &app->canvas_editor->selection.box, false);
		t3f_refresh_menus();
	}
	return 0;
}

int pa_menu_edit_unfloat_selection(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	if(app->canvas_editor->selection.bitmap_stack)
	{
		pa_unfloat_canvas_editor_selection(app->canvas_editor, &app->canvas_editor->selection.box);
		pa_clear_canvas_editor_selection(app->canvas_editor);
		t3f_refresh_menus();
	}
	return 0;
}

int pa_menu_edit_multilayer_cut(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;
	char undo_path[1024];
	int i;

	t3f_debug_message("Enter pa_menu_edit_multilayer_cut()\n");

	if(app->canvas_editor->selection.box.width > 0 && app->canvas_editor->selection.box.height > 0)
	{
		pa_get_undo_path("undo", app->canvas_editor->undo_count, undo_path, 1024);
		if(pa_make_delete_selection_undo(app->canvas_editor, PA_UNDO_TYPE_CUT_SELECTION, "Cut Selection", true, undo_path))
		{
			pa_finalize_undo(app->canvas_editor);
		}
		if(!app->canvas_editor->selection.bitmap_stack)
		{
			pa_copy_canvas_to_clipboard(app->canvas_editor, -1, app->canvas_editor->selection.box.start_x, app->canvas_editor->selection.box.start_y, app->canvas_editor->selection.box.width, app->canvas_editor->selection.box.height);
			for(i = 0; i < app->canvas->layer_max; i++)
			{
				pa_draw_primitive_to_canvas(app->canvas_editor->canvas, i, app->canvas_editor->selection.box.start_x, app->canvas_editor->selection.box.start_y, app->canvas_editor->selection.box.end_x, app->canvas_editor->selection.box.end_y, NULL, al_map_rgba_f(0.0, 0.0, 0.0, 0.0), NULL, PA_RENDER_COPY, NULL, pa_draw_filled_rectangle);
			}
			pa_clear_canvas_editor_selection(app->canvas_editor);
		}
		else
		{
			if(pa_copy_bitmap_to_clipboard(app->canvas_editor, app->canvas_editor->selection.bitmap_stack))
			{
				app->canvas_editor->clipboard.x = app->canvas_editor->selection.box.start_x;
				app->canvas_editor->clipboard.y = app->canvas_editor->selection.box.start_y;
				pa_clear_canvas_editor_selection(app->canvas_editor);
			}
		}
		t3f_refresh_menus();
	}
	t3f_debug_message("Exit pa_menu_edit_multilayer_cut()\n");

	return 0;
}

int pa_menu_edit_multilayer_copy(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	t3f_debug_message("Enter pa_menu_edit_multilayer_copy()\n");
	if(app->canvas_editor->selection.box.width > 0 && app->canvas_editor->selection.box.height > 0)
	{
		if(!app->canvas_editor->selection.bitmap_stack)
		{
			pa_copy_canvas_to_clipboard(app->canvas_editor, -1, app->canvas_editor->selection.box.start_x, app->canvas_editor->selection.box.start_y, app->canvas_editor->selection.box.width, app->canvas_editor->selection.box.height);
		}
		else
		{
			if(pa_copy_bitmap_to_clipboard(app->canvas_editor, app->canvas_editor->selection.bitmap_stack))
			{
				app->canvas_editor->clipboard.x = app->canvas_editor->selection.box.start_x;
				app->canvas_editor->clipboard.y = app->canvas_editor->selection.box.start_y;
			}
		}
		t3f_refresh_menus();
	}
	t3f_debug_message("Exit pa_menu_edit_multilayer_copy()\n");

	return 0;
}

int pa_menu_edit_multilayer_flip_horizontal(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	if(app->canvas_editor->selection.box.width > 0 && app->canvas_editor->selection.box.height > 0)
	{
		pa_flip_selection(app->canvas_editor, true, false, true);
	}

	return 0;
}

int pa_menu_edit_multilayer_flip_vertical(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	if(app->canvas_editor->selection.box.width > 0 && app->canvas_editor->selection.box.height > 0)
	{
		pa_flip_selection(app->canvas_editor, false, true, true);
	}

	return 0;
}

int pa_menu_edit_multilayer_turn_clockwise(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	if(app->canvas_editor->selection.box.width > 0 && app->canvas_editor->selection.box.height > 0)
	{
		pa_turn_selection(app->canvas_editor, 1, true);
	}

	return 0;
}

int pa_menu_edit_multilayer_turn_counter_clockwise(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	if(app->canvas_editor->selection.box.width > 0 && app->canvas_editor->selection.box.height > 0)
	{
		pa_turn_selection(app->canvas_editor, -1, true);
	}

	return 0;
}

int pa_menu_edit_multilayer_delete(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;
	char undo_path[1024];
	int i;

	t3f_debug_message("Enter pa_menu_edit_delete()\n");
	if(app->canvas_editor->selection.box.width > 0 && app->canvas_editor->selection.box.height > 0)
	{
		pa_get_undo_path("undo", app->canvas_editor->undo_count, undo_path, 1024);
		if(pa_make_delete_selection_undo(app->canvas_editor, PA_UNDO_TYPE_DELETE_SELECTION, "Delete Selection", true, undo_path))
		{
			pa_finalize_undo(app->canvas_editor);
		}
		if(!app->canvas_editor->selection.bitmap_stack)
		{
			for(i = 0; i < app->canvas->layer_max; i++)
			{
				pa_draw_primitive_to_canvas(app->canvas_editor->canvas, i, app->canvas_editor->selection.box.start_x, app->canvas_editor->selection.box.start_y, app->canvas_editor->selection.box.end_x, app->canvas_editor->selection.box.end_y, NULL, al_map_rgba_f(0.0, 0.0, 0.0, 0.0), NULL, PA_RENDER_COPY, NULL, pa_draw_filled_rectangle);
			}
			pa_clear_canvas_editor_selection(app->canvas_editor);
			app->canvas_editor->modified++;
		}
		else
		{
			pa_clear_canvas_editor_selection(app->canvas_editor);
		}
		t3f_refresh_menus();
	}
	t3f_debug_message("Exit pa_menu_edit_delete()\n");

	return 0;
}

int pa_menu_edit_multilayer_float_selection(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	if(app->canvas_editor->selection.box.width > 0 && app->canvas_editor->selection.box.height > 0)
	{
		pa_float_canvas_editor_selection(app->canvas_editor, &app->canvas_editor->selection.box, true);
		t3f_refresh_menus();
	}
	return 0;
}
