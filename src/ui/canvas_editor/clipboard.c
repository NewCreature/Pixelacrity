#include "canvas_editor.h"
#include "modules/canvas/canvas_helpers.h"
#include "modules/dynamic_array.h"

void pa_free_clipboard(PA_CANVAS_EDITOR * cep)
{
	t3f_debug_message("Enter pa_free_clipboard()\n");
	if(cep->clipboard.bitmap_stack)
	{
		pa_destroy_bitmap_stack(cep->clipboard.bitmap_stack);
		cep->clipboard.bitmap_stack = NULL;
	}
	t3f_debug_message("Exit pa_free_clipboard()\n");
}

bool pa_copy_bitmap_to_clipboard(PA_CANVAS_EDITOR * cep, PA_BITMAP_STACK * bp)
{
	ALLEGRO_STATE old_state;
	int i;

	t3f_debug_message("Enter pa_copy_bitmap_to_clipboard()\n");
	al_store_state(&old_state, ALLEGRO_STATE_NEW_BITMAP_PARAMETERS);
	pa_free_clipboard(cep);
	al_set_new_bitmap_flags(ALLEGRO_NO_PREMULTIPLIED_ALPHA);
	cep->clipboard.bitmap_stack = pa_create_bitmap_stack(bp->layers, bp->width, bp->height);
	if(!cep->clipboard.bitmap_stack)
	{
		goto fail;
	}
	for(i = 0; i < bp->layers; i++)
	{
		if(bp->bitmap[i])
		{
			cep->clipboard.bitmap_stack->bitmap[i] = al_clone_bitmap(bp->bitmap[i]);
		}
	}
	cep->clipboard.layer = cep->selection.layer;
	cep->clipboard.layer_max = bp->layers;
	al_restore_state(&old_state);
	t3f_debug_message("Exit pa_copy_bitmap_to_clipboard()\n");
	return true;

	fail:
	{
		al_restore_state(&old_state);
		return false;
	}
}

bool pa_copy_canvas_to_clipboard(PA_CANVAS_EDITOR * cep, int layer, int x, int y, int width, int height)
{
	ALLEGRO_STATE old_state;
	int i;
	bool ret = false;

	t3f_debug_message("Enter pa_copy_canvas_to_clipboard()\n");
	al_store_state(&old_state, ALLEGRO_STATE_NEW_BITMAP_PARAMETERS);
	pa_free_clipboard(cep);
	al_set_new_bitmap_flags(ALLEGRO_NO_PREMULTIPLIED_ALPHA);
	cep->clipboard.bitmap_stack = pa_create_bitmap_stack(cep->canvas->layer_max, width, height);
	if(cep->clipboard.bitmap_stack)
	{
		if(layer < 0)
		{
			for(i = 0; i < cep->canvas->layer_max; i++)
			{
				cep->clipboard.bitmap_stack->bitmap[i] = al_create_bitmap(width, height);
				pa_render_canvas_to_bitmap(cep->canvas, i, i + 1, x, y, width, height, 0, cep->clipboard.bitmap_stack->bitmap[i], NULL);
			}
		}
		else
		{
			cep->clipboard.bitmap_stack->bitmap[layer] = al_create_bitmap(width, height);
			pa_render_canvas_to_bitmap(cep->canvas, layer, layer + 1, x, y, width, height, 0, cep->clipboard.bitmap_stack->bitmap[layer], NULL);
		}
		cep->clipboard.x = x, cep->clipboard.y = y;
		ret = true;
		cep->clipboard.layer = layer;
		cep->clipboard.layer_max = cep->canvas->layer_max;
	}
	al_restore_state(&old_state);
	t3f_debug_message("Exit pa_copy_canvas_to_clipboard()\n");
	return ret;
}

bool pa_add_layer_to_clipboard(PA_CANVAS_EDITOR * cep, int layer)
{
	PA_BITMAP_STACK * old_bitmap;
	int i;

	t3f_debug_message("Enter pa_add_layer_to_clipboard()\n");
	if(layer < 0)
	{
		layer = cep->clipboard.layer_max;
	}
	old_bitmap = cep->clipboard.bitmap_stack;
	cep->clipboard.bitmap_stack = pa_create_bitmap_stack(old_bitmap->layers + 1, old_bitmap->width, old_bitmap->height);
	if(cep->clipboard.bitmap_stack)
	{
		for(i = 0; i < layer; i++)
		{
			cep->clipboard.bitmap_stack->bitmap[i] = old_bitmap->bitmap[i];
		}
		for(i = layer + 1; i < cep->clipboard.layer_max + 1; i++)
		{
			cep->clipboard.bitmap_stack->bitmap[i] = old_bitmap->bitmap[i - 1];
		}
		cep->clipboard.bitmap_stack->bitmap[layer] = al_create_bitmap(old_bitmap->width, old_bitmap->height);
		if(cep->clipboard.bitmap_stack->bitmap[layer])
		{
			cep->clipboard.layer_max++;
			pa_destroy_bitmap_stack(old_bitmap);
			t3f_debug_message("pa_add_layer_to_clipboard() success\n");
			return true;
		}
	}
	cep->clipboard.bitmap_stack = old_bitmap;
	t3f_debug_message("pa_add_layer_to_clipboard() fail\n");
	return false;
}

bool pa_remove_layer_from_clipboard(PA_CANVAS_EDITOR * cep, int layer)
{
	PA_BITMAP_STACK * old_bitmap;
	int i;

	t3f_debug_message("Enter pa_remove_layer_from_clipboard()\n");
	old_bitmap = cep->clipboard.bitmap_stack;
	cep->clipboard.bitmap_stack = pa_create_bitmap_stack(old_bitmap->layers - 1, old_bitmap->width, old_bitmap->height);
	if(cep->clipboard.bitmap_stack)
	{
		for(i = 0; i < layer; i++)
		{
			cep->clipboard.bitmap_stack->bitmap[i] = old_bitmap->bitmap[i];
		}
		al_destroy_bitmap(cep->clipboard.bitmap_stack->bitmap[layer]);
		for(i = layer; i < cep->clipboard.layer_max - 1; i++)
		{
			cep->clipboard.bitmap_stack->bitmap[i] = old_bitmap->bitmap[i + 1];
		}
		cep->clipboard.layer_max--;
		pa_destroy_bitmap_stack(old_bitmap);
		t3f_debug_message("pa_remove_layer_from_clipboard() success\n");
		return true;
	}
	cep->clipboard.bitmap_stack = old_bitmap;
	t3f_debug_message("pa_remove_layer_from_clipboard() fail\n");
	return false;
}
