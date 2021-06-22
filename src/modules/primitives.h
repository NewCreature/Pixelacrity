#ifndef PA_PRIMITIVES_H
#define PA_PRIMITIVES_H

#include "t3f/t3f.h"

void pa_sort_coordinates(int * x1, int * x2);

void pa_draw_line(int x1, int y1, int x2, int y2, ALLEGRO_BITMAP * bp, ALLEGRO_COLOR color);
void pa_draw_rectangle(int x1, int y1, int x2, int y2, ALLEGRO_BITMAP * bp, ALLEGRO_COLOR color);
void pa_draw_filled_rectangle(int x1, int y1, int x2, int y2, ALLEGRO_BITMAP * bp, ALLEGRO_COLOR color);
void pa_draw_oval(int x1, int y1, int x2, int y2, ALLEGRO_BITMAP * bp, ALLEGRO_COLOR color);
void pa_draw_filled_oval(int x1, int y1, int x2, int y2, ALLEGRO_BITMAP * bp, ALLEGRO_COLOR color);
void pa_draw_quad(int x1, int y1, int x2, int y2, ALLEGRO_BITMAP * bp, ALLEGRO_COLOR color);

#endif
