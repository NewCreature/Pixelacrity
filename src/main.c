#include "t3f/t3f.h"
#include "instance.h"
#include "canvas_file.h"
#include "pixel_shader.h"
#include "ui/canvas_editor.h"
#include "ui/menu_file_proc.h"
#include "ui/menu_edit_proc.h"

/* main logic routine */
void app_logic(void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	/* handle signals */
	if(app->canvas_editor)
	{
		switch(app->canvas_editor->signal)
		{
			case QUIXEL_CANVAS_EDITOR_SIGNAL_DELETE_LAYER:
			{
				app->canvas_editor->signal = QUIXEL_CANVAS_EDITOR_SIGNAL_NONE;
				break;
			}
		}
	}

	quixel_process_ui(app->ui);
	strcpy(app->ui->status_left_message, "");
	if(app->canvas_editor->tool_state == QUIXEL_TOOL_STATE_DRAWING)
	{
		sprintf(app->ui->status_left_message, "(%d, %d)", abs(app->canvas_editor->click_x - app->canvas_editor->hover_x) + 1, abs(app->canvas_editor->click_y - app->canvas_editor->hover_y) + 1);
	}
	if(app->canvas_editor->current_frame < app->canvas->frame_max)
	{
		sprintf(app->ui->status_left_message, "Frame: %s (%d, %d)", app->canvas->frame[app->canvas_editor->current_frame]->name, app->canvas->frame[app->canvas_editor->current_frame]->width, app->canvas->frame[app->canvas_editor->current_frame]->height);
	}
}

/* main rendering routine */
void app_render(void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	quixel_render_ui(app->ui);
}

static int get_config_val(ALLEGRO_CONFIG * cp, const char * section, const char * key, int default_val)
{
	const char * val;

	val = al_get_config_value(cp, section, key);
	if(val)
	{
		return atoi(val);
	}
	return default_val;
}

/* initialize our app, load graphics, etc. */
bool app_initialize(APP_INSTANCE * app, int argc, char * argv[])
{
	char buf[1024];

	/* initialize T3F */
	if(!t3f_initialize(T3F_APP_TITLE, 640, 480, 60.0, app_logic, app_render, T3F_DEFAULT | T3F_USE_MENU, app))
	{
		printf("Error initializing T3F\n");
		return false;
	}

	app->alpha_shader = quixel_create_pixel_shader("data/shaders/alpha_blend_shader.glsl");
	if(!app->alpha_shader)
	{
		printf("Error initializing alpha shader!\n");
		return false;
	}

	al_use_shader(app->alpha_shader);

	quixel_menu_file_new(0, app);
	if(!app->canvas)
	{
		return false;
	}
	app->canvas_editor = quixel_create_canvas_editor(app->canvas);
	if(!app->canvas_editor)
	{
		return false;
	}
	app->canvas_editor->current_layer = get_config_val(app->canvas->config, "state", "current_layer", 0);
	app->canvas_editor->view_x = get_config_val(app->canvas->config, "state", "view_x", app->canvas->bitmap_size * 8 + app->canvas->bitmap_size / 2);
	app->canvas_editor->view_y = get_config_val(app->canvas->config, "state", "view_y", app->canvas->bitmap_size * 8 + app->canvas->bitmap_size / 2);
	app->canvas_editor->view_zoom = get_config_val(app->canvas->config, "state", "view_zoom", 8);

	app->ui = quixel_create_ui(app->canvas_editor);
	if(!app->ui)
	{
		printf("Unable to create UI!\n");
		return false;
	}
	t3gui_show_dialog(app->ui->dialog[QUIXEL_UI_DIALOG_MAIN], NULL, T3GUI_PLAYER_NO_ESCAPE, app);

	al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);

	return true;
}

void app_exit(APP_INSTANCE * app)
{
	char buf[1024];

	sprintf(buf, "%d", app->canvas_editor->current_layer);
	al_set_config_value(app->canvas->config, "state", "current_layer", buf);
	sprintf(buf, "%d", app->canvas_editor->view_x);
	al_set_config_value(app->canvas->config, "state", "view_x", buf);
	sprintf(buf, "%d", app->canvas_editor->view_y);
	al_set_config_value(app->canvas->config, "state", "view_y", buf);
	sprintf(buf, "%d", app->canvas_editor->view_zoom);
	al_set_config_value(app->canvas->config, "state", "view_zoom", buf);
	quixel_destroy_canvas_editor(app->canvas_editor);
	quixel_destroy_canvas(app->canvas);
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

	return 0;
}
