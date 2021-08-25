#include "t3f/t3f.h"
#include "canvas_editor.h"
#include "undo.h"
#include "modules/canvas/canvas_helpers.h"
#include "modules/primitives.h"
#include "modules/dynamic_array.h"
#include "clipboard.h"
#include "undo_selection.h"
#include "ui/window.h"

void pa_free_selection(PA_CANVAS_EDITOR * cep)
{
	int i;

	t3f_debug_message("Enter pa_free_selection()\n");
	if(cep->selection.bitmap)
	{
		for(i = 0; i < cep->selection.layer_max; i++)
		{
			if(cep->selection.bitmap[i])
			{
				al_destroy_bitmap(cep->selection.bitmap[i]);
			}
		}
		pa_free((void **)cep->selection.bitmap);
		cep->selection.bitmap = NULL;
	}
	t3f_debug_message("Exit pa_free_selection()\n");
}

void pa_clear_canvas_editor_selection(PA_CANVAS_EDITOR * cep)
{
	pa_free_selection(cep);
	cep->selection.box.width = 0;
	cep->selection.box.height = 0;
	cep->selection.box.state = PA_BOX_STATE_IDLE;
}

static bool create_unfloat_undo(PA_CANVAS_EDITOR * cep)
{
	char undo_path[1024];

	pa_get_undo_path("undo", cep->undo_count, undo_path, 1024);
	if(pa_make_unfloat_selection_undo(cep, undo_path))
	{
		return true;
	}
	return false;
}

static bool create_float_undo(PA_CANVAS_EDITOR * cep, PA_BOX * bp)
{
	char undo_path[1024];

	pa_get_undo_path("undo", cep->undo_count, undo_path, 1024);
	if(pa_make_float_selection_undo(cep, bp, undo_path))
	{
		return true;
	}
	return false;
}

static void free_selection(PA_CANVAS_EDITOR * cep)
{
	int i;

	if(cep->selection.bitmap)
	{
		for(i = 0; i < cep->selection.layer_max; i++)
		{
			if(cep->selection.bitmap[i])
			{
				al_destroy_bitmap(cep->selection.bitmap[i]);
			}
		}
		pa_free((void **)cep->selection.bitmap);
		cep->selection.bitmap = NULL;
	}
}

bool pa_handle_float_canvas_editor_selection(PA_CANVAS_EDITOR * cep, PA_BOX * bp, bool multilayer)
{
	ALLEGRO_STATE old_state;
	ALLEGRO_TRANSFORM identity;
	int i;

	t3f_debug_message("Enter pa_handle_float_canvas_editor_selection()\n");
	al_store_state(&old_state, ALLEGRO_STATE_BLENDER | ALLEGRO_STATE_TRANSFORM | ALLEGRO_STATE_TARGET_BITMAP);
	al_set_new_bitmap_flags(0);
	cep->selection.bitmap = (ALLEGRO_BITMAP **)pa_malloc(sizeof(ALLEGRO_BITMAP *), cep->canvas->layer_max);
	if(!cep->selection.bitmap)
	{
		goto fail;
	}
	cep->selection.layer_max = cep->canvas->layer_max;
	if(multilayer)
	{
		cep->selection.layer = -1;
	}
	else
	{
		cep->selection.layer = cep->current_layer;
	}
	if(cep->selection.layer >= 0)
	{
		cep->selection.bitmap[cep->current_layer] = al_create_bitmap(bp->width, bp->height);
		if(!cep->selection.bitmap[cep->current_layer])
		{
			goto fail;
		}
		al_set_target_bitmap(cep->selection.bitmap[cep->current_layer]);
		al_identity_transform(&identity);
		al_use_transform(&identity);
		al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);
		al_clear_to_color(al_map_rgba_f(0.0, 0.0, 0.0, 0.0));
		pa_render_canvas_layer(cep->canvas, cep->current_layer, bp->start_x, bp->start_y, 1, 0, 0, bp->width, bp->height);
		al_restore_state(&old_state);
		pa_draw_primitive_to_canvas(cep->canvas, cep->current_layer, bp->start_x, bp->start_y, bp->start_x + bp->width - 1, bp->start_y + bp->height - 1, NULL, al_map_rgba_f(0, 0, 0, 0), NULL, PA_RENDER_COPY, NULL, pa_draw_filled_rectangle);
	}
	else
	{
		for(i = 0; i < cep->selection.layer_max; i++)
		{
			cep->selection.bitmap[i] = al_create_bitmap(bp->width, bp->height);
			if(!cep->selection.bitmap[i])
			{
				goto fail;
			}
			al_set_target_bitmap(cep->selection.bitmap[i]);
			al_identity_transform(&identity);
			al_use_transform(&identity);
			al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);
			al_clear_to_color(al_map_rgba_f(0.0, 0.0, 0.0, 0.0));
			pa_render_canvas_layer(cep->canvas, i, bp->start_x, bp->start_y, 1, 0, 0, bp->width, bp->height);
			al_restore_state(&old_state);
			pa_draw_primitive_to_canvas(cep->canvas, i, bp->start_x, bp->start_y, bp->start_x + bp->width - 1, bp->start_y + bp->height - 1, NULL, al_map_rgba_f(0, 0, 0, 0), NULL, PA_RENDER_COPY, NULL, pa_draw_filled_rectangle);
		}
	}
	cep->modified++;
	pa_set_window_message(NULL);
	t3f_debug_message("Exit pa_handle_float_canvas_editor_selection()\n");

	return true;

	fail:
	{
		t3f_debug_message("Fail pa_handle_float_canvas_editor_selection()\n");
		free_selection(cep);
		al_restore_state(&old_state);
		return false;
	}
}

