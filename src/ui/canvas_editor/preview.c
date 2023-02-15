#include "preview.h"
#include "modules/canvas/canvas.h"
#include "modules/canvas/canvas_helpers.h"
#include "modules/pixel_shader.h"
#include "modules/bitmap.h"

PA_CANVAS_EDITOR_PREVIEW * pa_create_canvas_editor_preview(int width, int height)
{
  PA_CANVAS_EDITOR_PREVIEW * pp;

  pp = malloc(sizeof(PA_CANVAS_EDITOR_PREVIEW));
  if(pp)
  {
    memset(pp, 0, sizeof(PA_CANVAS_EDITOR_PREVIEW));
    pp->width = width;
    pp->height = height;
		pp->background_bitmap = pa_make_checkerboard_bitmap(t3f_color_white, al_map_rgba_f(0.9, 0.9, 0.9, 1.0));
		if(!pp->background_bitmap)
		{
			goto fail;
		}
  }
  return pp;

	fail:
	{
		if(pp)
		{
			pa_destroy_canvas_editor_preview(pp);
		}
	}
	return NULL;
}

void pa_resize_canvas_editor_preview(PA_CANVAS_EDITOR_PREVIEW * pp, int width, int height)
{
	bool recreate = false;

	if(pp->bitmap)
	{
		al_destroy_bitmap(pp->bitmap);
		recreate = true;
	}
	pp->width = width;
	pp->height = height;
	if(recreate)
	{
		pp->bitmap = al_create_bitmap(pp->width, pp->height);
	}
}

void pa_destroy_canvas_editor_preview(PA_CANVAS_EDITOR_PREVIEW * pp)
{
	if(pp->background_bitmap)
	{
		al_destroy_bitmap(pp->background_bitmap);
	}
  if(pp->bitmap)
  {
    al_destroy_bitmap(pp->bitmap);
  }
  free(pp);
}

void pa_update_canvas_editor_preview(PA_CANVAS_EDITOR_PREVIEW * pp, PA_CANVAS * cp, ALLEGRO_SHADER * shader)
{
	ALLEGRO_STATE old_state;
	ALLEGRO_TRANSFORM transform;
	int i;

	al_store_state(&old_state, ALLEGRO_STATE_TRANSFORM | ALLEGRO_STATE_TARGET_BITMAP | ALLEGRO_STATE_BLENDER);
	pa_get_canvas_dimensions(cp, &pp->canvas_x, &pp->canvas_y, &pp->canvas_width, &pp->canvas_height, 0, false);
	if(pp->canvas_width < 1 || pp->canvas_height < 1)
	{
		return;
	}
  if(!pp->bitmap)
  {
    pp->bitmap = al_create_bitmap(pp->width, pp->height);
  }
	if(!pp->overlay)
	{
		pp->overlay = al_create_bitmap(pp->width, pp->height);
	}
  if(!pp->bitmap)
  {
    return;
  }
	pp->scale = (float)al_get_bitmap_width(pp->bitmap) / (float)pp->canvas_width;
	if((float)pp->canvas_height * pp->scale > al_get_bitmap_height(pp->bitmap))
	{
		pp->scale = (float)al_get_bitmap_height(pp->bitmap) / (float)pp->canvas_height;
		pp->offset_x = al_get_bitmap_width(pp->bitmap) / 2.0 - (pp->canvas_width * pp->scale) / 2.0;
	}
	else
	{
		pp->offset_y = al_get_bitmap_height(pp->bitmap) / 2.0 - (pp->canvas_height * pp->scale) / 2.0;
	}
	al_set_target_bitmap(pp->bitmap);
	pa_set_target_pixel_shader(shader);
	al_identity_transform(&transform);
	al_use_transform(&transform);
	pa_draw_tiled_background(pp->background_bitmap, 0, 0, al_get_bitmap_width(pp->bitmap), al_get_bitmap_height(pp->bitmap), pp->background_scale);
	al_build_transform(&transform, pp->offset_x, pp->offset_y, pp->scale, pp->scale, 0.0);
	al_use_transform(&transform);
	for(i = 0; i < cp->layer_max; i++)
	{
		al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA);
		if(!(cp->layer[i]->flags & PA_CANVAS_FLAG_HIDDEN))
		{
			pa_draw_canvas_layer(cp, i, cp->layer[i]->flags, pp->bitmap, pp->canvas_x, pp->canvas_y, pp->canvas_width, pp->canvas_height);
		}
	}
	al_restore_state(&old_state);
}

void pa_update_canvas_editor_preview_overlay(PA_CANVAS_EDITOR_PREVIEW * pp, int x, int y, int width, int height)
{
	ALLEGRO_STATE old_state;
	ALLEGRO_TRANSFORM transform;
	int start_x, start_y;

	al_store_state(&old_state, ALLEGRO_STATE_TRANSFORM | ALLEGRO_STATE_TARGET_BITMAP | ALLEGRO_STATE_BLENDER);
	al_set_target_bitmap(pp->overlay);
	al_identity_transform(&transform);
	al_use_transform(&transform);
	al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);
	al_clear_to_color(al_map_rgba_f(0.0, 0.0, 0.0, 0.5));
	start_x = pp->offset_x + x * pp->scale - pp->canvas_x * pp->scale;
	start_y = pp->offset_y + y * pp->scale - pp->canvas_y * pp->scale;
	al_draw_filled_rectangle(start_x, start_y, start_x + width * pp->scale, start_y + height * pp->scale, al_map_rgba_f(0.0, 0.0, 0.0, 0.0));
	al_restore_state(&old_state);
}
