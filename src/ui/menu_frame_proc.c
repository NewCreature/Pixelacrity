#include "instance.h"
#include "canvas.h"

int quixel_menu_frame_add(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;
	char buf[256];

	if(app->canvas_editor->selection.box.width > 0 && app->canvas_editor->selection.box.height > 0)
	{
		sprintf(buf, "Frame %d", app->canvas->frame_max + 1);
		quixel_add_canvas_frame(app->canvas, buf, app->canvas_editor->selection.box.start_x, app->canvas_editor->selection.box.start_y, app->canvas_editor->selection.box.width, app->canvas_editor->selection.box.height);
		app->canvas_editor->current_frame = app->canvas->frame_max - 1;
	}
	return 0;
}

int quixel_menu_frame_delete(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	if(app->canvas_editor->current_frame < app->canvas->frame_max)
	{
		quixel_remove_canvas_frame(app->canvas, app->canvas_editor->current_frame);
	}
	return 0;
}

int quixel_menu_frame_previous(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	app->canvas_editor->current_frame--;
	if(app->canvas_editor->current_frame < 0)
	{
		app->canvas_editor->current_frame = app->canvas->frame_max - 1;
		if(app->canvas_editor->current_frame < 0)
		{
			app->canvas_editor->current_frame = 0;
		}
	}
	return 0;
}

int quixel_menu_frame_next(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	app->canvas_editor->current_frame++;
	if(app->canvas_editor->current_frame >= app->canvas->frame_max)
	{
		app->canvas_editor->current_frame = 0;
	}
	return 0;
}
