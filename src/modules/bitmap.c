#include "t3f/t3f.h"
#include <allegro5/allegro_opengl.h>

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

void pa_get_bitmap_dimensions(ALLEGRO_BITMAP * bp, int * x, int * y, int * width, int * height)
{
	ALLEGRO_COLOR c;
	unsigned char r, a;
	int left_x = al_get_bitmap_width(bp);
	int right_x = 0;
	int top_y = al_get_bitmap_height(bp);
	int bottom_y = 0;
	int l, m;

	al_lock_bitmap(bp, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READONLY);
	for(l = 0; l < al_get_bitmap_height(bp); l++)
	{
		for(m = 0; m < al_get_bitmap_width(bp); m++)
		{
			c = al_get_pixel(bp, m, l);
			al_unmap_rgba(c, &r, &r, &r, &a);
			if(a > 0)
			{
				if(m < left_x)
				{
					left_x = m;
				}
				if(m > right_x)
				{
					right_x = m;
				}
				if(l < top_y)
				{
					top_y = l;
				}
				if(l > bottom_y)
				{
					bottom_y = l;
				}
			}
		}
	}
	al_unlock_bitmap(bp);
	if(x)
	{
		*x = left_x;
	}
	if(y)
	{
		*y = top_y;
	}
	if(width)
	{
		*width = (right_x - left_x) + 1;
	}
	if(height)
	{
		*height = (bottom_y - top_y) + 1;
	}
}

void pa_set_bitmap_flags(ALLEGRO_BITMAP * bp, int flags)
{
	GLint old_texture;
	//GLfloat color[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

	glGetIntegerv(GL_TEXTURE_BINDING_2D, &old_texture);
	glBindTexture(GL_TEXTURE_2D, al_get_opengl_texture(bp));
	if(flags & ALLEGRO_MIN_LINEAR)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}
	if(flags & ALLEGRO_MAG_LINEAR)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
	//glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
	glBindTexture(GL_TEXTURE_2D, old_texture);
}

/* When compositing to a bitmap to save, we need to remove the pre-
   multiplication so the image file does not contain the pre-multiplied 
	 RGB values. */
void pa_unpremultiply_bitmap_alpha(ALLEGRO_BITMAP * bp)
{
	int i, j;
	ALLEGRO_COLOR c;
	float r, g, b, a;
	ALLEGRO_STATE old_state;

	al_store_state(&old_state, ALLEGRO_STATE_TRANSFORM | ALLEGRO_STATE_TARGET_BITMAP | ALLEGRO_STATE_BLENDER);
	al_lock_bitmap(bp, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READWRITE);
	al_set_target_bitmap(bp);
	for(i = 0; i < al_get_bitmap_height(bp); i++)
	{
		for(j = 0; j < al_get_bitmap_width(bp); j++)
		{
			c = al_get_pixel(bp, j, i);
			al_unmap_rgba_f(c, &r, &g, &b, &a);
			if(a > 0.0)
			{
				al_put_pixel(j, i, al_map_rgba_f(r / a, g / a, b / a, a));
			}
			else
			{
				al_put_pixel(j, i, al_map_rgba_f(0.0, 0.0, 0.0, 0.0));
			}
		}
	}
	al_unlock_bitmap(bp);
	al_restore_state(&old_state);
}

void pa_draw_tiled_background(ALLEGRO_BITMAP * bp, int x, int y, int width, int height, int scale)
{
	int i, j;
	int tw, th;
	bool held = al_is_bitmap_drawing_held();

	if(held)
	{
		al_hold_bitmap_drawing(false);
	}
	al_hold_bitmap_drawing(true);
	tw = width / scale + 1;
	th = height / scale + 1;
	for(i = 0; i < th; i++)
	{
		for(j = 0; j < tw; j++)
		{
			t3f_draw_scaled_bitmap(bp, t3f_color_white, x + j * scale, y + i * scale, 0, scale, scale, 0);
		}
	}
	al_hold_bitmap_drawing(false);
	if(held)
	{
		al_hold_bitmap_drawing(true);
	}
}
