#ifndef PA_CANVAS_H
#define PA_CANVAS_H

#include "t3f/t3f.h"
#include "modules/box.h"

#define PA_CANVAS_FLAG_HIDDEN 1

typedef struct
{

	ALLEGRO_BITMAP *** bitmap;
	int flags;

} PA_CANVAS_LAYER;

typedef struct
{

	char * name;
	PA_BOX box;
	char * export_path;

} PA_CANVAS_FRAME;

typedef struct
{

	int bitmap_size;
	int layer_width, layer_height;

	PA_CANVAS_LAYER ** layer;
	int layer_max;

	PA_CANVAS_FRAME ** frame;
	int frame_max;

	int export_offset_x;
	int export_offset_y;

} PA_CANVAS;

PA_CANVAS * pa_create_canvas(int bitmap_max);
void pa_destroy_canvas(PA_CANVAS * cp);

bool pa_add_canvas_layer(PA_CANVAS * cp, int layer);
bool pa_remove_canvas_layer(PA_CANVAS * cp, int layer);

bool pa_add_canvas_frame(PA_CANVAS * cp, const char * name, int x, int y, int width, int height);
bool pa_remove_canvas_frame(PA_CANVAS * cp, int frame);

void pa_shift_canvas_bitmap_array(PA_CANVAS * cp, int shift_x, int shift_y);
bool pa_resize_canvas_bitmap_array(PA_CANVAS * cp, int width, int height);
bool pa_expand_canvas(PA_CANVAS * cp, int layer, int x, int y);

void pa_render_canvas_layer(PA_CANVAS * cp, int i, int x, int y, int scale, float ox, float oy, int width, int height);
void pa_render_canvas(PA_CANVAS * cp, int x, int y, int scale, float ox, float oy, int width, int height);

#endif
