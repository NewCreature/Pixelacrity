#ifndef QUIXEL_CANVAS_H
#define QUIXEL_CANVAS_H

#include "t3f/t3f.h"
#include "modules/box.h"

#define QUIXEL_CANVAS_FLAG_HIDDEN 1

typedef struct
{

	ALLEGRO_BITMAP *** bitmap;
	int flags;

} QUIXEL_CANVAS_LAYER;

typedef struct
{

	char * name;
	QUIXEL_BOX box;

} QUIXEL_CANVAS_FRAME;

typedef struct
{

	int bitmap_size;
	int layer_width, layer_height;

	QUIXEL_CANVAS_LAYER ** layer;
	int layer_max;

	QUIXEL_CANVAS_FRAME ** frame;
	int frame_max;

	ALLEGRO_CONFIG * config;

	int export_offset_x;
	int export_offset_y;

} QUIXEL_CANVAS;

QUIXEL_CANVAS * quixel_create_canvas(int bitmap_max);
void quixel_destroy_canvas(QUIXEL_CANVAS * cp);

bool quixel_add_canvas_layer(QUIXEL_CANVAS * cp);
bool quixel_remove_canvas_layer(QUIXEL_CANVAS * cp, int layer);

bool quixel_add_canvas_frame(QUIXEL_CANVAS * cp, const char * name, int x, int y, int width, int height);
bool quixel_remove_canvas_frame(QUIXEL_CANVAS * cp, int frame);

void quixel_shift_canvas_bitmap_array(QUIXEL_CANVAS * cp, int shift_x, int shift_y);
bool quixel_resize_canvas_bitmap_array(QUIXEL_CANVAS * cp, int width, int height);
bool quixel_expand_canvas(QUIXEL_CANVAS * cp, int layer, int x, int y);

void quixel_render_canvas_layer(QUIXEL_CANVAS * cp, int i, int x, int y, int scale, float ox, float oy, int width, int height);
void quixel_render_canvas(QUIXEL_CANVAS * cp, int x, int y, int scale, float ox, float oy, int width, int height);

#endif
