#ifndef PA_UNDO_FLOOD_FILL_H
#define PA_UNDO_FLOOD_FILL_H

#include "modules/queue.h"

bool pa_make_flood_fill_undo(PA_CANVAS_EDITOR * cep, int layer, ALLEGRO_COLOR color, PA_QUEUE * qp, int shift_x, int shift_y, const char * fn);
bool pa_make_flood_fill_redo(PA_CANVAS_EDITOR * cep, int layer, ALLEGRO_COLOR color, PA_QUEUE * qp, int shift_x, int shift_y, const char * fn);
bool pa_apply_flood_fill_undo(PA_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp);
bool pa_apply_flood_fill_redo(PA_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp);

#endif
