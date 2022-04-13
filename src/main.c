#include "t3f/t3f.h"
#include "instance.h"
#include "modules/canvas/canvas_file.h"
#include "modules/pixel_shader.h"
#include "modules/date.h"
#include "modules/mouse_cursor.h"
#include "ui/canvas_editor/canvas_editor.h"
#include "ui/canvas_editor/undo/undo.h"
#include "ui/menu/menu.h"
#include "ui/menu/file_proc.h"
#include "ui/menu/edit_proc.h"
#include "ui/window.h"
#include "shortcuts.h"

void app_event_handler(ALLEGRO_EVENT * event, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;
	ALLEGRO_STATE old_state;
	int i;

	switch(event->type)
	{
		case ALLEGRO_EVENT_DISPLAY_RESIZE:
		{
			t3f_debug_message("Resize event start\n");
			t3f_event_handler(event);
			t3f_debug_message("Resize event default handling done\n");
			app->restart_ui = true;
			if(app->canvas_editor)
			{
				if(app->canvas_editor->tool_bitmap)
				{
					al_store_state(&old_state, ALLEGRO_STATE_NEW_BITMAP_PARAMETERS);
					al_set_new_bitmap_flags(0);
					al_destroy_bitmap(app->canvas_editor->tool_bitmap);
					app->canvas_editor->tool_bitmap = al_create_bitmap(al_get_display_width(t3f_display), al_get_display_height(t3f_display));
					if(!app->canvas_editor->tool_bitmap)
					{
						printf("Could not resize tool bitmap!");
					}
					al_restore_state(&old_state);
				}
				if(app->canvas_editor->scratch_bitmap)
				{
					al_store_state(&old_state, ALLEGRO_STATE_NEW_BITMAP_PARAMETERS);
					al_set_new_bitmap_flags(0);
					al_destroy_bitmap(app->canvas_editor->scratch_bitmap);
					app->canvas_editor->scratch_bitmap = al_create_bitmap(al_get_display_width(t3f_display), al_get_display_height(t3f_display));
					if(!app->canvas_editor->scratch_bitmap)
					{
						printf("Could not resize scratch bitmap!");
					}
					al_restore_state(&old_state);
				}
			}
			t3f_debug_message("Resize event custom handling done\n");
			break;
		}
		case ALLEGRO_EVENT_DISPLAY_CLOSE:
		{
			t3f_debug_message("Display close event start\n");
			if(app->ui->brush_popup_dialog && event->display.source == app->ui->brush_popup_dialog->display)
			{
				pa_close_popup_dialog(app->ui->brush_popup_dialog);
				app->ui->brush_popup_dialog = NULL;
			}
			else if(app->ui->color_popup_dialog && event->display.source == app->ui->color_popup_dialog->display)
			{
				pa_close_popup_dialog(app->ui->color_popup_dialog);
				app->ui->color_popup_dialog = NULL;
			}
			else
			{
				pa_menu_file_exit(0, data);
			}
			t3f_debug_message("Display close event custom handling done\n");
			break;
		}
		case ALLEGRO_EVENT_DISPLAY_SWITCH_OUT:
		case ALLEGRO_EVENT_DISPLAY_SWITCH_IN:
		{
			for(i = 0; i < ALLEGRO_KEY_MAX; i++)
			{
				t3f_key[i] = 0;
			}
			break;
		}
		case ALLEGRO_EVENT_MOUSE_ENTER_DISPLAY:
		{
			pa_allow_mouse_cursor_changes(true);
			pa_set_mouse_cursor(app->last_cursor);
			break;
		}
		case ALLEGRO_EVENT_MOUSE_LEAVE_DISPLAY:
		{
			app->last_cursor = pa_get_mouse_cursor();
			pa_set_mouse_cursor(ALLEGRO_SYSTEM_MOUSE_CURSOR_ARROW);
			pa_allow_mouse_cursor_changes(false);
			break;
		}
		default:
		{
			t3f_event_handler(event);
			break;
		}
	}
}

/* main logic routine */
void app_logic(void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	if(app->restart_ui)
	{
		pa_resize_ui(app->ui);
		app->restart_ui = false;
	}
	/* handle signals */
	if(app->canvas_editor)
	{
		switch(app->canvas_editor->signal)
		{
			case PA_CANVAS_EDITOR_SIGNAL_DELETE_LAYER:
			{
				app->canvas_editor->signal = PA_CANVAS_EDITOR_SIGNAL_NONE;
				break;
			}
		}
	}

	pa_process_ui(app->ui);
	strcpy(app->ui->status_left_message, "");
	if(app->canvas_editor->tool_state == PA_TOOL_STATE_DRAWING)
	{
		sprintf(app->ui->status_left_message, "(%d, %d)", abs((int)app->canvas_editor->start_x - (int)app->canvas_editor->end_x) + 1, abs((int)app->canvas_editor->start_y - (int)app->canvas_editor->end_y) + 1);
	}
	else if(app->canvas_editor->tool_state == PA_TOOL_STATE_EDITING)
	{
		sprintf(app->ui->status_left_message, "Selection: (%d, %d)", app->canvas_editor->selection.box.width, app->canvas_editor->selection.box.height);
	}
	else if(app->canvas_editor->current_frame < app->canvas->frame_max)
	{
		sprintf(app->ui->status_left_message, "(%d, %d)", app->canvas_editor->hover_x - app->canvas->frame[app->canvas_editor->current_frame]->box.start_x, app->canvas_editor->hover_y - app->canvas->frame[app->canvas_editor->current_frame]->box.start_y);
		sprintf(app->ui->status_right_message, "\t%s", app->canvas->frame[app->canvas_editor->current_frame]->export_path ? app->canvas->frame[app->canvas_editor->current_frame]->export_path : "");
	}
	pa_handle_shortcuts(app);
}

