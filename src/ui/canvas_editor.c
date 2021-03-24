#include "t3f/t3f.h"
#include "t3gui/t3gui.h"
#include "canvas_editor.h"
#include "canvas_helpers.h"
#include "primitives.h"

static int get_config_val(ALLEGRO_CONFIG * cp, const char * section, const char * key, int default_val)
{
	const char * val;

	val = al_get_config_value(cp, section, key);
	if(val)
	{
		return atoi(val);
	}
	return default_val;
}

QUIXEL_CANVAS_EDITOR * quixel_create_canvas_editor(QUIXEL_CANVAS * cp)
{
	QUIXEL_CANVAS_EDITOR * cep;
	ALLEGRO_STATE old_state;

	cep = malloc(sizeof(QUIXEL_CANVAS_EDITOR));
	if(!cep)
	{
		goto fail;
	}
	memset(cep, 0, sizeof(QUIXEL_CANVAS_EDITOR));
	al_store_state(&old_state, ALLEGRO_STATE_NEW_BITMAP_PARAMETERS);
	al_set_new_bitmap_flags(0);
	cep->scratch_bitmap = al_create_bitmap(al_get_display_width(t3f_display), al_get_display_height(t3f_display));
	al_restore_state(&old_state);
	if(!cep->scratch_bitmap)
	{
		goto fail;
	}
	cep->peg_bitmap = al_load_bitmap_flags("data/graphics/peg.png", ALLEGRO_NO_PREMULTIPLIED_ALPHA);
	if(!cep->peg_bitmap)
	{
		goto fail;
	}
	cep->canvas = cp;
	cep->left_base_color = al_map_rgba_f(1.0, 0.0, 0.0, 1.0);
	cep->right_base_color = al_map_rgba_f(0.0, 0.0, 0.0, 0.0);
	cep->current_layer = get_config_val(cep->canvas->config, "state", "current_layer", 0);
	cep->view_x = get_config_val(cep->canvas->config, "state", "view_x", cep->canvas->bitmap_size * 8 + cep->canvas->bitmap_size / 2);
	cep->view_y = get_config_val(cep->canvas->config, "state", "view_y", cep->canvas->bitmap_size * 8 + cep->canvas->bitmap_size / 2);
	cep->view_zoom = get_config_val(cep->canvas->config, "state", "view_zoom", 8);
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
	if(cep->peg_bitmap)
	{
		al_destroy_bitmap(cep->peg_bitmap);
	}
	if(cep->scratch_bitmap)
	{
		al_destroy_bitmap(cep->scratch_bitmap);
	}
	free(cep);
}

void quixel_center_canvas_editor(QUIXEL_CANVAS_EDITOR * cep, int frame)
{
	int x, y, w, h;

	if(frame < cep->canvas->frame_max)
	{
		cep->view_x = cep->canvas->frame[frame]->box.start_x + cep->canvas->frame[frame]->box.width / 2 - (cep->editor_element->w / cep->view_zoom) / 2;
		cep->view_y = cep->canvas->frame[frame]->box.start_y + cep->canvas->frame[frame]->box.height / 2 - (cep->editor_element->h / cep->view_zoom) / 2;
	}
	else
	{
		quixel_get_canvas_dimensions(cep->canvas, &x, &y, &w, &h, 0);
		cep->view_x = x + w / 2 - (cep->editor_element->w / cep->view_zoom) / 2;
		cep->view_y = y + h / 2 - (cep->editor_element->h / cep->view_zoom) / 2;
	}
}

void quixel_unfloat_canvas_editor_selection(QUIXEL_CANVAS_EDITOR * cep, QUIXEL_BOX * bp)
{
	quixel_draw_primitive_to_canvas(cep->canvas, cep->current_layer, bp->start_x, bp->start_y, bp->start_x + bp->width, bp->start_y + bp->height, cep->scratch_bitmap, al_map_rgba_f(0, 0, 0, 0), QUIXEL_RENDER_COMPOSITE, quixel_draw_quad);
	cep->selection.floating = false;
}

bool quixel_make_canvas_editor_undo_state(QUIXEL_CANVAS_EDITOR * cep, int layer, int x, int y, int width, int height, const char * fn)
{
	ALLEGRO_STATE old_state;
	ALLEGRO_BITMAP * bp = NULL;
	ALLEGRO_FILE * fp = NULL;

	al_store_state(&old_state, ALLEGRO_STATE_NEW_BITMAP_PARAMETERS);
	al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
	if(width <= 0 || height <= 0)
	{
		quixel_get_canvas_dimensions(cep->canvas, &x, &y, &width, &height, 0);
	}
	if(width > 0 && height > 0)
	{
		bp = al_create_bitmap(width, height);
		if(!bp)
		{
			goto fail;
		}
	}
	al_restore_state(&old_state);
	if(width > 0 && height > 0)
	{
		quixel_render_canvas_to_bitmap(cep->canvas, layer, layer + 1, x, y, width, height, 0, bp);
	}
	fp = al_fopen(fn, "wb");
	if(!fp)
	{
		printf("fail: %s\n", fn);
		goto fail;
	}
	if(width < 0 || height < 0)
	{
		al_fputc(fp, 0);
	}
	else
	{
		al_fputc(fp, 1);
		al_fwrite32le(fp, x);
		al_fwrite32le(fp, y);
		if(!al_save_bitmap_f(fp, ".png", bp))
		{
			printf("fail: %s\n", fn);
			goto fail;
		}
	}
	al_fclose(fp);
	return true;

	fail:
	{
		if(fp)
		{
			al_fclose(fp);
		}
		if(bp)
		{
			al_destroy_bitmap(bp);
		}
		return false;
	}
}

bool quixel_apply_canvas_editor_undo_state(QUIXEL_CANVAS_EDITOR * cep, const char * fn)
{
	return false;
}
