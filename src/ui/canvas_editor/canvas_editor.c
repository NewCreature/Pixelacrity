#include "t3f/t3f.h"
#include "t3gui/t3gui.h"
#include "canvas_editor.h"
#include "modules/canvas/canvas_helpers.h"
#include "modules/primitives.h"
#include "undo.h"
#include "undo_selection.h"

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
	if(cep->selection.bitmap)
	{
		al_destroy_bitmap(cep->selection.bitmap);
	}
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
	cep->view_fx = cep->view_x;
	cep->view_fy = cep->view_y;
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

static bool create_float_undo(QUIXEL_CANVAS_EDITOR * cep, QUIXEL_BOX * bp)
{
	char undo_path[1024];

	quixel_get_undo_path("undo", cep->undo_count, undo_path, 1024);
	if(quixel_make_float_selection_undo(cep, bp, undo_path))
	{
		return true;
	}
	return false;
}

bool quixel_handle_float_canvas_editor_selection(QUIXEL_CANVAS_EDITOR * cep, QUIXEL_BOX * bp)
{
	ALLEGRO_STATE old_state;
	ALLEGRO_TRANSFORM identity;

	t3f_debug_message("Enter quixel_handle_float_canvas_editor_selection()\n");
	al_store_state(&old_state, ALLEGRO_STATE_BLENDER | ALLEGRO_STATE_TRANSFORM | ALLEGRO_STATE_TARGET_BITMAP);
	al_set_new_bitmap_flags(0);
	cep->selection.bitmap = al_create_bitmap(bp->width, bp->height);
	if(!cep->selection.bitmap)
	{
		goto fail;
	}
	cep->selection.combined_bitmap = al_create_bitmap(bp->width, bp->height);
	if(!cep->selection.combined_bitmap)
	{
		goto fail;
	}
	al_set_target_bitmap(cep->selection.bitmap);
	al_identity_transform(&identity);
	al_use_transform(&identity);
	al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);
	al_clear_to_color(al_map_rgba_f(0.0, 0.0, 0.0, 0.0));
	quixel_render_canvas_layer(cep->canvas, cep->current_layer, bp->start_x, bp->start_y, 1, 0, 0, bp->width, bp->height);
	al_restore_state(&old_state);
	quixel_draw_primitive_to_canvas(cep->canvas, cep->current_layer, bp->start_x, bp->start_y, bp->start_x + bp->width - 1, bp->start_y + bp->height - 1, NULL, al_map_rgba_f(0, 0, 0, 0), QUIXEL_RENDER_COPY, quixel_draw_filled_rectangle);
	cep->modified++;
	t3f_debug_message("Exit quixel_handle_float_canvas_editor_selection()\n");

	return true;

	fail:
	{
		t3f_debug_message("Fail quixel_handle_float_canvas_editor_selection()\n");
		if(cep->selection.bitmap)
		{
			al_destroy_bitmap(cep->selection.bitmap);
			cep->selection.bitmap = NULL;
		}
		if(cep->selection.combined_bitmap)
		{
			al_destroy_bitmap(cep->selection.combined_bitmap);
			cep->selection.combined_bitmap = NULL;
		}
		al_restore_state(&old_state);
		return false;
	}
}

void quixel_float_canvas_editor_selection(QUIXEL_CANVAS_EDITOR * cep, QUIXEL_BOX * bp)
{
	t3f_debug_message("Enter quixel_float_canvas_editor_selection()\n");
	create_float_undo(cep, bp);
	quixel_finalize_undo(cep);
	quixel_handle_float_canvas_editor_selection(cep, bp);
	t3f_debug_message("Exit quixel_float_canvas_editor_selection()\n");
}

