#include "t3f/t3f.h"
#include "t3f/file_utils.h"
#include "instance.h"
#include "defines.h"
#include "modules/canvas/canvas_helpers.h"
#include "menu.h"
#include "file_proc.h"

bool pa_can_export(PA_CANVAS_EDITOR * cep)
{
	int x, y, w, h;

	if(cep->canvas->frame_max > 0)
	{
		return true;
	}
	else
	{
		pa_get_canvas_dimensions(cep->canvas, &x, &y, &w, &h, 0, false);
		if(w > 0 && h > 0)
		{
			return true;
		}
	}
	return false;
}

bool pa_can_reexport(PA_CANVAS_EDITOR * cep)
{
	if(cep->export_path || (cep->canvas->frame_max > 0 && cep->canvas->frame[cep->current_frame]->export_path))
	{
		return true;
	}
	return false;
}

bool pa_can_reexport_all(PA_CANVAS_EDITOR * cep)
{
	int i;

	for(i = 0; i < cep->canvas->frame_max; i++)
	{
		if(cep->canvas->frame[i]->export_path)
		{
			return true;
		}
	}
	return false;
}

bool pa_resave_allowed(PA_CANVAS_EDITOR * cep)
{
	const char * extension = t3f_get_path_extension(cep->canvas_path);

	if(!strcasecmp(extension, PA_CANVAS_FILE_EXTENSION))
	{
		return false;
	}
	if(!strcasecmp(extension, ".png") || !strcasecmp(extension, ".tga"))
	{
		return true;
	}
	return false;
}

static int menu_reexport_update_proc(ALLEGRO_MENU * mp, int item, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	if(pa_can_reexport(app->canvas_editor))
	{
		t3f_set_menu_item_flags(mp, item, 0);
	}
	else
	{
		t3f_set_menu_item_flags(mp, item, ALLEGRO_MENU_ITEM_DISABLED);
	}
	return 0;
}

static int menu_reexport_all_update_proc(ALLEGRO_MENU * mp, int item, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	if(pa_can_reexport_all(app->canvas_editor))
	{
		t3f_set_menu_item_flags(mp, item, 0);
	}
	else
	{
		t3f_set_menu_item_flags(mp, item, ALLEGRO_MENU_ITEM_DISABLED);
	}
	return 0;
}

static int menu_export_update_proc(ALLEGRO_MENU * mp, int item, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;
	int x, y, w, h;

	if(app->canvas->frame_max > 0)
	{
		t3f_set_menu_item_flags(mp, item, 0);
	}
	else
	{
		pa_get_canvas_dimensions(app->canvas, &x, &y, &w, &h, 0, false);
		if(w > 0 && h > 0)
		{
			t3f_set_menu_item_flags(mp, item, 0);
		}
		else
		{
			t3f_set_menu_item_flags(mp, item, ALLEGRO_MENU_ITEM_DISABLED);
		}
	}
	return 0;
}

static int menu_resave_update_proc(ALLEGRO_MENU * mp, int item, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;
	const char * extension;

	extension = t3f_get_path_extension(app->canvas_editor->canvas_path);
	if(!strcasecmp(extension, ".png") || !strcasecmp(extension, ".tga"))
	{
		t3f_set_menu_item_flags(mp, item, 0);
	}
	else
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
	t3f_add_menu_item(mp, "Re-Save Image", 0, NULL, pa_menu_file_resave_image, menu_resave_update_proc);
	t3f_add_menu_item(mp, NULL, 0, NULL, NULL, NULL);
	t3f_add_menu_item(mp, "Import", 0, NULL, pa_menu_file_import, pa_menu_base_update_proc);
	t3f_add_menu_item(mp, NULL, 0, NULL, NULL, NULL);
	t3f_add_menu_item(mp, "Export", 0, NULL, pa_menu_file_export, menu_export_update_proc);
	t3f_add_menu_item(mp, "Re-Export", 0, NULL, pa_menu_file_reexport, menu_reexport_update_proc);
	t3f_add_menu_item(mp, "Re-Export All", 0, NULL, pa_menu_file_reexport_all, menu_reexport_all_update_proc);
	#ifndef ALLEGRO_MACOSX
		t3f_add_menu_item(mp, NULL, 0, NULL, NULL, NULL);
		t3f_add_menu_item(mp, "Exit", 0, NULL, pa_menu_file_exit, pa_menu_base_update_proc);
	#endif

	return mp;
}
