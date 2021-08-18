#include "t3f/t3f.h"
#include "instance.h"
#include "menu.h"
#include "file_proc.h"

static int menu_export_all_update_proc(ALLEGRO_MENU * mp, int item, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;
	int i;

	for(i = 0; i < app->canvas->frame_max; i++)
	{
		if(app->canvas->frame[i]->export_path)
		{
			t3f_set_menu_item_flags(mp, item, 0);
			break;
		}
	}
	if(i >= app->canvas->frame_max)
	{
		t3f_set_menu_item_flags(mp, item, ALLEGRO_MENU_ITEM_DISABLED);
	}
	return 0;
}

ALLEGRO_MENU * pa_create_file_menu(void)
{
	ALLEGRO_MENU * mp;

	mp = al_create_menu();
	if(!mp)
	{
		return NULL;
	}
	t3f_add_menu_item(mp, "New", 0, NULL, pa_menu_file_new, pa_menu_base_update_proc);
	t3f_add_menu_item(mp, "Load", 0, NULL, pa_menu_file_load, pa_menu_base_update_proc);
	t3f_add_menu_item(mp, "Save", 0, NULL, pa_menu_file_save, pa_menu_base_update_proc);
	t3f_add_menu_item(mp, "Save As", 0, NULL, pa_menu_file_save_as, pa_menu_base_update_proc);
	t3f_add_menu_item(mp, NULL, 0, NULL, NULL, NULL);
	t3f_add_menu_item(mp, "Import", 0, NULL, pa_menu_file_import, pa_menu_base_update_proc);
	t3f_add_menu_item(mp, NULL, 0, NULL, NULL, NULL);
	t3f_add_menu_item(mp, "Export", 0, NULL, pa_menu_file_export, pa_menu_base_update_proc);
	t3f_add_menu_item(mp, "Export All", 0, NULL, pa_menu_file_export_all, menu_export_all_update_proc);
	t3f_add_menu_item(mp, "Export As", 0, NULL, pa_menu_file_export_as, pa_menu_base_update_proc);
	#ifndef ALLEGRO_MACOSX
		t3f_add_menu_item(mp, NULL, 0, NULL, NULL, NULL);
		t3f_add_menu_item(mp, "Exit", 0, NULL, pa_menu_file_exit, pa_menu_base_update_proc);
	#endif

	return mp;
}
