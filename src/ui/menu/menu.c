#include "t3f/t3f.h"
#include "ui/ui.h"
#include "menu_file_proc.h"
#include "menu_edit_proc.h"
#include "menu_frame_proc.h"
#include "menu_layer_proc.h"
#include "instance.h"

static int menu_base_update_proc(ALLEGRO_MENU * mp, int item, void * data)
{
	return 0;
}

static int menu_undo_update_proc(ALLEGRO_MENU * mp, int item, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	al_set_menu_item_caption(mp, item, app->canvas_editor->undo_name);
	if(app->canvas_editor->undo_count > 0)
	{
		t3f_set_menu_item_flags(mp, item, 0);
	}
	else
	{
		t3f_set_menu_item_flags(mp, item, ALLEGRO_MENU_ITEM_DISABLED);
	}
	return 0;
}

static int menu_redo_update_proc(ALLEGRO_MENU * mp, int item, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	al_set_menu_item_caption(mp, item, app->canvas_editor->redo_name);
	if(app->canvas_editor->redo_count > 0)
	{
		t3f_set_menu_item_flags(mp, item, 0);
	}
	else
	{
		t3f_set_menu_item_flags(mp, item, ALLEGRO_MENU_ITEM_DISABLED);
	}
	return 0;
}

static int menu_frame_add_update_proc(ALLEGRO_MENU * mp, int item, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	if(app->canvas_editor->selection.box.width > 0 && app->canvas_editor->selection.box.height > 0)
	{
		t3f_set_menu_item_flags(mp, item, 0);
	}
	else
	{
		t3f_set_menu_item_flags(mp, item, ALLEGRO_MENU_ITEM_DISABLED);
	}
	return 0;
}

static int menu_frame_delete_update_proc(ALLEGRO_MENU * mp, int item, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	if(app->canvas_editor->canvas->frame_max > 0)
	{
		t3f_set_menu_item_flags(mp, item, 0);
	}
	else
	{
		t3f_set_menu_item_flags(mp, item, ALLEGRO_MENU_ITEM_DISABLED);
	}
	return 0;
}

static int menu_layer_delete_update_proc(ALLEGRO_MENU * mp, int item, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	if(app->canvas_editor->canvas->layer_max > 1)
	{
		t3f_set_menu_item_flags(mp, item, 0);
	}
	else
	{
		t3f_set_menu_item_flags(mp, item, ALLEGRO_MENU_ITEM_DISABLED);
	}
	return 0;
}

