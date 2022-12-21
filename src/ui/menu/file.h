#ifndef PA_MENU_FILE_H
#define PA_MENU_FILE_H

#include "t3f/t3f.h"
#include "ui/ui.h"

bool pa_can_export(PA_CANVAS_EDITOR * cep);
bool pa_can_reexport(PA_CANVAS_EDITOR * cep);
bool pa_can_reexport_all(PA_CANVAS_EDITOR * cep);
bool pa_resave_allowed(PA_CANVAS_EDITOR * cep);
ALLEGRO_MENU * pa_create_file_menu(ALLEGRO_MENU * recent_menu);
ALLEGRO_MENU * pa_create_load_recent_menu(PA_UI * uip);

#endif
