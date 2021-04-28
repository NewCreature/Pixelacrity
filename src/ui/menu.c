#include "t3f/t3f.h"
#include "ui.h"
#include "menu_file_proc.h"
#include "menu_edit_proc.h"
#include "menu_frame_proc.h"
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

	uip->menu[QUIXEL_UI_MENU_FRAME] = al_create_menu();
	if(!uip->menu[QUIXEL_UI_MENU_FRAME])
	{
		return false;
	}
	t3f_add_menu_item(uip->menu[QUIXEL_UI_MENU_FRAME], "Add", ALLEGRO_MENU_ITEM_DISABLED, NULL, quixel_menu_frame_add, menu_frame_add_update_proc);
	t3f_add_menu_item(uip->menu[QUIXEL_UI_MENU_FRAME], "Delete", ALLEGRO_MENU_ITEM_DISABLED, NULL, quixel_menu_frame_delete, menu_frame_delete_update_proc);
	t3f_add_menu_item(uip->menu[QUIXEL_UI_MENU_FRAME], NULL, 0, NULL, NULL, NULL);
	t3f_add_menu_item(uip->menu[QUIXEL_UI_MENU_FRAME], "Previous", ALLEGRO_MENU_ITEM_DISABLED, NULL, quixel_menu_frame_previous, menu_frame_delete_update_proc);
	t3f_add_menu_item(uip->menu[QUIXEL_UI_MENU_FRAME], "Next", ALLEGRO_MENU_ITEM_DISABLED, NULL, quixel_menu_frame_next, menu_frame_delete_update_proc);

	uip->menu[QUIXEL_UI_MENU_MAIN] = al_create_menu();
	if(!uip->menu[QUIXEL_UI_MENU_MAIN])
	{
		return false;
	}

	t3f_add_menu_item(uip->menu[QUIXEL_UI_MENU_MAIN], "File", 0, uip->menu[QUIXEL_UI_MENU_FILE], NULL, NULL);
	t3f_add_menu_item(uip->menu[QUIXEL_UI_MENU_MAIN], "Edit", 0, uip->menu[QUIXEL_UI_MENU_EDIT], NULL, NULL);
	t3f_add_menu_item(uip->menu[QUIXEL_UI_MENU_MAIN], "Frame", 0, uip->menu[QUIXEL_UI_MENU_FRAME], NULL, NULL);
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
