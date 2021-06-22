#ifndef PA_MENU_H
#define PA_MENU_H

#include "ui/ui.h"

bool pa_setup_menus(PA_UI * uip);
void pa_destroy_menus(PA_UI * uip);

void pa_update_undo_menu(PA_UI * uip, const char * undo_name, const char * redo_name);

#endif
