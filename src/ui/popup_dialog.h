#ifndef PA_UI_POPUP_DIALOG_H
#define PA_UI_POPUP_DIALOG_H

#include "t3f/t3f.h"
#include "theme.h"
#include "ui_defines.h"

typedef struct
{

	ALLEGRO_DISPLAY * display;
	PA_UI_THEME * theme;
	T3GUI_DIALOG * dialog;
	char * edit_text[PA_UI_MAX_DIALOG_EDIT_BOXES];
	T3GUI_ELEMENT * element[PA_UI_MAX_DIALOG_ELEMENTS];

} PA_POPUP_DIALOG;

PA_POPUP_DIALOG * pa_create_popup_dialog(const char * theme_file, int w, int h, void * data);
void pa_close_popup_dialog(PA_POPUP_DIALOG * dp);

#endif
