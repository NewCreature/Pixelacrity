#ifndef PA_BITMAP_H
#define PA_BITMAP_H

#include "t3f/t3f.h"

void pa_flip_bitmap(ALLEGRO_BITMAP * bp, bool h, bool v);
void pa_turn_bitmap(ALLEGRO_BITMAP ** bp, int amount);
ALLEGRO_BITMAP * pa_make_checkerboard_bitmap(ALLEGRO_COLOR c1, ALLEGRO_COLOR c2);

#endif