void pa_float_canvas_editor_selection(PA_CANVAS_EDITOR * cep, PA_BOX * bp, bool multilayer)
{
	t3f_debug_message("Enter pa_float_canvas_editor_selection()\n");
	create_float_undo(cep, bp);
	pa_finalize_undo(cep);
	pa_handle_float_canvas_editor_selection(cep, bp, multilayer);
	t3f_debug_message("Exit pa_float_canvas_editor_selection()\n");
}

void pa_handle_unfloat_canvas_editor_selection(PA_CANVAS_EDITOR * cep, PA_BOX * bp)
{
	int i;

	t3f_debug_message("Enter pa_handle_unfloat_canvas_editor_selection()\n");
	if(pa_handle_canvas_expansion(cep->canvas, cep->selection.box.start_x, cep->selection.box.start_y, cep->selection.box.end_x, cep->selection.box.end_y, &cep->shift_x, &cep->shift_y))
	{
		pa_shift_canvas_editor_variables(cep, cep->shift_x * cep->canvas->bitmap_size, cep->shift_y * cep->canvas->bitmap_size);
	}
	if(cep->selection.layer >= 0)
	{
		pa_draw_primitive_to_canvas(cep->canvas, cep->current_layer, bp->start_x, bp->start_y, bp->start_x + bp->width, bp->start_y + bp->height, NULL, al_map_rgba_f(0, 0, 0, 0), cep->selection.bitmap[cep->selection.layer],  PA_RENDER_COPY, cep->conditional_copy_shader, pa_draw_quad);
		al_use_shader(cep->standard_shader);
	}
	else
	{
		for(i = 0; i < cep->selection.layer_max; i++)
		{
			if(cep->selection.bitmap[i])
			{
				pa_draw_primitive_to_canvas(cep->canvas, i, bp->start_x, bp->start_y, bp->start_x + bp->width, bp->start_y + bp->height, NULL, al_map_rgba_f(0, 0, 0, 0), cep->selection.bitmap[i], PA_RENDER_COPY, cep->conditional_copy_shader, pa_draw_quad);
				al_use_shader(cep->standard_shader);
			}
		}
	}
	if(cep->selection.bitmap)
	{

	}
	free_selection(cep);
	cep->modified++;
	t3f_debug_message("Exit pa_handle_unfloat_canvas_editor_selection()\n");
}

void pa_unfloat_canvas_editor_selection(PA_CANVAS_EDITOR * cep, PA_BOX * bp)
{
	t3f_debug_message("Enter pa_unfloat_canvas_editor_selection()\n");
	if(create_unfloat_undo(cep))
	{
		pa_finalize_undo(cep);
	}
	pa_handle_unfloat_canvas_editor_selection(cep, bp);
	t3f_debug_message("Exit pa_unfloat_canvas_editor_selection()\n");
}

