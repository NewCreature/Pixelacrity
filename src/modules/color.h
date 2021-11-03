#ifndef PA_COLOR_H
#define PA_COLOR_H

bool pa_color_equal(ALLEGRO_COLOR c1, ALLEGRO_COLOR c2);
ALLEGRO_COLOR pa_shade_color(ALLEGRO_COLOR color, float l);
ALLEGRO_COLOR pa_alpha_color(ALLEGRO_COLOR color, float alpha);
float pa_get_color_shade(ALLEGRO_COLOR color);
float pa_get_color_alpha(ALLEGRO_COLOR color);
void pa_print_color(ALLEGRO_COLOR color);
ALLEGRO_COLOR pa_get_real_color(ALLEGRO_COLOR color, ALLEGRO_BITMAP * scratch);
ALLEGRO_COLOR pa_get_color_from_html(const char * html);

#endif
