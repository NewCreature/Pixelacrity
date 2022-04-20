#ifndef PA_COLOR_POPUP_DIALOG_H
#define PA_COLOR_POPUP_DIALOG_H

#include "popup_dialog.h"
#include "theme.h"

#define PA_COLOR_DIALOG_ELEMENT_R_SLIDER 0
#define PA_COLOR_DIALOG_ELEMENT_G_SLIDER 1
#define PA_COLOR_DIALOG_ELEMENT_B_SLIDER 2
#define PA_COLOR_DIALOG_ELEMENT_A_SLIDER 3

PA_POPUP_DIALOG * pa_create_color_editor_popup_dialog(ALLEGRO_COLOR * color, PA_UI_THEME * tp);

void pa_color_dialog_pre_logic(PA_POPUP_DIALOG * dp);
void pa_color_dialog_post_logic(PA_POPUP_DIALOG * dp);

#endif
