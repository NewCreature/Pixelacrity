#ifndef PA_MENU_FRAME_H
#define PA_MENU_FRAME_H

#include "t3f/t3f.h"

ALLEGRO_MENU * pa_create_frame_icon_template_menu(void);
ALLEGRO_MENU * pa_create_frame_display_template_menu(void);
ALLEGRO_MENU * pa_create_frame_add_menu(ALLEGRO_MENU * icon_template_mp, ALLEGRO_MENU * display_template_mp);
ALLEGRO_MENU * pa_create_frame_menu(ALLEGRO_MENU * add_mp);

#endif
