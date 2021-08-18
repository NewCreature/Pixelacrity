#include "t3f/t3f.h"
#include "instance.h"
#include "menu.h"
#include "layer_proc.h"

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

static int menu_layer_move_up_update_proc(ALLEGRO_MENU * mp, int item, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	if(app->canvas_editor->current_layer < app->canvas->layer_max - 1)
	{
		t3f_set_menu_item_flags(mp, item, 0);
	}
	else
	{
		t3f_set_menu_item_flags(mp, item, ALLEGRO_MENU_ITEM_DISABLED);
	}
	return 0;
}

static int menu_layer_move_down_update_proc(ALLEGRO_MENU * mp, int item, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	if(app->canvas_editor->current_layer > 0)
	{
		t3f_set_menu_item_flags(mp, item, 0);
	}
	else
	{
		t3f_set_menu_item_flags(mp, item, ALLEGRO_MENU_ITEM_DISABLED);
	}
	return 0;
}

ALLEGRO_MENU * pa_create_layer_menu(void)
{
	ALLEGRO_MENU * mp;

	mp = al_create_menu();
	if(!mp)
	{
		return NULL;
	}
	t3f_add_menu_item(mp, "Add", 0, NULL, pa_menu_layer_add, menu_base_update_proc);
	t3f_add_menu_item(mp, "Delete", ALLEGRO_MENU_ITEM_DISABLED, NULL, pa_menu_layer_delete, menu_layer_delete_update_proc);
	t3f_add_menu_item(mp, NULL, 0, NULL, NULL, NULL);
	t3f_add_menu_item(mp, "Move Up", ALLEGRO_MENU_ITEM_DISABLED, NULL, pa_menu_layer_move_up, menu_layer_move_up_update_proc);
	t3f_add_menu_item(mp, "Move Down", ALLEGRO_MENU_ITEM_DISABLED, NULL, pa_menu_layer_move_down, menu_layer_move_down_update_proc);
	t3f_add_menu_item(mp, NULL, 0, NULL, NULL, NULL);
	t3f_add_menu_item(mp, "Previous", ALLEGRO_MENU_ITEM_DISABLED, NULL, pa_menu_layer_previous, menu_layer_delete_update_proc);
	t3f_add_menu_item(mp, "Next", ALLEGRO_MENU_ITEM_DISABLED, NULL, pa_menu_layer_next, menu_layer_delete_update_proc);

	return mp;
}
