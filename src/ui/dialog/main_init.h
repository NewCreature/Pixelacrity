#ifndef PA_UI_DIALOG_MAIN_INIT_H
#define PA_UI_DIALOG_MAIN_INIT_H

#include "ui/dialog.h"
#include "ui/canvas_editor/canvas_editor.h"

PA_DIALOG * pa_create_main_dialog(PA_CANVAS_EDITOR * cep);

void pa_resize_main_dialog(PA_UI * uip);

#endif
