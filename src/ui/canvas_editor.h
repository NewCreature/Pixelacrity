#ifndef QUIXEL_CANVAS_EDITOR_H
#define QUIXEL_CANVAS_EDITOR_H

#include "canvas.h"

#define QUIXEL_CANVAS_EDITOR_SIGNAL_NONE         0
#define QUIXEL_CANVAS_EDITOR_SIGNAL_DELETE_LAYER 1

typedef struct
{

	ALLEGRO_BITMAP * bg_tile;

	int bg_scale;
	int view_x, view_y;
	int view_zoom;
	int hover_x, hover_y;
	int current_layer;

	int signal;

} QUIXEL_CANVAS_EDITOR;

QUIXEL_CANVAS_EDITOR * quixel_create_canvas_editor(void);
void quixel_destroy_canvas_editor(QUIXEL_CANVAS_EDITOR * cep);

void quixel_canvas_editor_logic(QUIXEL_CANVAS_EDITOR * cep, QUIXEL_CANVAS * cp);
void quixel_canvas_editor_render(QUIXEL_CANVAS_EDITOR * cep, QUIXEL_CANVAS * cp);

#endif
