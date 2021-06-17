#ifndef QUIXEL_FLOOD_FILL_H
#define QUIXEL_FLOOD_FILL_H

#include "modules/queue.h"

bool quixel_flood_fill_canvas(QUIXEL_CANVAS * cp, int layer, int start_x, int start_y, ALLEGRO_COLOR color, QUIXEL_QUEUE * out_qp);
void quixel_flood_fill_canvas_from_queue(QUIXEL_CANVAS * cp, int layer, ALLEGRO_COLOR color, QUIXEL_QUEUE * qp);

#endif
