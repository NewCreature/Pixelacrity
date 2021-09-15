#ifndef PA_MULTI_LAYER_BITMAP_H
#define PA_MULTI_LAYER_BITMAP_H

typedef struct
{

	ALLEGRO_BITMAP ** bitmap;
	int layers;
	int width, height;

} PA_BITMAP_STACK;

PA_BITMAP_STACK * pa_create_bitmap_stack(int layers, int width, int height);
void pa_destroy_bitmap_stack(PA_BITMAP_STACK * bp);
bool pa_add_layer_to_bitmap_stack(PA_BITMAP_STACK * bp, ALLEGRO_BITMAP * bitmap, int layer);
bool pa_remove_layer_from_bitmap_stack(PA_BITMAP_STACK * bp, int layer);
int pa_get_bitmap_stack_width(PA_BITMAP_STACK * bp);
int pa_get_bitmap_stack_height(PA_BITMAP_STACK * bp);

#endif
