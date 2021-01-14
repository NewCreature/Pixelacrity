#ifndef QUIXEL_CANVAS_HELPERS_H
#define QUIXEL_CANVAS_HELPERS_H

#include "t3f/t3f.h"
#include "canvas.h"

ALLEGRO_BITMAP * quixel_render_canvas_to_bitmap(QUIXEL_CANVAS * cp, int start_layer, int end_layer);
void quixel_import_bitmap_to_canvas(QUIXEL_CANVAS * cp, ALLEGRO_BITMAP * bp, int layer, int x, int y);

#endif