bool quixel_setup_menus(QUIXEL_UI * uip)
{
	uip->menu[QUIXEL_UI_MENU_FILE] = al_create_menu();
	if(!uip->menu[QUIXEL_UI_MENU_FILE])
	{
		return false;
	}
	t3f_add_menu_item(uip->menu[QUIXEL_UI_MENU_FILE], "New", 0, NULL, quixel_menu_file_new, menu_base_update_proc);
	t3f_add_menu_item(uip->menu[QUIXEL_UI_MENU_FILE], "Load", 0, NULL, quixel_menu_file_load, menu_base_update_proc);
	t3f_add_menu_item(uip->menu[QUIXEL_UI_MENU_FILE], "Save", 0, NULL, quixel_menu_file_save, menu_base_update_proc);
	t3f_add_menu_item(uip->menu[QUIXEL_UI_MENU_FILE], "Save As", 0, NULL, quixel_menu_file_save_as, menu_base_update_proc);
	t3f_add_menu_item(uip->menu[QUIXEL_UI_MENU_FILE], NULL, 0, NULL, NULL, NULL);
	t3f_add_menu_item(uip->menu[QUIXEL_UI_MENU_FILE], "Import", 0, NULL, quixel_menu_file_import, menu_base_update_proc);
	t3f_add_menu_item(uip->menu[QUIXEL_UI_MENU_FILE], "Export", 0, NULL, quixel_menu_file_export, menu_base_update_proc);
	#ifndef ALLEGRO_MACOSX
		t3f_add_menu_item(uip->menu[QUIXEL_UI_MENU_FILE], NULL, 0, NULL, NULL, NULL);
		t3f_add_menu_item(uip->menu[QUIXEL_UI_MENU_FILE], "Exit", 0, NULL, quixel_menu_file_exit, menu_base_update_proc);
	#endif

	uip->menu[QUIXEL_UI_MENU_EDIT] = al_create_menu();
	if(!uip->menu[QUIXEL_UI_MENU_EDIT])
	{
		return false;
	}
	t3f_add_menu_item(uip->menu[QUIXEL_UI_MENU_EDIT], "Undo", ALLEGRO_MENU_ITEM_DISABLED, NULL, quixel_menu_edit_undo, menu_undo_update_proc);
	t3f_add_menu_item(uip->menu[QUIXEL_UI_MENU_EDIT], "Redo", ALLEGRO_MENU_ITEM_DISABLED, NULL, quixel_menu_edit_redo, menu_redo_update_proc);
	t3f_add_menu_item(uip->menu[QUIXEL_UI_MENU_EDIT], NULL, 0, NULL, NULL, NULL);
	t3f_add_menu_item(uip->menu[QUIXEL_UI_MENU_EDIT], "Cut", 0, NULL, quixel_menu_edit_cut, menu_base_update_proc);
	t3f_add_menu_item(uip->menu[QUIXEL_UI_MENU_EDIT], "Copy", 0, NULL, quixel_menu_edit_copy, menu_base_update_proc);
	t3f_add_menu_item(uip->menu[QUIXEL_UI_MENU_EDIT], "Paste", 0, NULL, quixel_menu_edit_paste, menu_base_update_proc);

	uip->menu[QUIXEL_UI_MENU_FRAME_ADD_ICON_TEMPLATE] = al_create_menu();
	if(!uip->menu[QUIXEL_UI_MENU_FRAME_ADD_ICON_TEMPLATE])
	{
		return false;
	}
	t3f_add_menu_item(uip->menu[QUIXEL_UI_MENU_FRAME_ADD_ICON_TEMPLATE], "8x8", 0, NULL, quixel_menu_frame_add_8x8, menu_base_update_proc);
	t3f_add_menu_item(uip->menu[QUIXEL_UI_MENU_FRAME_ADD_ICON_TEMPLATE], "12x12", 0, NULL, quixel_menu_frame_add_12x12, menu_base_update_proc);
	t3f_add_menu_item(uip->menu[QUIXEL_UI_MENU_FRAME_ADD_ICON_TEMPLATE], "16x16", 0, NULL, quixel_menu_frame_add_16x16, menu_base_update_proc);
	t3f_add_menu_item(uip->menu[QUIXEL_UI_MENU_FRAME_ADD_ICON_TEMPLATE], "24x24", 0, NULL, quixel_menu_frame_add_24x24, menu_base_update_proc);
	t3f_add_menu_item(uip->menu[QUIXEL_UI_MENU_FRAME_ADD_ICON_TEMPLATE], "32x32", 0, NULL, quixel_menu_frame_add_32x32, menu_base_update_proc);
	t3f_add_menu_item(uip->menu[QUIXEL_UI_MENU_FRAME_ADD_ICON_TEMPLATE], "48x48", 0, NULL, quixel_menu_frame_add_48x48, menu_base_update_proc);
	t3f_add_menu_item(uip->menu[QUIXEL_UI_MENU_FRAME_ADD_ICON_TEMPLATE], "64x64", 0, NULL, quixel_menu_frame_add_64x64, menu_base_update_proc);
	t3f_add_menu_item(uip->menu[QUIXEL_UI_MENU_FRAME_ADD_ICON_TEMPLATE], "72x72", 0, NULL, quixel_menu_frame_add_72x72, menu_base_update_proc);
	t3f_add_menu_item(uip->menu[QUIXEL_UI_MENU_FRAME_ADD_ICON_TEMPLATE], "96x96", 0, NULL, quixel_menu_frame_add_96x96, menu_base_update_proc);
	t3f_add_menu_item(uip->menu[QUIXEL_UI_MENU_FRAME_ADD_ICON_TEMPLATE], "128x128", 0, NULL, quixel_menu_frame_add_128x128, menu_base_update_proc);
	t3f_add_menu_item(uip->menu[QUIXEL_UI_MENU_FRAME_ADD_ICON_TEMPLATE], "256x256", 0, NULL, quixel_menu_frame_add_256x256, menu_base_update_proc);
	t3f_add_menu_item(uip->menu[QUIXEL_UI_MENU_FRAME_ADD_ICON_TEMPLATE], "512x512", 0, NULL, quixel_menu_frame_add_512x512, menu_base_update_proc);

	uip->menu[QUIXEL_UI_MENU_FRAME_ADD_DISPLAY_TEMPLATE] = al_create_menu();
	if(!uip->menu[QUIXEL_UI_MENU_FRAME_ADD_DISPLAY_TEMPLATE])
	{
		return false;
	}
	t3f_add_menu_item(uip->menu[QUIXEL_UI_MENU_FRAME_ADD_DISPLAY_TEMPLATE], "256x224", 0, NULL, quixel_menu_frame_add_256x224, menu_base_update_proc);
	t3f_add_menu_item(uip->menu[QUIXEL_UI_MENU_FRAME_ADD_DISPLAY_TEMPLATE], "320x200", 0, NULL, quixel_menu_frame_add_320x200, menu_base_update_proc);
	t3f_add_menu_item(uip->menu[QUIXEL_UI_MENU_FRAME_ADD_DISPLAY_TEMPLATE], "320x240", 0, NULL, quixel_menu_frame_add_320x240, menu_base_update_proc);
	t3f_add_menu_item(uip->menu[QUIXEL_UI_MENU_FRAME_ADD_DISPLAY_TEMPLATE], "640x480", 0, NULL, quixel_menu_frame_add_640x480, menu_base_update_proc);

	uip->menu[QUIXEL_UI_MENU_FRAME_ADD] = al_create_menu();
	if(!uip->menu[QUIXEL_UI_MENU_FRAME_ADD])
	{
		return false;
	}
	t3f_add_menu_item(uip->menu[QUIXEL_UI_MENU_FRAME_ADD], "from Selection", ALLEGRO_MENU_ITEM_DISABLED, NULL, quixel_menu_frame_add_from_selection, menu_frame_add_update_proc);
	t3f_add_menu_item(uip->menu[QUIXEL_UI_MENU_FRAME_ADD], NULL, 0, NULL, NULL, NULL);
	t3f_add_menu_item(uip->menu[QUIXEL_UI_MENU_FRAME_ADD], "Icon Template", 0, uip->menu[QUIXEL_UI_MENU_FRAME_ADD_ICON_TEMPLATE], NULL, NULL);
	t3f_add_menu_item(uip->menu[QUIXEL_UI_MENU_FRAME_ADD], "Display Template", 0, uip->menu[QUIXEL_UI_MENU_FRAME_ADD_DISPLAY_TEMPLATE], NULL, NULL);

	uip->menu[QUIXEL_UI_MENU_FRAME] = al_create_menu();
	if(!uip->menu[QUIXEL_UI_MENU_FRAME])
	{
		return false;
	}
	t3f_add_menu_item(uip->menu[QUIXEL_UI_MENU_FRAME], "Add", 0, uip->menu[QUIXEL_UI_MENU_FRAME_ADD], NULL, NULL);
	t3f_add_menu_item(uip->menu[QUIXEL_UI_MENU_FRAME], "Delete", ALLEGRO_MENU_ITEM_DISABLED, NULL, quixel_menu_frame_delete, menu_frame_delete_update_proc);
	t3f_add_menu_item(uip->menu[QUIXEL_UI_MENU_FRAME], NULL, 0, NULL, NULL, NULL);
	t3f_add_menu_item(uip->menu[QUIXEL_UI_MENU_FRAME], "Previous", ALLEGRO_MENU_ITEM_DISABLED, NULL, quixel_menu_frame_previous, menu_frame_delete_update_proc);
	t3f_add_menu_item(uip->menu[QUIXEL_UI_MENU_FRAME], "Next", ALLEGRO_MENU_ITEM_DISABLED, NULL, quixel_menu_frame_next, menu_frame_delete_update_proc);

	uip->menu[QUIXEL_UI_MENU_LAYER] = al_create_menu();
	if(!uip->menu[QUIXEL_UI_MENU_LAYER])
	{
		return false;
	}
	t3f_add_menu_item(uip->menu[QUIXEL_UI_MENU_LAYER], "Add", 0, NULL, quixel_menu_layer_add, menu_base_update_proc);
	t3f_add_menu_item(uip->menu[QUIXEL_UI_MENU_LAYER], "Delete", ALLEGRO_MENU_ITEM_DISABLED, NULL, quixel_menu_layer_delete, menu_layer_delete_update_proc);
	t3f_add_menu_item(uip->menu[QUIXEL_UI_MENU_LAYER], NULL, 0, NULL, NULL, NULL);
	t3f_add_menu_item(uip->menu[QUIXEL_UI_MENU_LAYER], "Previous", ALLEGRO_MENU_ITEM_DISABLED, NULL, quixel_menu_layer_previous, menu_layer_delete_update_proc);
	t3f_add_menu_item(uip->menu[QUIXEL_UI_MENU_LAYER], "Next", ALLEGRO_MENU_ITEM_DISABLED, NULL, quixel_menu_layer_next, menu_layer_delete_update_proc);

	uip->menu[QUIXEL_UI_MENU_MAIN] = al_create_menu();
	if(!uip->menu[QUIXEL_UI_MENU_MAIN])
	{
		return false;
	}

	t3f_add_menu_item(uip->menu[QUIXEL_UI_MENU_MAIN], "File", 0, uip->menu[QUIXEL_UI_MENU_FILE], NULL, NULL);
	t3f_add_menu_item(uip->menu[QUIXEL_UI_MENU_MAIN], "Edit", 0, uip->menu[QUIXEL_UI_MENU_EDIT], NULL, NULL);
	t3f_add_menu_item(uip->menu[QUIXEL_UI_MENU_MAIN], "Frame", 0, uip->menu[QUIXEL_UI_MENU_FRAME], NULL, NULL);
	t3f_add_menu_item(uip->menu[QUIXEL_UI_MENU_MAIN], "Layer", 0, uip->menu[QUIXEL_UI_MENU_LAYER], NULL, NULL);
	t3f_refresh_menus();

	return true;
}

void quixel_destroy_menus(QUIXEL_UI * uip)
{
	int i;

	for(i = QUIXEL_UI_MAX_MENUS - 1; i >= 0; i--)
	{
		if(uip->menu[i])
		{
			al_destroy_menu(uip->menu[i]);
			uip->menu[i] = NULL;
		}
	}
	t3f_reset_menus();
}

void quixel_update_undo_menu(QUIXEL_UI * uip, const char * undo_name, const char * redo_name)
{
	al_set_menu_item_caption(uip->menu[QUIXEL_UI_MENU_EDIT], 1, redo_name);
}
