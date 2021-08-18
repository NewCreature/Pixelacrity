#include "t3f/t3f.h"
#include "instance.h"
#include "menu.h"
#include "frame_proc.h"

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

ALLEGRO_MENU * pa_create_frame_icon_template_menu(void)
{
	ALLEGRO_MENU * mp;

	mp = al_create_menu();
	if(!mp)
	{
		return NULL;
	}
	t3f_add_menu_item(mp, "8x8", 0, NULL, pa_menu_frame_add_8x8, menu_base_update_proc);
	t3f_add_menu_item(mp, "12x12", 0, NULL, pa_menu_frame_add_12x12, menu_base_update_proc);
	t3f_add_menu_item(mp, "16x16", 0, NULL, pa_menu_frame_add_16x16, menu_base_update_proc);
	t3f_add_menu_item(mp, "24x24", 0, NULL, pa_menu_frame_add_24x24, menu_base_update_proc);
	t3f_add_menu_item(mp, "32x32", 0, NULL, pa_menu_frame_add_32x32, menu_base_update_proc);
	t3f_add_menu_item(mp, "48x48", 0, NULL, pa_menu_frame_add_48x48, menu_base_update_proc);
	t3f_add_menu_item(mp, "64x64", 0, NULL, pa_menu_frame_add_64x64, menu_base_update_proc);
	t3f_add_menu_item(mp, "72x72", 0, NULL, pa_menu_frame_add_72x72, menu_base_update_proc);
	t3f_add_menu_item(mp, "96x96", 0, NULL, pa_menu_frame_add_96x96, menu_base_update_proc);
	t3f_add_menu_item(mp, "128x128", 0, NULL, pa_menu_frame_add_128x128, menu_base_update_proc);
	t3f_add_menu_item(mp, "256x256", 0, NULL, pa_menu_frame_add_256x256, menu_base_update_proc);
	t3f_add_menu_item(mp, "512x512", 0, NULL, pa_menu_frame_add_512x512, menu_base_update_proc);

	return mp;
}

ALLEGRO_MENU * pa_create_frame_display_template_menu(void)
{
	ALLEGRO_MENU * mp;

	mp = al_create_menu();
	if(!mp)
	{
		return NULL;
	}
	t3f_add_menu_item(mp, "256x224", 0, NULL, pa_menu_frame_add_256x224, menu_base_update_proc);
	t3f_add_menu_item(mp, "320x200", 0, NULL, pa_menu_frame_add_320x200, menu_base_update_proc);
	t3f_add_menu_item(mp, "320x240", 0, NULL, pa_menu_frame_add_320x240, menu_base_update_proc);
	t3f_add_menu_item(mp, "640x480", 0, NULL, pa_menu_frame_add_640x480, menu_base_update_proc);

	return mp;
}

ALLEGRO_MENU * pa_create_frame_add_menu(ALLEGRO_MENU * icon_template_mp, ALLEGRO_MENU * display_template_mp)
{
	ALLEGRO_MENU * mp;

	mp = al_create_menu();
	if(!mp)
	{
		return NULL;
	}
	t3f_add_menu_item(mp, "from Selection", ALLEGRO_MENU_ITEM_DISABLED, NULL, pa_menu_frame_add_from_selection, menu_frame_add_update_proc);
	t3f_add_menu_item(mp, NULL, 0, NULL, NULL, NULL);
	t3f_add_menu_item(mp, "Icon Template", 0, icon_template_mp, NULL, NULL);
	t3f_add_menu_item(mp, "Display Template", 0, display_template_mp, NULL, NULL);

	return mp;
}

ALLEGRO_MENU * pa_create_frame_menu(ALLEGRO_MENU * add_mp)
{
	ALLEGRO_MENU * mp;

	mp = al_create_menu();
	if(!mp)
	{
		return NULL;
	}
	t3f_add_menu_item(mp, "Add", 0, add_mp, NULL, NULL);
	t3f_add_menu_item(mp, "Delete", ALLEGRO_MENU_ITEM_DISABLED, NULL, pa_menu_frame_delete, menu_frame_delete_update_proc);
	t3f_add_menu_item(mp, NULL, 0, NULL, NULL, NULL);
	t3f_add_menu_item(mp, "Previous", ALLEGRO_MENU_ITEM_DISABLED, NULL, pa_menu_frame_previous, menu_frame_delete_update_proc);
	t3f_add_menu_item(mp, "Next", ALLEGRO_MENU_ITEM_DISABLED, NULL, pa_menu_frame_next, menu_frame_delete_update_proc);

	return mp;
}
