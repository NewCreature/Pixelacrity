#ifndef PA_FLOOD_FILL_H
#define PA_FLOOD_FILL_H

#include "modules/queue.h"

bool pa_flood_fill_canvas(PA_CANVAS * cp, int layer, int start_x, int start_y, ALLEGRO_COLOR color, PA_QUEUE * out_qp);
void pa_flood_fill_canvas_from_queue(PA_CANVAS * cp, int layer, ALLEGRO_COLOR color, PA_QUEUE * qp);

#endif
