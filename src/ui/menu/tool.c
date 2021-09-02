#include "t3f/t3f.h"
#include "instance.h"
#include "menu.h"
#include "tool_proc.h"

int menu_grab_brush_update_proc(ALLEGRO_MENU * mp, int item, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	if(app->canvas_editor->selection.box.width > 0 && app->canvas_editor->selection.box.height > 0 && !(app->canvas_editor->selection.bitmap && app->canvas_editor->selection.layer < 0))
	{
		t3f_set_menu_item_flags(mp, item, 0);
	}
	else
	{
		t3f_set_menu_item_flags(mp, item, ALLEGRO_MENU_ITEM_DISABLED);
	}
	return 0;
}

int menu_tool_update_proc(ALLEGRO_MENU * mp, int item, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;
	int i;

	for(i = 0; i < 16; i++)
	{
		if(app->ui->tool_menu_item[i] == item)
		{
			if(app->canvas_editor->current_tool == i)
			{
				t3f_set_menu_item_flags(mp, item, ALLEGRO_MENU_ITEM_CHECKED);
			}
			else
			{
				t3f_set_menu_item_flags(mp, item, 0);
			}
			break;
		}
	}

	return 0;
}

ALLEGRO_MENU * pa_create_tool_brush_menu(void)
{
	ALLEGRO_MENU * mp;

	mp = al_create_menu();
	if(!mp)
	{
		return NULL;
	}
	t3f_add_menu_item(mp, "Reset", 0, NULL, pa_menu_tool_brush_reset, pa_menu_base_update_proc);
	t3f_add_menu_item(mp, "Grab from Selection", 0, NULL, pa_menu_tool_brush_grab_from_selection, menu_grab_brush_update_proc);
	t3f_add_menu_item(mp, "Grab from Selection Multicolor", 0, NULL, pa_menu_tool_brush_grab_from_selection_multicolor, menu_grab_brush_update_proc);
	t3f_add_menu_item(mp, "Load", 0, NULL, pa_menu_tool_brush_load, pa_menu_base_update_proc);
	t3f_add_menu_item(mp, "Load Multicolor", 0, NULL, pa_menu_tool_brush_load_multicolor, pa_menu_base_update_proc);

	return mp;
}

ALLEGRO_MENU * pa_create_tool_menu(ALLEGRO_MENU * brush_mp, int * item_array)
{
	ALLEGRO_MENU * mp;

	mp = al_create_menu();
	if(!mp)
	{
		return NULL;
	}
	t3f_add_menu_item(mp, "Brush", 0, brush_mp, NULL, pa_menu_base_update_proc);
	t3f_add_menu_item(mp, NULL, 0, NULL, NULL, NULL);
	item_array[PA_TOOL_PIXEL] = t3f_add_menu_item(mp, "Pixel", ALLEGRO_MENU_ITEM_CHECKBOX, NULL, pa_menu_tool_pixel, menu_tool_update_proc);
	item_array[PA_TOOL_LINE] = t3f_add_menu_item(mp, "Line", ALLEGRO_MENU_ITEM_CHECKBOX, NULL, pa_menu_tool_line, menu_tool_update_proc);
	item_array[PA_TOOL_RECTANGLE] = t3f_add_menu_item(mp, "Rectangle", ALLEGRO_MENU_ITEM_CHECKBOX, NULL, pa_menu_tool_rectangle, menu_tool_update_proc);
	item_array[PA_TOOL_FILLED_RECTANGLE] = t3f_add_menu_item(mp, "Filled Rectangle", ALLEGRO_MENU_ITEM_CHECKBOX, NULL, pa_menu_tool_filled_rectangle, menu_tool_update_proc);
	item_array[PA_TOOL_OVAL] = t3f_add_menu_item(mp, "Oval", ALLEGRO_MENU_ITEM_CHECKBOX, NULL, pa_menu_tool_oval, menu_tool_update_proc);
	item_array[PA_TOOL_FILLED_OVAL] = t3f_add_menu_item(mp, "Filled Oval", ALLEGRO_MENU_ITEM_CHECKBOX, NULL, pa_menu_tool_filled_oval, menu_tool_update_proc);
	item_array[PA_TOOL_FLOOD_FILL] = t3f_add_menu_item(mp, "Flood Fill", ALLEGRO_MENU_ITEM_CHECKBOX, NULL, pa_menu_tool_flood_fill, menu_tool_update_proc);
	item_array[PA_TOOL_ERASER] = t3f_add_menu_item(mp, "Eraser", ALLEGRO_MENU_ITEM_CHECKBOX, NULL, pa_menu_tool_eraser, menu_tool_update_proc);
	item_array[PA_TOOL_DROPPER] = t3f_add_menu_item(mp, "Dropper", ALLEGRO_MENU_ITEM_CHECKBOX, NULL, pa_menu_tool_dropper, menu_tool_update_proc);
	item_array[PA_TOOL_SELECTION] = t3f_add_menu_item(mp, "Selector", ALLEGRO_MENU_ITEM_CHECKBOX, NULL, pa_menu_tool_selector, menu_tool_update_proc);
	item_array[PA_TOOL_FRAME] = t3f_add_menu_item(mp, "Frame", ALLEGRO_MENU_ITEM_CHECKBOX, NULL, pa_menu_tool_frame, menu_tool_update_proc);

	return mp;
}
