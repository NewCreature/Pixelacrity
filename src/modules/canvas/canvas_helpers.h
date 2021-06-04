#ifndef QUIXEL_CANVAS_HELPERS_H
#define QUIXEL_CANVAS_HELPERS_H

#define QUIXEL_RENDER_COPY      0
#define QUIXEL_RENDER_COMPOSITE 1

#include "t3f/t3f.h"
#include "canvas.h"

void quixel_get_canvas_dimensions(QUIXEL_CANVAS * cp, int * offset_x, int * offset_y, int * width, int * height, int flags_filter);
void quixel_render_canvas_to_bitmap(QUIXEL_CANVAS * cp, int start_layer, int end_layer, int x, int y, int w, int h, int flags_filter, ALLEGRO_BITMAP * bp);
ALLEGRO_BITMAP * quixel_get_bitmap_from_canvas(QUIXEL_CANVAS * cp, int start_layer, int end_layer, int flags_filter);
void quixel_import_bitmap_to_canvas(QUIXEL_CANVAS * cp, ALLEGRO_BITMAP * bp, int layer, int x, int y);
void quixel_draw_primitive_to_canvas(QUIXEL_CANVAS * cp, int layer, int x1, int y1, int x2, int y2, ALLEGRO_BITMAP * bp, ALLEGRO_COLOR color, int mode, void (*primitive_proc)(int x1, int y1, int x2, int y2, ALLEGRO_BITMAP * bp, ALLEGRO_COLOR color));
ALLEGRO_COLOR quixel_get_canvas_pixel(QUIXEL_CANVAS * cp, int layer, int x, int y);
bool quixel_handle_canvas_expansion(QUIXEL_CANVAS * cp, int left, int top, int right, int bottom, int * shift_x, int * shift_y);

#endif
