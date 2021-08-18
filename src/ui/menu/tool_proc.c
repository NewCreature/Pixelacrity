#include "t3f/t3f.h"
#include "instance.h"

int pa_menu_tool_brush_reset(int id, void * data)
{
		APP_INSTANCE * app = (APP_INSTANCE *)data;

		return 0;
}

int pa_menu_tool_brush_grab_from_selection(int id, void * data)
{
		APP_INSTANCE * app = (APP_INSTANCE *)data;

		return 0;
}

int pa_menu_tool_brush_load(int id, void * data)
{
		APP_INSTANCE * app = (APP_INSTANCE *)data;

		return 0;
}

int pa_menu_tool_pixel(int id, void * data)
{
		APP_INSTANCE * app = (APP_INSTANCE *)data;

		pa_select_canvas_editor_tool(app->canvas_editor, 0);

		return 0;
}

int pa_menu_tool_line(int id, void * data)
{
		APP_INSTANCE * app = (APP_INSTANCE *)data;

		pa_select_canvas_editor_tool(app->canvas_editor, 1);

		return 0;
}

int pa_menu_tool_rectangle(int id, void * data)
{
		APP_INSTANCE * app = (APP_INSTANCE *)data;

		pa_select_canvas_editor_tool(app->canvas_editor, 2);

		return 0;
}

int pa_menu_tool_filled_rectangle(int id, void * data)
{
		APP_INSTANCE * app = (APP_INSTANCE *)data;

		pa_select_canvas_editor_tool(app->canvas_editor, 3);

		return 0;
}

int pa_menu_tool_oval(int id, void * data)
{
		APP_INSTANCE * app = (APP_INSTANCE *)data;

		pa_select_canvas_editor_tool(app->canvas_editor, 4);

		return 0;
}

int pa_menu_tool_filled_oval(int id, void * data)
{
		APP_INSTANCE * app = (APP_INSTANCE *)data;

		pa_select_canvas_editor_tool(app->canvas_editor, 5);

		return 0;
}

int pa_menu_tool_flood_fill(int id, void * data)
{
		APP_INSTANCE * app = (APP_INSTANCE *)data;

		pa_select_canvas_editor_tool(app->canvas_editor, 6);

		return 0;
}

int pa_menu_tool_eraser(int id, void * data)
{
		APP_INSTANCE * app = (APP_INSTANCE *)data;

		pa_select_canvas_editor_tool(app->canvas_editor, 7);

		return 0;
}

int pa_menu_tool_dropper(int id, void * data)
{
		APP_INSTANCE * app = (APP_INSTANCE *)data;

		pa_select_canvas_editor_tool(app->canvas_editor, 8);

		return 0;
}

int pa_menu_tool_selector(int id, void * data)
{
		APP_INSTANCE * app = (APP_INSTANCE *)data;

		pa_select_canvas_editor_tool(app->canvas_editor, 9);

		return 0;
}
