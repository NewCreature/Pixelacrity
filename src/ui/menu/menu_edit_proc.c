#include "instance.h"
#include "modules/canvas/canvas.h"
#include "modules/canvas/canvas_helpers.h"
#include "modules/primitives.h"
#include "ui/canvas_editor/undo.h"
#include "ui/canvas_editor/undo_tool.h"
#include "ui/canvas_editor/undo_selection.h"
#include "ui/canvas_editor/clipboard.h"

int quixel_menu_edit_undo(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;
	char undo_path[1024];

	t3f_debug_message("Enter quixel_menu_edit_undo()\n");
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
	t3f_debug_message("Exit quixel_menu_edit_undo()\n");

	return 0;
}

int quixel_menu_edit_redo(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;
	char redo_path[1024];

	t3f_debug_message("Enter quixel_menu_edit_redo()\n");
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
	t3f_debug_message("Exit quixel_menu_edit_redo()\n");

	return 0;
}

int quixel_menu_edit_cut(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	t3f_debug_message("Enter quixel_menu_edit_cut()\n");
	if(app->canvas_editor->selection.box.width > 0 && app->canvas_editor->selection.box.height > 0)
	{
		if(!app->canvas_editor->selection.bitmap)
		{
			pa_copy_canvas_to_clipboard(app->canvas_editor, app->canvas_editor->current_layer, app->canvas_editor->selection.box.start_x, app->canvas_editor->selection.box.start_y, app->canvas_editor->selection.box.width, app->canvas_editor->selection.box.height);
			quixel_draw_primitive_to_canvas(app->canvas_editor->canvas, app->canvas_editor->current_layer, app->canvas_editor->selection.box.start_x, app->canvas_editor->selection.box.start_y, app->canvas_editor->selection.box.end_x, app->canvas_editor->selection.box.end_y, NULL, al_map_rgba_f(0.0, 0.0, 0.0, 0.0), QUIXEL_RENDER_COPY, quixel_draw_filled_rectangle);
			app->canvas_editor->selection.box.width = 0;
			app->canvas_editor->selection.box.height = 0;
		}
		else
		{
			if(pa_copy_bitmap_to_clipboard(app->canvas_editor, app->canvas_editor->selection.bitmap))
			{
				if(app->canvas_editor->selection.bitmap)
				{
					al_destroy_bitmap(app->canvas_editor->selection.bitmap);
					app->canvas_editor->selection.bitmap = NULL;
				}
				if(app->canvas_editor->selection.combined_bitmap)
				{
					al_destroy_bitmap(app->canvas_editor->selection.combined_bitmap);
					app->canvas_editor->selection.combined_bitmap = NULL;
				}
				app->canvas_editor->selection.box.width = 0;
				app->canvas_editor->selection.box.height = 0;
			}
		}
		t3f_refresh_menus();
	}
	t3f_debug_message("Exit quixel_menu_edit_cut()\n");

	return 0;
}

int quixel_menu_edit_copy(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	t3f_debug_message("Enter quixel_menu_edit_copy()\n");
	if(app->canvas_editor->selection.box.width > 0 && app->canvas_editor->selection.box.height > 0)
	{
		if(!app->canvas_editor->selection.bitmap)
		{
			pa_copy_canvas_to_clipboard(app->canvas_editor, app->canvas_editor->current_layer, app->canvas_editor->selection.box.start_x, app->canvas_editor->selection.box.start_y, app->canvas_editor->selection.box.width, app->canvas_editor->selection.box.height);
		}
		else
		{
			if(pa_copy_bitmap_to_clipboard(app->canvas_editor, app->canvas_editor->selection.bitmap))
			{
			}
		}
		t3f_refresh_menus();
	}
	t3f_debug_message("Exit quixel_menu_edit_copy()\n");

	return 0;
}

