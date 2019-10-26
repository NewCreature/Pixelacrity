#ifndef QUIXEL_CANVAS_H
#define QUIXEL_CANVAS_H

#define QUIXEL_CANVAS_MAX_WIDTH  32
#define QUIXEL_CANVAS_MAX_HEIGHT 32

typedef struct
{

	ALLEGRO_BITMAP * bitmap[QUIXEL_CANVAS_MAX_HEIGHT][QUIXEL_CANVAS_MAX_WIDTH];
	int bitmap_size;

} QUIXEL_CANVAS;

QUIXEL_CANVAS * quixel_create_canvas(void);
void quixel_destroy_canvas(QUIXEL_CANVAS * cp);

void quixel_expand_canvas(QUIXEL_CANVAS * cp, int x, int y);
void quixel_render_canvas(QUIXEL_CANVAS * cp, int x, int y, int width, int height, int scale);

#endif
