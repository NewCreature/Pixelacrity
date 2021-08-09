#include "canvas_editor.h"
#include "modules/canvas/canvas_helpers.h"
#include "modules/dynamic_array.h"

void pa_free_clipboard(PA_CANVAS_EDITOR * cep)
{
	int i;

	if(cep->clipboard.bitmap)
	{
		for(i = 0; i < cep->clipboard.layer_max; i++)
		{
			if(cep->clipboard.bitmap[i])
			{
				al_destroy_bitmap(cep->clipboard.bitmap[i]);
			}
		}
		pa_free((void **)cep->clipboard.bitmap);
		cep->clipboard.bitmap = NULL;
	}
}

bool pa_copy_bitmap_to_clipboard(PA_CANVAS_EDITOR * cep, ALLEGRO_BITMAP ** bp, int max)
{
	ALLEGRO_STATE old_state;
	int i;

	al_store_state(&old_state, ALLEGRO_STATE_NEW_BITMAP_PARAMETERS);
	pa_free_clipboard(cep);
	al_set_new_bitmap_flags(ALLEGRO_NO_PREMULTIPLIED_ALPHA);
	cep->clipboard.bitmap = (ALLEGRO_BITMAP **)pa_malloc(sizeof(ALLEGRO_BITMAP *), max);
	if(cep->clipboard.bitmap)
	{
		for(i = 0; i < max; i++)
		{
			cep->clipboard.bitmap[i] = al_clone_bitmap(bp[i]);
		}
	}
	cep->clipboard.layer_max = max;
	al_restore_state(&old_state);
	if(cep->clipboard.bitmap)
	{
		return true;
	}
	return false;
}

bool pa_copy_canvas_to_clipboard(PA_CANVAS_EDITOR * cep, int layer, int x, int y, int width, int height)
{
	ALLEGRO_STATE old_state;
	int i;
	bool ret = false;

	al_store_state(&old_state, ALLEGRO_STATE_NEW_BITMAP_PARAMETERS);
	pa_free_clipboard(cep);
	al_set_new_bitmap_flags(ALLEGRO_NO_PREMULTIPLIED_ALPHA);
	cep->clipboard.bitmap = (ALLEGRO_BITMAP **)pa_malloc(sizeof(ALLEGRO_BITMAP *), cep->canvas->layer_max);
	if(cep->clipboard.bitmap)
	{
		if(cep->clipboard.bitmap)
		{
			if(layer < 0)
			{
				for(i = 0; i < cep->canvas->layer_max; i++)
				{
					cep->clipboard.bitmap[i] = al_create_bitmap(width, height);
					pa_render_canvas_to_bitmap(cep->canvas, i, i + 1, x, y, width, height, 0, cep->clipboard.bitmap[i]);
				}
			}
			else
			{
				cep->clipboard.bitmap[layer] = al_create_bitmap(width, height);
				pa_render_canvas_to_bitmap(cep->canvas, layer, layer + 1, x, y, width, height, 0, cep->clipboard.bitmap[layer]);
			}
			cep->clipboard.x = x, cep->clipboard.y = y;
			ret = true;
		}
		cep->clipboard.layer = layer;
	}
	al_restore_state(&old_state);
	return ret;
}

bool pa_add_layer_to_clipboard(PA_CANVAS_EDITOR * cep, int layer)
{
	ALLEGRO_BITMAP ** old_bitmap;
	int i;

	if(layer < 0)
	{
		layer = cep->clipboard.layer_max;
	}
	old_bitmap = cep->clipboard.bitmap;
	cep->clipboard.bitmap = (ALLEGRO_BITMAP **)pa_malloc(sizeof(ALLEGRO_BITMAP *), cep->clipboard.layer_max + 1);
	if(cep->clipboard.bitmap)
	{
		for(i = 0; i < layer; i++)
		{
			cep->clipboard.bitmap[i] = old_bitmap[i];
		}
		for(i = layer + 1; i < cep->clipboard.layer_max + 1; i++)
		{
			cep->clipboard.bitmap[i] = old_bitmap[i - 1];
		}
		cep->clipboard.bitmap[layer] = al_create_bitmap(al_get_bitmap_width(cep->clipboard.bitmap[0]), al_get_bitmap_height(cep->clipboard.bitmap[0]));
		if(cep->clipboard.bitmap[layer])
		{
			cep->clipboard.layer_max++;
			pa_free((void **)old_bitmap);
			return true;
		}
	}
	return false;
}

bool pa_remove_layer_from_clipboard(PA_CANVAS_EDITOR * cep, int layer)
{
	ALLEGRO_BITMAP ** old_bitmap;
	int i;

	old_bitmap = cep->clipboard.bitmap;
	cep->clipboard.bitmap = (ALLEGRO_BITMAP **)pa_malloc(sizeof(ALLEGRO_BITMAP *), cep->clipboard.layer_max - 1);
	if(cep->clipboard.bitmap)
	{
		for(i = 0; i < layer; i++)
		{
			cep->clipboard.bitmap[i] = old_bitmap[i];
		}
		al_destroy_bitmap(cep->clipboard.bitmap[layer]);
		for(i = layer; i < cep->clipboard.layer_max - 1; i++)
		{
			cep->clipboard.bitmap[i] = old_bitmap[i + 1];
		}
		cep->clipboard.layer_max--;
		pa_free((void **)old_bitmap);
		return true;
	}
	return false;
}
