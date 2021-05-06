#ifndef QUIXEL_UI_H
#define QUIXEL_UI_H

#include "t3f/t3f.h"
#include "t3gui/t3gui.h"
#include "modules/canvas/canvas.h"
#include "canvas_editor/canvas_editor.h"

#define QUIXEL_UI_MAX_BITMAPS 32
#define QUIXEL_UI_BITMAP_BG    0

#define QUIXEL_UI_MAX_MENUS                       32
#define QUIXEL_UI_MENU_MAIN                        0
#define QUIXEL_UI_MENU_FILE                        1
#define QUIXEL_UI_MENU_EDIT                        2
#define QUIXEL_UI_MENU_FRAME                       3
#define QUIXEL_UI_MENU_LAYER                       4
#define QUIXEL_UI_MENU_FRAME_ADD                   5
#define QUIXEL_UI_MENU_FRAME_ADD_ICON_TEMPLATE     6
#define QUIXEL_UI_MENU_FRAME_ADD_DISPLAY_TEMPLATE  7

#define QUIXEL_UI_MAX_DIALOGS 32
#define QUIXEL_UI_DIALOG_MAIN  0

#define QUIXEL_UI_MAX_THEMES  32

#define QUIXEL_UI_ELEMENT_SPACE 8
#define QUIXEL_UI_BG_SCALE     24

typedef struct
{

	ALLEGRO_BITMAP * bitmap[QUIXEL_UI_MAX_BITMAPS];
	ALLEGRO_MENU * menu[QUIXEL_UI_MAX_MENUS];
	T3GUI_DIALOG * dialog[QUIXEL_UI_MAX_DIALOGS];
	T3GUI_THEME * theme[QUIXEL_UI_MAX_THEMES];

	T3GUI_ELEMENT * canvas_editor_element;
	T3GUI_ELEMENT * layer_list_element;
	T3GUI_ELEMENT * layer_remove_button_element;

	char status_left_message[1024];
	char status_middle_message[1024];
	char status_right_message[1024];

} QUIXEL_UI;

QUIXEL_UI * quixel_create_ui(QUIXEL_CANVAS_EDITOR * cep);
void quixel_destroy_ui(QUIXEL_UI * uip);

void quixel_process_ui(QUIXEL_UI * uip);
void quixel_render_ui(QUIXEL_UI * uip);

#endif
