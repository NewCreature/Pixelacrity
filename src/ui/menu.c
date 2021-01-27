#include "t3f/t3f.h"
#include "ui.h"
#include "menu_file_proc.h"
#include "menu_edit_proc.h"

static int menu_base_update_proc(ALLEGRO_MENU * mp, int item, void * data)
{
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

	uip->menu[QUIXEL_UI_MENU_MAIN] = al_create_menu();
	if(!uip->menu[QUIXEL_UI_MENU_MAIN])
	{
		return false;
	}

	t3f_add_menu_item(uip->menu[QUIXEL_UI_MENU_MAIN], "File", 0, uip->menu[QUIXEL_UI_MENU_FILE], NULL, NULL);

	return true;
}

void quixel_destroy_menus(QUIXEL_UI * uip)
{
	int i;

	for(i = 0; i < QUIXEL_UI_MAX_MENUS; i++)
	{
		if(uip->menu[i])
		{
			al_destroy_menu(uip->menu[i]);
			uip->menu[i] = NULL;
		}
	}
}
