#include "t3f/t3f.h"
#include "ui/ui.h"
#include "file.h"
#include "edit.h"
#include "frame.h"
#include "layer.h"
#include "tool.h"
#include "palette.h"
#include "instance.h"

int pa_menu_base_update_proc(ALLEGRO_MENU * mp, int item, void * data)
{
	return 0;
}

bool pa_setup_menus(PA_UI * uip)
{
	uip->menu[PA_UI_MENU_LOAD_RECENT] = pa_create_load_recent_menu(uip);
	if(!uip->menu[PA_UI_MENU_LOAD_RECENT])
	{
		return false;
	}

	uip->menu[PA_UI_MENU_FILE] = pa_create_file_menu(uip->menu[PA_UI_MENU_LOAD_RECENT]);
	if(!uip->menu[PA_UI_MENU_FILE])
	{
		return false;
	}

	uip->menu[PA_UI_MENU_EDIT_MULTILAYER] = pa_create_edit_multilayer_menu();
	if(!uip->menu[PA_UI_MENU_EDIT_MULTILAYER])
	{
		return false;
	}

	uip->menu[PA_UI_MENU_EDIT] = pa_create_edit_menu(uip->menu[PA_UI_MENU_EDIT_MULTILAYER]);
	if(!uip->menu[PA_UI_MENU_EDIT])
	{
		return false;
	}

	uip->menu[PA_UI_MENU_FRAME_ADD_ICON_TEMPLATE] = pa_create_frame_icon_template_menu();
	if(!uip->menu[PA_UI_MENU_FRAME_ADD_ICON_TEMPLATE])
	{
		return false;
	}

	uip->menu[PA_UI_MENU_FRAME_ADD_DISPLAY_TEMPLATE] = pa_create_frame_display_template_menu();
	if(!uip->menu[PA_UI_MENU_FRAME_ADD_DISPLAY_TEMPLATE])
	{
		return false;
	}

	uip->menu[PA_UI_MENU_FRAME_ADD] = pa_create_frame_add_menu(uip->menu[PA_UI_MENU_FRAME_ADD_ICON_TEMPLATE], uip->menu[PA_UI_MENU_FRAME_ADD_DISPLAY_TEMPLATE]);
	if(!uip->menu[PA_UI_MENU_FRAME_ADD])
	{
		return false;
	}

	uip->menu[PA_UI_MENU_FRAME] = pa_create_frame_menu(uip->menu[PA_UI_MENU_FRAME_ADD]);
	if(!uip->menu[PA_UI_MENU_FRAME])
	{
		return false;
	}

	uip->menu[PA_UI_MENU_MAIN] = al_create_menu();
	if(!uip->menu[PA_UI_MENU_MAIN])
	{
		return false;
	}

	uip->menu[PA_UI_MENU_LAYER] = pa_create_layer_menu();
	if(!uip->menu[PA_UI_MENU_LAYER])
	{
		return false;
	}

	uip->menu[PA_UI_MENU_TOOL_BRUSH] = pa_create_tool_brush_menu();
	if(!uip->menu[PA_UI_MENU_TOOL_BRUSH])
	{
		return false;
	}

	uip->menu[PA_UI_MENU_TOOL] = pa_create_tool_menu(uip->menu[PA_UI_MENU_TOOL_BRUSH], uip->tool_menu_item);
	if(!uip->menu[PA_UI_MENU_TOOL])
	{
		return false;
	}

	uip->menu[PA_UI_MENU_PALETTE] = pa_create_palette_menu();
	if(!uip->menu[PA_UI_MENU_PALETTE])
	{
		return false;
	}

	t3f_add_menu_item(uip->menu[PA_UI_MENU_MAIN], "File", 0, uip->menu[PA_UI_MENU_FILE], NULL, NULL);
	t3f_add_menu_item(uip->menu[PA_UI_MENU_MAIN], "Edit", 0, uip->menu[PA_UI_MENU_EDIT], NULL, NULL);
	t3f_add_menu_item(uip->menu[PA_UI_MENU_MAIN], "Frame", 0, uip->menu[PA_UI_MENU_FRAME], NULL, NULL);
	t3f_add_menu_item(uip->menu[PA_UI_MENU_MAIN], "Layer", 0, uip->menu[PA_UI_MENU_LAYER], NULL, NULL);
	t3f_add_menu_item(uip->menu[PA_UI_MENU_MAIN], "Tool", 0, uip->menu[PA_UI_MENU_TOOL], NULL, NULL);
	t3f_add_menu_item(uip->menu[PA_UI_MENU_MAIN], "Palette", 0, uip->menu[PA_UI_MENU_PALETTE], NULL, NULL);
	t3f_refresh_menus();

	return true;
}

void pa_destroy_menus(PA_UI * uip)
{
	int i;

	al_set_display_menu(t3f_display, NULL);
	for(i = 0; i < PA_UI_MAX_MENUS; i++)
	{
		uip->menu[i] = NULL;
	}
	t3f_reset_menus();
}

void pa_update_undo_menu(PA_UI * uip, const char * undo_name, const char * redo_name)
{
	al_set_menu_item_caption(uip->menu[PA_UI_MENU_EDIT], 1, redo_name);
}
