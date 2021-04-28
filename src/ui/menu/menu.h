#ifndef QUIXEL_MENU_H
#define QUIXEL_MENU_H

#include "ui/ui.h"

bool quixel_setup_menus(QUIXEL_UI * uip);
void quixel_destroy_menus(QUIXEL_UI * uip);

void quixel_update_undo_menu(QUIXEL_UI * uip, const char * undo_name, const char * redo_name);

#endif
