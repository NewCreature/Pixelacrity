#ifndef PA_COLOR_POPUP_DIALOG_H
#define PA_COLOR_POPUP_DIALOG_H

#include "ui/dialog.h"
#include "ui/theme.h"
#include "ui/canvas_editor/canvas_editor.h"

#define PA_COLOR_DIALOG_ELEMENT_R_SLIDER       0
#define PA_COLOR_DIALOG_ELEMENT_G_SLIDER       1
#define PA_COLOR_DIALOG_ELEMENT_B_SLIDER       2
#define PA_COLOR_DIALOG_ELEMENT_A_SLIDER       3
#define PA_COLOR_DIALOG_ELEMENT_COLOR          4
#define PA_COLOR_DIALOG_ELEMENT_R_UP_BUTTON    5
#define PA_COLOR_DIALOG_ELEMENT_R_DOWN_BUTTON  6
#define PA_COLOR_DIALOG_ELEMENT_G_UP_BUTTON    7
#define PA_COLOR_DIALOG_ELEMENT_G_DOWN_BUTTON  8
#define PA_COLOR_DIALOG_ELEMENT_B_UP_BUTTON    9
#define PA_COLOR_DIALOG_ELEMENT_B_DOWN_BUTTON 10
#define PA_COLOR_DIALOG_ELEMENT_A_UP_BUTTON   11
#define PA_COLOR_DIALOG_ELEMENT_A_DOWN_BUTTON 12

PA_DIALOG * pa_create_color_editor_popup_dialog(PA_COLOR_INFO * color_info, float ox, float oy);

void pa_color_dialog_pre_logic(PA_DIALOG * dp);
void pa_color_dialog_post_logic(PA_DIALOG * dp);

#endif
