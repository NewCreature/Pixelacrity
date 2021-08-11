#ifndef PA_UI_H
#define PA_UI_H

#include "t3f/t3f.h"
#include "t3gui/t3gui.h"
#include "modules/canvas/canvas.h"
#include "canvas_editor/canvas_editor.h"

#define PA_UI_MAX_BITMAPS 32
#define PA_UI_BITMAP_BG    0

#define PA_UI_MAX_MENUS                       32
#define PA_UI_MENU_MAIN                        0
#define PA_UI_MENU_FILE                        1
#define PA_UI_MENU_EDIT                        2
#define PA_UI_MENU_FRAME                       3
#define PA_UI_MENU_LAYER                       4
#define PA_UI_MENU_FRAME_ADD                   5
#define PA_UI_MENU_FRAME_ADD_ICON_TEMPLATE     6
#define PA_UI_MENU_FRAME_ADD_DISPLAY_TEMPLATE  7
#define PA_UI_MENU_EDIT_MULTILAYER             8

#define PA_UI_MAX_DIALOGS 32
#define PA_UI_DIALOG_MAIN  0

#define PA_UI_MAX_DIALOG_ELEMENTS            128
#define PA_UI_ELEMENT_LEFT_PANE                0
#define PA_UI_ELEMENT_LEFT_COLOR               1
#define PA_UI_ELEMENT_LEFT_SHADE_SLIDER        2
#define PA_UI_ELEMENT_LEFT_ALPHA_SLIDER        3
#define PA_UI_ELEMENT_RIGHT_COLOR              4
#define PA_UI_ELEMENT_RIGHT_SHADE_SLIDER       5
#define PA_UI_ELEMENT_RIGHT_ALPHA_SLIDER       6
#define PA_UI_ELEMENT_PALETTE                  7
#define PA_UI_ELEMENT_RIGHT_PANE               8
#define PA_UI_ELEMENT_BUTTON_PIXEL             9
#define PA_UI_ELEMENT_BUTTON_LINE             10
#define PA_UI_ELEMENT_BUTTON_RECTANGLE        11
#define PA_UI_ELEMENT_BUTTON_FILLED_RECTANGLE 12
#define PA_UI_ELEMENT_BUTTON_OVAL             13
#define PA_UI_ELEMENT_BUTTON_FILLED_OVAL      14
#define PA_UI_ELEMENT_BUTTON_FLOOD_FILL       15
#define PA_UI_ELEMENT_BUTTON_DROPPER          16
#define PA_UI_ELEMENT_BUTTON_ERASER           17
#define PA_UI_ELEMENT_BUTTON_SELECTION        18
#define PA_UI_ELEMENT_LAYER_LIST              19
#define PA_UI_ELEMENT_BUTTON_ADD_LAYER        20
#define PA_UI_ELEMENT_BUTTON_REMOVE_LAYER     21
#define PA_UI_ELEMENT_STATUS_BAR              22
#define PA_UI_ELEMENT_STATUS_LEFT_MESSAGE     23
#define PA_UI_ELEMENT_STATUS_MIDDLE_MESSAGE   24
#define PA_UI_ELEMENT_STATUS_RIGHT_MESSAGE    25
#define PA_UI_ELEMENT_CANVAS_EDITOR           26

#define PA_UI_MAX_THEMES  32

#define PA_UI_ELEMENT_SPACE 8
#define PA_UI_BG_SCALE     24

typedef struct
{

	ALLEGRO_BITMAP * bitmap[PA_UI_MAX_BITMAPS];
	ALLEGRO_MENU * menu[PA_UI_MAX_MENUS];
	T3GUI_DIALOG * dialog[PA_UI_MAX_DIALOGS];
	T3GUI_THEME * theme[PA_UI_MAX_THEMES];

	T3GUI_ELEMENT * element[PA_UI_MAX_DIALOG_ELEMENTS];
	T3GUI_ELEMENT * color_picker_element[PA_UI_MAX_DIALOG_ELEMENTS];
	T3GUI_ELEMENT * palette_color_element[PA_UI_MAX_DIALOG_ELEMENTS];

	char status_left_message[1024];
	char status_middle_message[1024];
	char status_right_message[1024];

} PA_UI;

void pa_resize_ui(PA_UI * uip);
PA_UI * pa_create_ui(PA_CANVAS_EDITOR * cep);
void pa_destroy_ui(PA_UI * uip);

void pa_process_ui(PA_UI * uip);
void pa_render_ui(PA_UI * uip);

#endif