void quixel_handle_unfloat_canvas_editor_selection(QUIXEL_CANVAS_EDITOR * cep, QUIXEL_BOX * bp)
{
	t3f_debug_message("Enter quixel_handle_unfloat_canvas_editor_selection()\n");
	if(quixel_handle_canvas_expansion(cep->canvas, cep->selection.box.start_x, cep->selection.box.start_y, cep->selection.box.end_x, cep->selection.box.end_y, &cep->shift_x, &cep->shift_y))
	{
		quixel_shift_canvas_editor_variables(cep, cep->shift_x * cep->canvas->bitmap_size, cep->shift_y * cep->canvas->bitmap_size);
	}
	quixel_draw_primitive_to_canvas(cep->canvas, cep->current_layer, bp->start_x, bp->start_y, bp->start_x + bp->width, bp->start_y + bp->height, cep->selection.combined_bitmap, al_map_rgba_f(0, 0, 0, 0), QUIXEL_RENDER_COPY, quixel_draw_quad);
	al_destroy_bitmap(cep->selection.bitmap);
	cep->selection.bitmap = NULL;
	cep->modified++;
	t3f_debug_message("Exit quixel_handle_unfloat_canvas_editor_selection()\n");
}

void quixel_unfloat_canvas_editor_selection(QUIXEL_CANVAS_EDITOR * cep, QUIXEL_BOX * bp)
{
	t3f_debug_message("Enter quixel_unfloat_canvas_editor_selection()\n");
	if(create_unfloat_undo(cep))
	{
		quixel_finalize_undo(cep);
	}
	quixel_handle_unfloat_canvas_editor_selection(cep, bp);
	t3f_debug_message("Exit quixel_unfloat_canvas_editor_selection()\n");
}

void quixel_shift_canvas_editor_variables(QUIXEL_CANVAS_EDITOR * cep, int ox, int oy)
{
	cep->view_x += ox;
	cep->view_y += oy;
	cep->view_fx = cep->view_x;
	cep->view_fy = cep->view_y;
	cep->click_x += ox;
	cep->click_y += oy;
	cep->release_x += ox;
	cep->release_y += oy;
	cep->hover_x += ox;
	cep->hover_y += oy;
	cep->scratch_offset_x += ox;
	cep->scratch_offset_y += oy;
	cep->selection.box.start_x += ox;
	cep->selection.box.start_y += oy;
	cep->selection.box.middle_x += ox;
	cep->selection.box.middle_y += oy;
	cep->selection.box.end_x += ox;
	cep->selection.box.end_y += oy;
}

void quixel_select_canvas_editor_tool(QUIXEL_CANVAS_EDITOR * cep, int tool)
{
	if(cep->current_tool == QUIXEL_TOOL_SELECTION && tool != QUIXEL_TOOL_SELECTION)
	{
		if(cep->selection.bitmap)
		{
			quixel_unfloat_canvas_editor_selection(cep, &cep->selection.box);
		}
		cep->selection.box.width = 0;
		cep->selection.box.height = 0;
	}
	cep->current_tool = tool;
}

bool quixel_import_image(QUIXEL_CANVAS_EDITOR * cep, const char * fn)
{
	t3f_debug_message("Enter quixel_import_image()\n");
	cep->selection.bitmap = al_load_bitmap_flags(fn, ALLEGRO_NO_PREMULTIPLIED_ALPHA);
	if(cep->selection.bitmap)
	{
		cep->selection.combined_bitmap = al_create_bitmap(al_get_bitmap_width(cep->selection.bitmap), al_get_bitmap_height(cep->selection.bitmap));
		if(!cep->selection.combined_bitmap)
		{
			goto fail;
		}
		quixel_select_canvas_editor_tool(cep, QUIXEL_TOOL_SELECTION);
		quixel_initialize_box(&cep->selection.box, 0, 0, al_get_bitmap_width(cep->selection.bitmap), al_get_bitmap_height(cep->selection.bitmap), cep->peg_bitmap);
		quixel_update_box_handles(&cep->selection.box, cep->view_x, cep->view_y, cep->view_zoom);
	}
	t3f_debug_message("Exit quixel_import_image()\n");
	return true;

	fail:
	{
		t3f_debug_message("Fail quixel_import_image()\n");
		if(cep->selection.bitmap)
		{
			al_destroy_bitmap(cep->selection.bitmap);
			cep->selection.bitmap = NULL;
		}
		if(cep->selection.combined_bitmap)
		{
			al_destroy_bitmap(cep->selection.combined_bitmap);
			cep->selection.combined_bitmap = NULL;
		}
		return false;
	}
}
