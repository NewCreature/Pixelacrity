#include "t3f/t3f.h"
#include "instance.h"
#include "menu.h"
#include "palette_proc.h"

ALLEGRO_MENU * pa_create_palette_menu(void)
{
	ALLEGRO_MENU * mp;

	mp = al_create_menu();
	if(!mp)
	{
		return NULL;
	}
	t3f_add_menu_item(mp, "Reset to Default", 0, NULL, pa_menu_palette_reset, pa_menu_base_update_proc);
	t3f_add_menu_item(mp, NULL, 0, NULL, NULL, NULL);
	t3f_add_menu_item(mp, "Load", 0, NULL, pa_menu_palette_load, pa_menu_base_update_proc);
	t3f_add_menu_item(mp, "Save", 0, NULL, pa_menu_palette_save, pa_menu_base_update_proc);

	return mp;
}
