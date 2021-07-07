#ifndef PA_PIXEL_SHADER_H
#define PA_PIXEL_SHADER_H

#include "t3f/t3f.h"

ALLEGRO_SHADER * pa_create_pixel_shader(const char * fn);
bool pa_set_target_pixel_shader(ALLEGRO_SHADER * sp);

#endif
