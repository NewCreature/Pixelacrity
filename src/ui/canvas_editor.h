#ifndef QUIXEL_CANVAS_EDITOR_H
#define QUIXEL_CANVAS_EDITOR_H

#include "canvas.h"

#define QUIXEL_CANVAS_EDITOR_SIGNAL_NONE         0
#define QUIXEL_CANVAS_EDITOR_SIGNAL_DELETE_LAYER 1

#define QUIXEL_CANVAS_EDITOR_TOOL_PIXEL            0
#define QUIXEL_CANVAS_EDITOR_TOOL_LINE             1
#define QUIXEL_CANVAS_EDITOR_TOOL_RECTANGLE        2
#define QUIXEL_CANVAS_EDITOR_TOOL_FILLED_RECTANGLE 3
#define QUIXEL_CANVAS_EDITOR_TOOL_OVAL             4
#define QUIXEL_CANVAS_EDITOR_TOOL_FILLED_OVAL      5

typedef struct
{

	ALLEGRO_BITMAP * bg_tile;

	int bg_scale;
	int view_x, view_y;
	int view_zoom;
	int hover_x, hover_y;
	int current_layer;
	int current_tool;
	int tool_state;
	ALLEGRO_COLOR base_color;
	ALLEGRO_COLOR last_base_color;
	ALLEGRO_COLOR left_color;
	ALLEGRO_COLOR right_color;

	int signal;

} QUIXEL_CANVAS_EDITOR;

QUIXEL_CANVAS_EDITOR * quixel_create_canvas_editor(void);
void quixel_destroy_canvas_editor(QUIXEL_CANVAS_EDITOR * cep);

void quixel_canvas_editor_logic(QUIXEL_CANVAS_EDITOR * cep, QUIXEL_CANVAS * cp);
void quixel_canvas_editor_render(QUIXEL_CANVAS_EDITOR * cep, QUIXEL_CANVAS * cp);

#endif
