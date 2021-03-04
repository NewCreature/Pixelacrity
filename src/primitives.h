#ifndef QUIXEL_PRIMITIVES_H
#define QUIXEL_PRIMITIVES_H

#include "t3f/t3f.h"

void quixel_draw_line(int x1, int y1, int x2, int y2, ALLEGRO_COLOR color);
void quixel_draw_rectangle(int x1, int y1, int x2, int y2, ALLEGRO_COLOR color);
void quixel_draw_filled_rectangle(int x1, int y1, int x2, int y2, ALLEGRO_COLOR color);
void quixel_draw_oval(int x1, int y1, int x2, int y2, ALLEGRO_COLOR color);
void quixel_draw_filled_oval(int x1, int y1, int x2, int y2, ALLEGRO_COLOR color);

#endif
