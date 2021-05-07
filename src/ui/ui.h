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

#define QUIXEL_UI_MAX_DIALOG_ELEMENTS            128
#define QUIXEL_UI_ELEMENT_LEFT_PANE                0
#define QUIXEL_UI_ELEMENT_LEFT_COLOR               1
#define QUIXEL_UI_ELEMENT_LEFT_SHADE_SLIDER        2
#define QUIXEL_UI_ELEMENT_LEFT_ALPHA_SLIDER        3
#define QUIXEL_UI_ELEMENT_RIGHT_COLOR              4
#define QUIXEL_UI_ELEMENT_RIGHT_SHADE_SLIDER       5
#define QUIXEL_UI_ELEMENT_RIGHT_ALPHA_SLIDER       6
#define QUIXEL_UI_ELEMENT_PALETTE                  7
#define QUIXEL_UI_ELEMENT_RIGHT_PANE               8
#define QUIXEL_UI_ELEMENT_BUTTON_PIXEL             9
#define QUIXEL_UI_ELEMENT_BUTTON_LINE             10
#define QUIXEL_UI_ELEMENT_BUTTON_RECTANGLE        11
#define QUIXEL_UI_ELEMENT_BUTTON_FILLED_RECTANGLE 12
#define QUIXEL_UI_ELEMENT_BUTTON_OVAL             13
#define QUIXEL_UI_ELEMENT_BUTTON_FILLED_OVAL      14
#define QUIXEL_UI_ELEMENT_BUTTON_FLOOD_FILL       15
#define QUIXEL_UI_ELEMENT_BUTTON_DROPPER          16
#define QUIXEL_UI_ELEMENT_BUTTON_SELECTION        17
#define QUIXEL_UI_ELEMENT_LAYER_LIST              18
#define QUIXEL_UI_ELEMENT_BUTTON_ADD_LAYER        19
#define QUIXEL_UI_ELEMENT_BUTTON_REMOVE_LAYER     20
#define QUIXEL_UI_ELEMENT_STATUS_BAR              21
#define QUIXEL_UI_ELEMENT_STATUS_LEFT_MESSAGE     22
#define QUIXEL_UI_ELEMENT_STATUS_MIDDLE_MESSAGE   23
#define QUIXEL_UI_ELEMENT_STATUS_RIGHT_MESSAGE    24
#define QUIXEL_UI_ELEMENT_CANVAS_EDITOR           25

#define QUIXEL_UI_MAX_THEMES  32

#define QUIXEL_UI_ELEMENT_SPACE 8
#define QUIXEL_UI_BG_SCALE     24

typedef struct
{

	ALLEGRO_BITMAP * bitmap[QUIXEL_UI_MAX_BITMAPS];
	ALLEGRO_MENU * menu[QUIXEL_UI_MAX_MENUS];
	T3GUI_DIALOG * dialog[QUIXEL_UI_MAX_DIALOGS];
	T3GUI_THEME * theme[QUIXEL_UI_MAX_THEMES];

	T3GUI_ELEMENT * element[QUIXEL_UI_MAX_DIALOG_ELEMENTS];
	T3GUI_ELEMENT * color_picker_element[QUIXEL_UI_MAX_DIALOG_ELEMENTS];
	T3GUI_ELEMENT * palette_color_element[QUIXEL_UI_MAX_DIALOG_ELEMENTS];

	char status_left_message[1024];
	char status_middle_message[1024];
	char status_right_message[1024];

} QUIXEL_UI;

void quixel_resize_ui(QUIXEL_UI * uip);
QUIXEL_UI * quixel_create_ui(QUIXEL_CANVAS_EDITOR * cep);
void quixel_destroy_ui(QUIXEL_UI * uip);

void quixel_process_ui(QUIXEL_UI * uip);
void quixel_render_ui(QUIXEL_UI * uip);

#endif
