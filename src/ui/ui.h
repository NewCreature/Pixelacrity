#ifndef PA_UI_H
#define PA_UI_H

#include "t3f/t3f.h"
#include "t3gui/t3gui.h"
#include "modules/canvas/canvas.h"
#include "canvas_editor/canvas_editor.h"
#include "theme.h"
#include "dialog.h"
#include "ui_defines.h"

typedef struct
{

	ALLEGRO_MENU * menu[PA_UI_MAX_MENUS];
	int tool_menu_item[16];
	int * load_recent_menu_item;
	int load_recent_menu_item_size;
	const char * load_recent_fn;

	PA_DIALOG * main_dialog;
	PA_DIALOG * brush_popup_dialog;
	PA_DIALOG * color_popup_dialog;

	/* color drag and drop helpers */
	T3GUI_ELEMENT * click_ep;
	T3GUI_ELEMENT * hover_ep;
	T3GUI_ELEMENT * floating_ep;

} PA_UI;

PA_UI * pa_create_ui(PA_CANVAS_EDITOR * cep);
void pa_destroy_ui(PA_UI * uip);

void pa_process_ui(PA_UI * uip);
void pa_render_ui(PA_UI * uip);

#endif
