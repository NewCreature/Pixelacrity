#include "t3f/t3f.h"
#include "multi_layer_bitmap.h"

static ALLEGRO_BITMAP ** allocate_bitmap_array(int count)
{
	ALLEGRO_BITMAP ** bp;
	int bitmap_size;

	bitmap_size = sizeof(ALLEGRO_BITMAP *) * (count);
	bp= malloc(bitmap_size);
	if(bp)
	{
		memset(bp, 0, bitmap_size);
	}
	return bp;
}

PA_BITMAP_STACK * pa_create_bitmap_stack(int layers, int width, int height)
{
	PA_BITMAP_STACK * bp;

	bp = malloc(sizeof(PA_BITMAP_STACK));
	if(!bp)
	{
		goto fail;
	}
	memset(bp, 0, sizeof(PA_BITMAP_STACK));
	bp->bitmap = allocate_bitmap_array(layers);
	if(!bp->bitmap)
	{
		goto fail;
	}
	bp->layers = layers;
	bp->width = width;
	bp->height = height;

	return bp;

	fail:
	{
		pa_destroy_bitmap_stack(bp);
		return NULL;
	}
}

void pa_destroy_bitmap_stack(PA_BITMAP_STACK * bp)
{
	int i;

	if(bp)
	{
		if(bp->bitmap)
		{
			for(i = 0; i < bp->layers; i++)
			{
				if(bp->bitmap[i])
				{
					al_destroy_bitmap(bp->bitmap[i]);
				}
			}
			free(bp->bitmap);
		}
		free(bp);
	}
}

bool pa_add_layer_to_bitmap_stack(PA_BITMAP_STACK * bp, ALLEGRO_BITMAP * bitmap, int layer)
{
	ALLEGRO_BITMAP ** old_bitmap;
	int i;

	if(layer < 0)
	{
		layer = bp->layers;
	}
	old_bitmap = bp->bitmap;
	bp->bitmap = allocate_bitmap_array(bp->layers + 1);
	if(bp->bitmap)
	{
		for(i = 0; i < layer; i++)
		{
			bp->bitmap[i] = old_bitmap[i];
		}
		for(i = layer + 1; i < bp->layers + 1; i++)
		{
			bp->bitmap[i] = old_bitmap[i - 1];
		}
		if(bitmap)
		{
			bp->bitmap[layer] = bitmap;
		}
		else
		{
			bp->bitmap[layer] = al_create_bitmap(bp->width, bp->height);
			if(!bp->bitmap[layer])
			{
				goto fail;
			}
		}
		bp->layers++;
		free(old_bitmap);
		return true;
	}
	fail:
	{
		if(bp->bitmap)
		{
			free(bp->bitmap);
		}
		bp->bitmap = old_bitmap;
		return false;
	}
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

int pa_get_bitmap_stack_width(PA_BITMAP_STACK * bp)
{
	int i;

	for(i = 0; i < bp->layers; i++)
	{
		if(bp->bitmap[i])
		{
			return al_get_bitmap_width(bp->bitmap[i]);
		}
	}
	return -1;
}

int pa_get_bitmap_stack_height(PA_BITMAP_STACK * bp)
{
	int i;

	for(i = 0; i < bp->layers; i++)
	{
		if(bp->bitmap[i])
		{
			return al_get_bitmap_height(bp->bitmap[i]);
		}
	}
	return -1;
}
