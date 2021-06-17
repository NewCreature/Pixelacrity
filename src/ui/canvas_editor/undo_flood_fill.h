#ifndef QUIXEL_UNDO_FLOOD_FILL_H
#define QUIXEL_UNDO_FLOOD_FILL_H

#include "modules/queue.h"

bool quixel_make_flood_fill_undo(QUIXEL_CANVAS_EDITOR * cep, int layer, ALLEGRO_COLOR color, QUIXEL_QUEUE * qp, const char * fn);
bool quixel_make_flood_fill_redo(QUIXEL_CANVAS_EDITOR * cep, int layer, ALLEGRO_COLOR color, QUIXEL_QUEUE * qp, const char * fn);
bool quixel_apply_flood_fill_undo(QUIXEL_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp);
bool quixel_apply_flood_fill_redo(QUIXEL_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp);

#endif
