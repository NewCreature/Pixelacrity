#ifndef PA_CANVAS_HELPERS_H
#define PA_CANVAS_HELPERS_H

#define PA_RENDER_COPY      0
#define PA_RENDER_COMPOSITE 1

#include "t3f/t3f.h"
#include "canvas.h"

void pa_get_canvas_dimensions(PA_CANVAS * cp, int * offset_x, int * offset_y, int * width, int * height, int flags_filter);
void pa_render_canvas_to_bitmap(PA_CANVAS * cp, int start_layer, int end_layer, int x, int y, int w, int h, int flags_filter, ALLEGRO_BITMAP * bp);
ALLEGRO_BITMAP * pa_get_bitmap_from_canvas(PA_CANVAS * cp, int start_layer, int end_layer, int flags_filter);
void pa_import_bitmap_to_canvas(PA_CANVAS * cp, ALLEGRO_BITMAP * bp, int layer, int x, int y);
void pa_draw_primitive_to_canvas(PA_CANVAS * cp, int layer, int x1, int y1, int x2, int y2, ALLEGRO_BITMAP * bp, ALLEGRO_COLOR color, int mode, void (*primitive_proc)(int x1, int y1, int x2, int y2, ALLEGRO_BITMAP * bp, ALLEGRO_COLOR color));
ALLEGRO_COLOR pa_get_canvas_pixel(PA_CANVAS * cp, int layer, int x, int y);
void pa_get_canvas_shift(PA_CANVAS * cp, int x, int y, int * shift_x, int * shift_y);
bool pa_handle_canvas_expansion(PA_CANVAS * cp, int left, int top, int right, int bottom, int * shift_x, int * shift_y);

#endif
