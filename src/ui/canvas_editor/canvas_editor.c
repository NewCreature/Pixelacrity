#include "t3f/t3f.h"
#include "t3gui/t3gui.h"
#include "canvas_editor.h"
#include "modules/canvas/canvas_helpers.h"
#include "modules/primitives.h"
#include "undo.h"

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
	cep->backup_tick = QUIXEL_BACKUP_INTERVAL;
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

static bool create_unfloat_undo(QUIXEL_CANVAS_EDITOR * cep)
{
	char undo_path[1024];

	quixel_get_undo_path("undo", cep->undo_count, undo_path, 1024);
	if(quixel_make_unfloat_selection_undo(cep, undo_path))
	{
		return true;
	}
	return false;
}

static bool create_float_undo(QUIXEL_CANVAS_EDITOR * cep)
{
	char undo_path[1024];

	quixel_get_undo_path("undo", cep->undo_count, undo_path, 1024);
	if(quixel_make_float_selection_undo(cep, undo_path))
	{
		return true;
	}
	return false;
}

void quixel_float_canvas_editor_selection(QUIXEL_CANVAS_EDITOR * cep, QUIXEL_BOX * bp)
{
	ALLEGRO_STATE old_state;
	ALLEGRO_TRANSFORM identity;

	create_float_undo(cep);
	quixel_finalize_undo(cep);
	al_store_state(&old_state, ALLEGRO_STATE_BLENDER | ALLEGRO_STATE_TRANSFORM | ALLEGRO_STATE_TARGET_BITMAP);
	al_set_target_bitmap(cep->scratch_bitmap);
	al_identity_transform(&identity);
	al_use_transform(&identity);
	al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);
	al_clear_to_color(al_map_rgba_f(0.0, 0.0, 0.0, 0.0));
	quixel_render_canvas_layer(cep->canvas, cep->current_layer, bp->start_x, bp->start_y, 1, 0, 0, bp->width, bp->height);
	al_restore_state(&old_state);
	quixel_draw_primitive_to_canvas(cep->canvas, cep->current_layer, bp->start_x, bp->start_y, bp->start_x + bp->width - 1, bp->start_y + bp->height - 1, NULL, al_map_rgba_f(0, 0, 0, 0), QUIXEL_RENDER_COPY, quixel_draw_filled_rectangle);
	cep->selection.floating = true;
}

void quixel_unfloat_canvas_editor_selection(QUIXEL_CANVAS_EDITOR * cep, QUIXEL_BOX * bp)
{
	if(create_unfloat_undo(cep))
	{
		quixel_finalize_undo(cep);
	}
	quixel_draw_primitive_to_canvas(cep->canvas, cep->current_layer, bp->start_x, bp->start_y, bp->start_x + bp->width, bp->start_y + bp->height, cep->scratch_bitmap, al_map_rgba_f(0, 0, 0, 0), QUIXEL_RENDER_COMPOSITE, quixel_draw_quad);
	cep->selection.floating = false;
}
