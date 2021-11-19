#ifndef PA_MENU_FILE_H
#define PA_MENU_FILE_H

#include "t3f/t3f.h"

bool pa_can_export(PA_CANVAS_EDITOR * cep);
bool pa_can_reexport(PA_CANVAS_EDITOR * cep);
bool pa_can_reexport_all(PA_CANVAS_EDITOR * cep);
ALLEGRO_MENU * pa_create_file_menu(void);

#endif
