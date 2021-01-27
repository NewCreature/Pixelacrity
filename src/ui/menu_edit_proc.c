#include "instance.h"
#include "canvas.h"

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
