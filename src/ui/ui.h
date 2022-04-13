#ifndef PA_UI_H
#define PA_UI_H

#include "t3f/t3f.h"
#include "t3gui/t3gui.h"
#include "modules/canvas/canvas.h"
#include "canvas_editor/canvas_editor.h"
#include "theme.h"
#include "popup_dialog.h"
#include "ui_defines.h"

typedef struct
{

	ALLEGRO_BITMAP * bitmap[PA_UI_MAX_BITMAPS];
	ALLEGRO_MENU * menu[PA_UI_MAX_MENUS];
	T3GUI_DIALOG * dialog[PA_UI_MAX_DIALOGS];
	PA_UI_THEME * theme;
	int tool_menu_item[16];

	T3GUI_ELEMENT * element[PA_UI_MAX_DIALOG_ELEMENTS];
	T3GUI_ELEMENT * left_shade_picker_element[PA_UI_MAX_DIALOG_ELEMENTS];
	T3GUI_ELEMENT * left_alpha_picker_element[PA_UI_MAX_DIALOG_ELEMENTS];
	T3GUI_ELEMENT * right_shade_picker_element[PA_UI_MAX_DIALOG_ELEMENTS];
	T3GUI_ELEMENT * right_alpha_picker_element[PA_UI_MAX_DIALOG_ELEMENTS];
	T3GUI_ELEMENT * palette_color_element[PA_UI_MAX_DIALOG_ELEMENTS];

	char status_left_message[1024];
	char status_middle_message[1024];
	char status_right_message[1024];

	PA_POPUP_DIALOG * brush_popup_dialog;
	PA_POPUP_DIALOG * color_popup_dialog;

} PA_UI;

void pa_resize_ui(PA_UI * uip);
PA_UI * pa_create_ui(PA_CANVAS_EDITOR * cep);
void pa_destroy_ui(PA_UI * uip);

void pa_process_ui(PA_UI * uip);
void pa_render_ui(PA_UI * uip);

#endif
