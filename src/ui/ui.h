#ifndef PA_UI_H
#define PA_UI_H

#include "t3f/t3f.h"
#include "t3gui/t3gui.h"
#include "modules/canvas/canvas.h"
#include "canvas_editor/canvas_editor.h"
#include "theme.h"
#include "popup_dialog.h"

#define PA_UI_MAX_BITMAPS                  48
#define PA_UI_BITMAP_BG                     0
#define PA_UI_BITMAP_TOOL_PIXEL             1
#define PA_UI_BITMAP_TOOL_LINE              2
#define PA_UI_BITMAP_TOOL_RECTANGLE         3
#define PA_UI_BITMAP_TOOL_FILLED_RECTANGLE  4
#define PA_UI_BITMAP_TOOL_OVAL              5
#define PA_UI_BITMAP_TOOL_FILLED_OVAL       6
#define PA_UI_BITMAP_TOOL_FLOOD             7
#define PA_UI_BITMAP_TOOL_REPLACE           8
#define PA_UI_BITMAP_TOOL_ERASER            9
#define PA_UI_BITMAP_TOOL_DROPPER          10
#define PA_UI_BITMAP_TOOL_SELECTION        11
#define PA_UI_BITMAP_TOOL_FRAME            12
#define PA_UI_BITMAP_TOOL_OUTLINE          13
#define PA_UI_BITMAP_TOOL_COMPOSITE        14
#define PA_UI_BITMAP_TOOLBAR_NEW           15
#define PA_UI_BITMAP_TOOLBAR_OPEN          16
#define PA_UI_BITMAP_TOOLBAR_SAVE          17
#define PA_UI_BITMAP_TOOLBAR_IMPORT        18
#define PA_UI_BITMAP_TOOLBAR_EXPORT        19
#define PA_UI_BITMAP_TOOLBAR_UNDO          20
#define PA_UI_BITMAP_TOOLBAR_REDO          21
#define PA_UI_BITMAP_TOOLBAR_CUT           22
#define PA_UI_BITMAP_TOOLBAR_COPY          23
#define PA_UI_BITMAP_TOOLBAR_PASTE         24
#define PA_UI_BITMAP_TOOLBAR_TURN_CW       25
#define PA_UI_BITMAP_TOOLBAR_TURN_CCW      26
#define PA_UI_BITMAP_TOOLBAR_FLIP_H        27
#define PA_UI_BITMAP_TOOLBAR_FLIP_V        28
#define PA_UI_BITMAP_LAYER_ADD             29
#define PA_UI_BITMAP_LAYER_REMOVE          30
#define PA_UI_BITMAP_BOX_HANDLE            31
#define PA_UI_BITMAP_SELECTION_SINGLE      32
#define PA_UI_BITMAP_SELECTION_MULTI       33

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
#define PA_UI_MENU_TOOL                        9
#define PA_UI_MENU_TOOL_BRUSH                 10
#define PA_UI_MENU_PALETTE                    11

#define PA_UI_MAX_DIALOGS 32
#define PA_UI_DIALOG_MAIN  0

#define PA_UI_MAX_DIALOG_ELEMENTS                256
#define PA_UI_ELEMENT_LEFT_PANE                    0
#define PA_UI_ELEMENT_LEFT_COLOR                   1
#define PA_UI_ELEMENT_LEFT_SHADE_SLIDER            2
#define PA_UI_ELEMENT_LEFT_ALPHA_SLIDER            3
#define PA_UI_ELEMENT_RIGHT_COLOR                  4
#define PA_UI_ELEMENT_RIGHT_SHADE_SLIDER           5
#define PA_UI_ELEMENT_RIGHT_ALPHA_SLIDER           6
#define PA_UI_ELEMENT_PALETTE                      7

#define PA_UI_ELEMENT_RIGHT_PANE                   8
#define PA_UI_ELEMENT_BUTTON_PIXEL                 9
#define PA_UI_ELEMENT_BUTTON_LINE                 10
#define PA_UI_ELEMENT_BUTTON_RECTANGLE            11
#define PA_UI_ELEMENT_BUTTON_FILLED_RECTANGLE     12
#define PA_UI_ELEMENT_BUTTON_OVAL                 13
#define PA_UI_ELEMENT_BUTTON_FILLED_OVAL          14
#define PA_UI_ELEMENT_BUTTON_FLOOD_FILL           15
#define PA_UI_ELEMENT_BUTTON_REPLACE              16
#define PA_UI_ELEMENT_BUTTON_DROPPER              17
#define PA_UI_ELEMENT_BUTTON_ERASER               18
#define PA_UI_ELEMENT_BUTTON_SELECTION            19
#define PA_UI_ELEMENT_BUTTON_FRAME                20
#define PA_UI_ELEMENT_LAYER_LIST                  21
#define PA_UI_ELEMENT_BUTTON_ADD_LAYER            22
#define PA_UI_ELEMENT_BUTTON_REMOVE_LAYER         23
#define PA_UI_ELEMENT_MAP                         24

#define PA_UI_ELEMENT_STATUS_BAR                  25
#define PA_UI_ELEMENT_STATUS_LEFT_MESSAGE         26
#define PA_UI_ELEMENT_STATUS_MIDDLE_MESSAGE       27
#define PA_UI_ELEMENT_STATUS_RIGHT_MESSAGE        28
#define PA_UI_ELEMENT_CANVAS_EDITOR               29
#define PA_UI_ELEMENT_TOOLBAR                     30
#define PA_UI_ELEMENT_BUTTON_NEW                  31
#define PA_UI_ELEMENT_BUTTON_OPEN                 32
#define PA_UI_ELEMENT_BUTTON_SAVE                 33
#define PA_UI_ELEMENT_BUTTON_IMPORT               34
#define PA_UI_ELEMENT_BUTTON_EXPORT               35
#define PA_UI_ELEMENT_BUTTON_UNDO                 36
#define PA_UI_ELEMENT_BUTTON_REDO                 37
#define PA_UI_ELEMENT_BUTTON_CUT                  38
#define PA_UI_ELEMENT_BUTTON_COPY                 39
#define PA_UI_ELEMENT_BUTTON_PASTE                40
#define PA_UI_ELEMENT_BUTTON_TURN_CW              41
#define PA_UI_ELEMENT_BUTTON_TURN_CCW             42
#define PA_UI_ELEMENT_BUTTON_FLIP_HORIZONTAL      43
#define PA_UI_ELEMENT_BUTTON_FLIP_VERTICAL        44
#define PA_UI_ELEMENT_TOOLBAR_FILE_SEPARATOR      45
#define PA_UI_ELEMENT_TOOLBAR_UNDO_SEPARATOR      46
#define PA_UI_ELEMENT_TOOLBAR_CLIPBOARD_SEPARATOR 47
#define PA_UI_ELEMENT_BRUSH                       48
#define PA_UI_ELEMENT_BUTTON_TOOL_MODE_COMPOSITE  49
#define PA_UI_ELEMENT_BUTTON_TOOL_MODE_OUTLINE    50

#define PA_UI_MAX_THEMES  32

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

} PA_UI;

void pa_resize_ui(PA_UI * uip);
PA_UI * pa_create_ui(PA_CANVAS_EDITOR * cep);
void pa_destroy_ui(PA_UI * uip);

void pa_process_ui(PA_UI * uip);
void pa_render_ui(PA_UI * uip);

#endif