void pa_update_selection(PA_CANVAS_EDITOR * canvas_editor, T3GUI_ELEMENT * d)
{
	PA_BOX old_box;
	bool snap = false;
	bool multilayer = false;

	if(canvas_editor->selection.box.width > 0 && canvas_editor->selection.box.height > 0)
	{
		memcpy(&old_box, &canvas_editor->selection.box, sizeof(PA_BOX));
		pa_update_box_handles(&canvas_editor->selection.box, canvas_editor->view_x, canvas_editor->view_y, canvas_editor->view_zoom);
		if(t3f_key[ALLEGRO_KEY_LCTRL] || t3f_key[ALLEGRO_KEY_RCTRL])
		{
			snap = true;
		}
		if(t3f_key[ALLEGRO_KEY_LSHIFT] || t3f_key[ALLEGRO_KEY_RSHIFT])
		{
			multilayer = true;
		}
		pa_box_logic(&canvas_editor->selection.box, canvas_editor->view_x, canvas_editor->view_y, canvas_editor->view_zoom, d->x, d->y, snap);
		if(canvas_editor->selection.box.state == PA_BOX_STATE_HOVER)
		{
			canvas_editor->want_cursor = ALLEGRO_SYSTEM_MOUSE_CURSOR_LINK;
		}
		else if(canvas_editor->selection.box.state == PA_BOX_STATE_HOVER_HANDLE)
		{
			canvas_editor->want_cursor = ALLEGRO_SYSTEM_MOUSE_CURSOR_LINK;
		}
		else if(canvas_editor->selection.box.state == PA_BOX_STATE_MOVING)
		{
			canvas_editor->want_cursor = ALLEGRO_SYSTEM_MOUSE_CURSOR_MOVE;
		}
		else if(canvas_editor->selection.box.state == PA_BOX_STATE_RESIZING)
		{
			canvas_editor->want_cursor = ALLEGRO_SYSTEM_MOUSE_CURSOR_LINK;
		}
		else
		{
			canvas_editor->want_cursor = ALLEGRO_SYSTEM_MOUSE_CURSOR_PRECISION;
		}
		if(!canvas_editor->selection.bitmap && (canvas_editor->selection.box.start_x != old_box.start_x || canvas_editor->selection.box.start_y != old_box.start_y))
		{
			if(canvas_editor->selection.box.state == PA_BOX_STATE_MOVING)
			{
				pa_float_canvas_editor_selection(canvas_editor, &old_box, multilayer);
			}
		}
	}
}

bool pa_add_layer_to_selection(PA_CANVAS_EDITOR * cep, int layer)
{
	ALLEGRO_BITMAP ** old_bitmap;
	int i;

	t3f_debug_message("Enter pa_add_layer_to_selection()\n");
	if(layer < 0)
	{
		layer = cep->selection.layer_max;
	}
	old_bitmap = cep->selection.bitmap;
	cep->selection.bitmap = (ALLEGRO_BITMAP **)pa_malloc(sizeof(ALLEGRO_BITMAP *), cep->selection.layer_max + 1);
	if(cep->selection.bitmap)
	{
		for(i = 0; i < layer; i++)
		{
			cep->selection.bitmap[i] = old_bitmap[i];
		}
		for(i = layer + 1; i < cep->selection.layer_max + 1; i++)
		{
			cep->selection.bitmap[i] = old_bitmap[i - 1];
		}
		cep->selection.bitmap[layer] = al_create_bitmap(al_get_bitmap_width(cep->selection.bitmap[0]), al_get_bitmap_height(cep->selection.bitmap[0]));
		if(cep->selection.bitmap[layer])
		{
			cep->selection.layer_max++;
			pa_free((void **)old_bitmap);
			t3f_debug_message("pa_add_layer_to_selection() success\n");
			return true;
		}
	}
	t3f_debug_message("pa_add_layer_to_selection() fail\n");
	return false;
}

bool pa_remove_layer_from_selection(PA_CANVAS_EDITOR * cep, int layer)
{
	ALLEGRO_BITMAP ** old_bitmap;
	int i;

	t3f_debug_message("Enter pa_remove_layer_from_selection()\n");
	old_bitmap = cep->selection.bitmap;
	cep->selection.bitmap = (ALLEGRO_BITMAP **)pa_malloc(sizeof(ALLEGRO_BITMAP *), cep->selection.layer_max - 1);
	if(cep->selection.bitmap)
	{
		for(i = 0; i < layer; i++)
		{
			cep->selection.bitmap[i] = old_bitmap[i];
		}
		al_destroy_bitmap(cep->selection.bitmap[layer]);
		for(i = layer; i < cep->selection.layer_max - 1; i++)
		{
			cep->selection.bitmap[i] = old_bitmap[i + 1];
		}
		cep->selection.layer_max--;
		pa_free((void **)old_bitmap);
		t3f_debug_message("pa_remove_layer_from_selection() success\n");
		return true;
	}
	t3f_debug_message("pa_remove_layer_from_selection() fail\n");
	return false;
}
