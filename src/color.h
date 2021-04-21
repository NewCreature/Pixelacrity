#ifndef QUIXEL_COLOR_H
#define QUIXEL_COLOR_H

bool quixel_color_equal(ALLEGRO_COLOR c1, ALLEGRO_COLOR c2);
ALLEGRO_COLOR quixel_shade_color(ALLEGRO_COLOR color, float l);
ALLEGRO_COLOR quixel_alpha_color(ALLEGRO_COLOR color, float alpha);
float quixel_get_color_shade(ALLEGRO_COLOR color);
float quixel_get_color_alpha(ALLEGRO_COLOR color);

#endif
