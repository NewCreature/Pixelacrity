#ifndef QUIXEL_UI_H
#define QUIXEL_UI_H

#include "t3f/t3f.h"
#include "t3gui/t3gui.h"
#include "canvas.h"
#include "canvas_editor.h"

#define QUIXEL_UI_MAX_BITMAPS    32

#define QUIXEL_UI_MAX_MENUS   32
#define QUIXEL_UI_MENU_MAIN    0
#define QUIXEL_UI_MENU_FILE    1

#define QUIXEL_UI_MAX_DIALOGS 32
#define QUIXEL_UI_DIALOG_MAIN  0

#define QUIXEL_UI_MAX_THEMES  32

typedef struct
{

	ALLEGRO_BITMAP * bitmap[QUIXEL_UI_MAX_BITMAPS];
	ALLEGRO_MENU * menu[QUIXEL_UI_MAX_MENUS];
	T3GUI_DIALOG * dialog[QUIXEL_UI_MAX_DIALOGS];
	T3GUI_THEME * theme[QUIXEL_UI_MAX_THEMES];

} QUIXEL_UI;

QUIXEL_UI * quixel_create_ui(QUIXEL_CANVAS_EDITOR * cep);
void quixel_destroy_ui(QUIXEL_UI * uip);

void quixel_process_ui(QUIXEL_UI * uip);
void quixel_render_ui(QUIXEL_UI * uip);

#endif
