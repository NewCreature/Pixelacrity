#ifndef PA_FLOOD_FILL_H
#define PA_FLOOD_FILL_H

#include "modules/queue.h"

bool pa_flood_fill_canvas(PA_CANVAS * cp, int layer, int start_x, int start_y, ALLEGRO_COLOR color, PA_QUEUE * out_qp);
bool pa_flood_fill_canvas_area(PA_CANVAS * cp, int layer, int left, int top, int right, int bottom, int start_x, int start_y, ALLEGRO_COLOR color, PA_QUEUE * out_qp);
bool pa_replace_canvas_color(PA_CANVAS * cp, int layer, ALLEGRO_COLOR old_color, ALLEGRO_COLOR new_color, PA_QUEUE * out_qp);
bool pa_replace_canvas_color_area(PA_CANVAS * cp, int layer, ALLEGRO_COLOR old_color, ALLEGRO_COLOR new_color, int left, int top, int right, int bottom, PA_QUEUE * out_qp);
void pa_flood_fill_canvas_from_queue(PA_CANVAS * cp, int layer, ALLEGRO_COLOR color, PA_QUEUE * qp);

#endif
