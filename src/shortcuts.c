#include "t3f/t3f.h"
#include "instance.h"
#include "ui/menu/menu_edit_proc.h"

void pa_handle_shortcuts(APP_INSTANCE * app)
{
	if((t3f_key[ALLEGRO_KEY_LCTRL] || t3f_key[ALLEGRO_KEY_RCTRL] || t3f_key[ALLEGRO_KEY_COMMAND]) && t3f_key[ALLEGRO_KEY_X])
	{
		quixel_menu_edit_cut(0, app);
		t3f_key[ALLEGRO_KEY_X] = 0;
	}
	else if((t3f_key[ALLEGRO_KEY_LCTRL] || t3f_key[ALLEGRO_KEY_RCTRL] || t3f_key[ALLEGRO_KEY_COMMAND]) && t3f_key[ALLEGRO_KEY_C])
	{
		quixel_menu_edit_copy(0, app);
		t3f_key[ALLEGRO_KEY_C] = 0;
	}
	if((t3f_key[ALLEGRO_KEY_LCTRL] || t3f_key[ALLEGRO_KEY_RCTRL] || t3f_key[ALLEGRO_KEY_COMMAND]) && t3f_key[ALLEGRO_KEY_V])
	{
		quixel_menu_edit_paste(0, app);
		t3f_key[ALLEGRO_KEY_V] = 0;
	}
}
