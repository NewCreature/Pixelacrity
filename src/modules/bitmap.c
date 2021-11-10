#include "t3f/t3f.h"

void pa_flip_bitmap(ALLEGRO_BITMAP * bp, bool h, bool v)
{
	ALLEGRO_BITMAP * temp_bp;
	ALLEGRO_STATE old_state;
	ALLEGRO_TRANSFORM identity;
	int flags = 0;

	al_store_state(&old_state, ALLEGRO_STATE_NEW_BITMAP_PARAMETERS | ALLEGRO_STATE_TARGET_BITMAP | ALLEGRO_STATE_TRANSFORM | ALLEGRO_STATE_BLENDER);
	temp_bp = al_clone_bitmap(bp);
	if(temp_bp)
	{
		al_set_target_bitmap(bp);
		al_identity_transform(&identity);
		al_use_transform(&identity);
		al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);
		if(h)
		{
			flags |= ALLEGRO_FLIP_HORIZONTAL;
		}
		if(v)
		{
			flags |= ALLEGRO_FLIP_VERTICAL;
		}
		al_draw_bitmap(temp_bp, 0, 0, flags);
		al_destroy_bitmap(temp_bp);
	}
	al_restore_state(&old_state);
}

void pa_turn_bitmap(ALLEGRO_BITMAP ** bp, int amount)
{
	ALLEGRO_BITMAP * temp_bp;
	ALLEGRO_STATE old_state;
	ALLEGRO_TRANSFORM identity;
	float angle;
	float cx, cy, dx, dy;

	al_store_state(&old_state, ALLEGRO_STATE_NEW_BITMAP_PARAMETERS | ALLEGRO_STATE_TARGET_BITMAP | ALLEGRO_STATE_TRANSFORM | ALLEGRO_STATE_BLENDER);
	if(abs(amount) % 2 == 0)
	{
		temp_bp = al_create_bitmap(al_get_bitmap_width(*bp), al_get_bitmap_height(*bp));
	}
	else
	{
		temp_bp = al_create_bitmap(al_get_bitmap_height(*bp), al_get_bitmap_width(*bp));
	}
	if(temp_bp)
	{
		al_set_target_bitmap(temp_bp);
		al_identity_transform(&identity);
		al_use_transform(&identity);
		al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);
		cx = (float)al_get_bitmap_width(*bp) / 2.0;
		cy = (float)al_get_bitmap_height(*bp) / 2.0;
		dx = (float)al_get_bitmap_width(temp_bp) / 2.0;
		dy = (float)al_get_bitmap_height(temp_bp) / 2.0;
		angle = (float)amount * (ALLEGRO_PI / 2.0);
		al_draw_rotated_bitmap(*bp, cx, cy, dx, dy, angle, 0);
		al_destroy_bitmap(*bp);
		*bp = temp_bp;
	}
	al_restore_state(&old_state);
}

ALLEGRO_BITMAP * pa_make_checkerboard_bitmap(ALLEGRO_COLOR c1, ALLEGRO_COLOR c2)
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
