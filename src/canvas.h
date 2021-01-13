#ifndef QUIXEL_CANVAS_H
#define QUIXEL_CANVAS_H

#include "t3f/t3f.h"

#define QUIXEL_CANVAS_MAX_WIDTH  32
#define QUIXEL_CANVAS_MAX_HEIGHT 32

typedef struct
{

	ALLEGRO_BITMAP * bitmap[QUIXEL_CANVAS_MAX_HEIGHT][QUIXEL_CANVAS_MAX_WIDTH];

} QUIXEL_CANVAS_LAYER;

typedef struct
{

	int bitmap_size;

	QUIXEL_CANVAS_LAYER ** layer;
	int layer_max;

} QUIXEL_CANVAS;

QUIXEL_CANVAS * quixel_create_canvas(int bitmap_max);
void quixel_destroy_canvas(QUIXEL_CANVAS * cp);

bool quixel_add_canvas_layer(QUIXEL_CANVAS * cp);
bool quixel_remove_canvas_layer(QUIXEL_CANVAS * cp, int layer);
bool quixel_expand_canvas(QUIXEL_CANVAS * cp, int layer, int x, int y);
void quixel_render_canvas(QUIXEL_CANVAS * cp, int x, int y, int width, int height, int scale);

#endif
