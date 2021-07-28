#include "t3f/t3f.h"
#include "multi_layer_bitmap.h"

PA_BITMAP_STACK * pa_create_bitmap_stack(int width, int height)
{
	PA_BITMAP_STACK * bp;

	bp = malloc(sizeof(PA_BITMAP_STACK));
	if(bp)
	{
		memset(bp, 0, sizeof(PA_BITMAP_STACK));
		bp->width = width;
		bp->height = height;
	}

	return bp;
}

void pa_destroy_bitmap_stack(PA_BITMAP_STACK * bp)
{
	int i;

	if(bp)
	{
		for(i = 0; i < bp->layers; i++)
		{
			if(bp->bitmap[i])
			{
				al_destroy_bitmap(bp->bitmap[i]);
			}
		}
		free(bp);
	}
}

bool pa_add_layer_to_bitmap_stack(PA_BITMAP_STACK * bp, int layer)
{
	ALLEGRO_BITMAP ** old_bitmap;
	int bitmap_size;
	int i;

	if(layer < 0)
	{
		layer = bp->layers;
	}
	old_bitmap = bp->bitmap;
	bitmap_size = sizeof(ALLEGRO_BITMAP *) * (bp->layers + 1);
	bp->bitmap = malloc(bitmap_size);
	if(bp->bitmap)
	{
		memset(bp->bitmap, 0, bitmap_size);
		for(i = 0; i < layer; i++)
		{
			bp->bitmap[i] = old_bitmap[i];
		}
		for(i = layer + 1; i < bp->layers + 1; i++)
		{
			bp->bitmap[i] = old_bitmap[i - 1];
		}
		bp->bitmap[layer] = al_create_bitmap(bp->width, bp->height);
		if(bp->bitmap[layer])
		{
			bp->layers++;
			free(old_bitmap);
			return true;
		}
	}
	bp->bitmap = old_bitmap;
	return false;
}

bool pa_remove_layer_from_bitmap_stack(PA_BITMAP_STACK * bp, int layer)
{
	ALLEGRO_BITMAP ** old_bitmap;
	int bitmap_size;
	int i;

	old_bitmap = bp->bitmap;
	bitmap_size = sizeof(ALLEGRO_BITMAP *) * (bp->layers - 1);
	bp->bitmap = malloc(bitmap_size);
	if(bp->bitmap)
	{
		memset(bp->bitmap, 0, bitmap_size);
		for(i = 0; i < layer; i++)
		{
			bp->bitmap[i] = old_bitmap[i];
		}
		free(old_bitmap[layer]);
		for(i = layer; i < bp->layers - 1; i++)
		{
			bp->bitmap[i] = old_bitmap[i + 1];
		}
		bp->layers--;
		free(old_bitmap);
		return true;
	}
	bp->bitmap = old_bitmap;
	return false;
}
