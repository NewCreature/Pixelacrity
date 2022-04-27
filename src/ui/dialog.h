#ifndef PA_UI_DIALOG_H
#define PA_UI_DIALOG_H

#include "t3f/t3f.h"
#include "theme.h"
#include "ui_defines.h"

typedef struct
{

	ALLEGRO_DISPLAY * display;
	PA_UI_THEME * theme;
	ALLEGRO_BITMAP * bitmap[PA_UI_MAX_BITMAPS];
	T3GUI_DIALOG * dialog;
	char * edit_text[PA_UI_MAX_DIALOG_EDIT_BOXES];
	T3GUI_ELEMENT * element[PA_UI_MAX_DIALOG_ELEMENTS];
	int element_id[PA_UI_MAX_DIALOG_ELEMENTS];
	int element_count;
	int variable[PA_UI_MAX_DIALOG_VARIABLES];
	char string[PA_UI_MAX_DIALOG_STRINGS][1024];
	void * user_data;

} PA_DIALOG;

PA_DIALOG * pa_create_dialog(ALLEGRO_DISPLAY * dp, const char * theme_file, int w, int h, void * data);
void pa_close_dialog(PA_DIALOG * dp);

void pa_track_dialog_element(PA_DIALOG * dp, T3GUI_ELEMENT * ep, int id);
T3GUI_ELEMENT * pa_get_dialog_element(PA_DIALOG * dp, int id);

#endif
