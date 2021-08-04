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
		pa_free((void **)cep->clipboard.bitmap, cep->canvas->layer_max);
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
	}
	al_restore_state(&old_state);
	return ret;
}