/* main rendering routine */
void app_render(void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	pa_render_ui(app->ui);
}

/* initialize our app, load graphics, etc. */
bool app_initialize(APP_INSTANCE * app, int argc, char * argv[])
{
	char date_string[256];
	char debug_fn[1024];
	const char * val;

	/* initialize T3F */
	if(!t3f_initialize(T3F_APP_TITLE, 640, 480, 60.0, app_logic, app_render, T3F_USE_KEYBOARD | T3F_USE_MOUSE | T3F_USE_MENU | T3F_RESIZABLE | T3F_NO_SCALE | T3F_USE_OPENGL, app))
	{
		printf("Error initializing T3F\n");
		return false;
	}
	val = al_get_config_value(t3f_config, "Debug", "clean_exit");
	if(val)
	{
		if(!strcmp(val, "false"))
		{
			val = al_get_config_value(t3f_config, "Debug", "log_path");
			if(val)
			{
				sprintf(debug_fn, "\"%s\"", val);
				t3f_open_url(debug_fn);
			}
		}
	}
	al_set_new_bitmap_flags(0);
	memset(app, 0, sizeof(APP_INSTANCE));
	app->last_cursor = ALLEGRO_SYSTEM_MOUSE_CURSOR_ARROW;
	t3f_set_event_handler(app_event_handler);

	pa_get_date_string(date_string, 256);
	strcat(date_string, ".log");
	t3f_get_filename(t3f_data_path, date_string, debug_fn, 1024);
	al_set_config_value(t3f_config, "Debug", "log_path", debug_fn);
	al_set_config_value(t3f_config, "Debug", "clean_exit", "false");
	t3f_save_config();
	t3f_open_debug_log(debug_fn);

	t3f_debug_message("Create starting canvas\n");
	pa_menu_file_new(0, app);
	if(!app->canvas)
	{
		t3f_debug_message("Failed to create starting canvas\n");
		return false;
	}
	t3f_debug_message("Create canvas editor\n");
	app->canvas_editor = pa_create_canvas_editor(app->canvas);
	if(!app->canvas_editor)
	{
		t3f_debug_message("Failed to create canvas editor\n");
		return false;
	}
	val = al_get_config_value(t3f_config, "App Data", "auto_backup");
	if(val)
	{
		if(!strcasecmp(val, "false"))
		{
			app->canvas_editor->auto_backup = false;
		}
	}
	pa_set_window_message(NULL);

	t3f_debug_message("Create UI\n");
	app->ui = pa_create_ui(app->canvas_editor);
	if(!app->ui)
	{
		return false;
	}
	t3f_debug_message("Resize UI\n");
	pa_resize_ui(app->ui);
	t3f_debug_message("Show GUI\n");
	t3gui_show_dialog(app->ui->dialog[PA_UI_DIALOG_MAIN], NULL, T3GUI_PLAYER_NO_ESCAPE | T3GUI_PLAYER_IGNORE_CLOSE, app);
	al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA);

	return true;
}

void app_exit(APP_INSTANCE * app)
{
	t3f_debug_message("Destroy UI\n");
	if(app->ui)
	{
		pa_destroy_ui(app->ui);
	}
	if(app->canvas_editor)
	{
		pa_resave_canvas_editor_state(app->canvas_editor);
		t3f_debug_message("Clean up undo data\n");
		pa_undo_clean_up(app->canvas_editor);
		t3f_debug_message("Destroy canvas editor\n");
		pa_destroy_canvas_editor(app->canvas_editor);
	}
	if(app->canvas)
	{
		t3f_debug_message("Destroy canvas\n");
		pa_destroy_canvas(app->canvas);
	}
	t3f_close_debug_log();
	al_set_config_value(t3f_config, "Debug", "clean_exit", "true");
}

int main(int argc, char * argv[])
{
	APP_INSTANCE app;

	if(app_initialize(&app, argc, argv))
	{
		t3f_run();
	}
	else
	{
		printf("Error: could not initialize T3F!\n");
	}
	app_exit(&app);
	t3f_finish();

	return 0;
}
