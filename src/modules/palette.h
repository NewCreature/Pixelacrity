#ifndef PA_PALETTE_H
#define PA_PALETTE_H

#include "t3f/t3f.h"

#define PA_DEFAULT_COLOR_MAX 81

typedef struct
{

	ALLEGRO_COLOR * color;
	int color_max;

} PA_PALETTE;

PA_PALETTE * pa_create_palette(void);
PA_PALETTE * pa_load_palette(const char * fn);
bool pa_save_palette(PA_PALETTE * pp, const char * fn);
void pa_destroy_palette(PA_PALETTE * pp);

bool pa_read_palette(PA_PALETTE * pp, const ALLEGRO_CONFIG * config);
bool pa_write_palette(PA_PALETTE * pp, ALLEGRO_CONFIG * config);

#endif
