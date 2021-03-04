#include "t3f/t3f.h"
#include "canvas_editor.h"
#include "tool_pixel.h"

static ALLEGRO_BITMAP * make_checkerboard_bitmap(ALLEGRO_COLOR c1, ALLEGRO_COLOR c2)
{
	ALLEGRO_STATE old_state;
	ALLEGRO_TRANSFORM identity;
	ALLEGRO_BITMAP * bp;

	al_store_state(&old_state, ALLEGRO_STATE_NEW_BITMAP_PARAMETERS | ALLEGRO_STATE_TARGET_BITMAP | ALLEGRO_STATE_TRANSFORM);
	al_set_new_bitmap_flags(0);
	bp = al_create_bitmap(2, 2);
	if(bp)
	{
		al_set_target_bitmap(bp);
		al_identity_transform(&identity);
		al_use_transform(&identity);
		al_lock_bitmap(bp, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_WRITEONLY);
		al_put_pixel(0, 0, c1);
		al_put_pixel(0, 1, c2);
		al_put_pixel(1, 0, c2);
		al_put_pixel(1, 1, c1);
		al_unlock_bitmap(bp);
	}
	al_restore_state(&old_state);
	return bp;
}

QUIXEL_CANVAS_EDITOR * quixel_create_canvas_editor(QUIXEL_CANVAS * cp)
{
	QUIXEL_CANVAS_EDITOR * cep;

	cep = malloc(sizeof(QUIXEL_CANVAS_EDITOR));
	if(!cep)
	{
		goto fail;
	}
	memset(cep, 0, sizeof(QUIXEL_CANVAS_EDITOR));
	cep->bg_tile = make_checkerboard_bitmap(t3f_color_white, al_map_rgba_f(0.9, 0.9, 0.9, 1.0));
	if(!cep->bg_tile)
	{
		goto fail;
	}
	cep->scratch_bitmap = al_create_bitmap(al_get_display_width(t3f_display), al_get_display_height(t3f_display));
	if(!cep->scratch_bitmap)
	{
		goto fail;
	}
	cep->canvas = cp;
	cep->base_color = al_map_rgba_f(1.0, 0.0, 0.0, 1.0);
	return cep;

	fail:
	{
		if(cep)
		{
			quixel_destroy_canvas_editor(cep);
		}
		return NULL;
	}
}

void quixel_destroy_canvas_editor(QUIXEL_CANVAS_EDITOR * cep)
{
	if(cep->scratch_bitmap)
	{
		al_destroy_bitmap(cep->scratch_bitmap);
	}
	if(cep->bg_tile)
	{
		al_destroy_bitmap(cep->bg_tile);
	}
	free(cep);
}