int quixel_menu_edit_paste(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;
	ALLEGRO_STATE old_state;

	t3f_debug_message("Enter quixel_menu_edit_paste()\n");
	if(app->canvas_editor->clipboard.bitmap)
	{
		al_store_state(&old_state, ALLEGRO_STATE_NEW_BITMAP_PARAMETERS);
		al_set_new_bitmap_flags(ALLEGRO_NO_PREMULTIPLIED_ALPHA);
		if(app->canvas_editor->selection.bitmap)
		{
			quixel_handle_unfloat_canvas_editor_selection(app->canvas_editor, &app->canvas_editor->selection.box);
		}
		app->canvas_editor->selection.bitmap = al_clone_bitmap(app->canvas_editor->clipboard.bitmap);
		if(app->canvas_editor->selection.bitmap)
		{
			app->canvas_editor->selection.combined_bitmap = al_create_bitmap(al_get_bitmap_width(app->canvas_editor->selection.bitmap), al_get_bitmap_height(app->canvas_editor->selection.bitmap));
			if(app->canvas_editor->selection.combined_bitmap)
			{
				quixel_initialize_box(&app->canvas_editor->selection.box, app->canvas_editor->view_x, app->canvas_editor->view_y, al_get_bitmap_width(app->canvas_editor->selection.bitmap), al_get_bitmap_height(app->canvas_editor->selection.bitmap), app->canvas_editor->peg_bitmap);
				quixel_update_box_handles(&app->canvas_editor->selection.box, app->canvas_editor->view_x, app->canvas_editor->view_y, app->canvas_editor->view_zoom);
			}
			else
			{
				al_destroy_bitmap(app->canvas_editor->selection.bitmap);
				app->canvas_editor->selection.bitmap = NULL;
			}
		}
		al_restore_state(&old_state);
		t3f_refresh_menus();
	}
	t3f_debug_message("Exit quixel_menu_edit_paste()\n");

	return 0;
}

int quixel_menu_edit_delete_layer(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	t3f_debug_message("Enter quixel_menu_edit_delete_layer()\n");
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
	t3f_debug_message("Exit quixel_menu_edit_delete_layer()\n");
	return 0;
}

int quixel_menu_edit_delete(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;
	char undo_path[1024];

	t3f_debug_message("Enter quixel_menu_edit_delete()\n");
	if(app->canvas_editor->selection.box.width > 0 && app->canvas_editor->selection.box.height > 0)
	{
		quixel_get_undo_path("undo", app->canvas_editor->undo_count, undo_path, 1024);
		if(!app->canvas_editor->selection.bitmap)
		{
			if(quixel_make_tool_undo(app->canvas_editor, "Delete Selection", app->canvas_editor->current_layer, app->canvas_editor->selection.box.start_x, app->canvas_editor->selection.box.start_y, app->canvas_editor->selection.box.width, app->canvas_editor->selection.box.height, undo_path))
			{
				quixel_finalize_undo(app->canvas_editor);
			}
			quixel_draw_primitive_to_canvas(app->canvas_editor->canvas, app->canvas_editor->current_layer, app->canvas_editor->selection.box.start_x, app->canvas_editor->selection.box.start_y, app->canvas_editor->selection.box.end_x, app->canvas_editor->selection.box.end_y, NULL, al_map_rgba_f(0.0, 0.0, 0.0, 0.0), QUIXEL_RENDER_COPY, quixel_draw_filled_rectangle);
			app->canvas_editor->selection.box.width = 0;
			app->canvas_editor->selection.box.height = 0;
			app->canvas_editor->modified++;
		}
		else
		{
			if(app->canvas_editor->selection.bitmap)
			{
				al_destroy_bitmap(app->canvas_editor->selection.bitmap);
				app->canvas_editor->selection.bitmap = NULL;
			}
			if(app->canvas_editor->selection.combined_bitmap)
			{
				al_destroy_bitmap(app->canvas_editor->selection.combined_bitmap);
				app->canvas_editor->selection.combined_bitmap = NULL;
			}
			app->canvas_editor->selection.box.width = 0;
			app->canvas_editor->selection.box.height = 0;
		}
		t3f_refresh_menus();
	}
	t3f_debug_message("Exit quixel_menu_edit_delete()\n");

	return 0;
}
