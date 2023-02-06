#include "canvas_editor.h"
#include "modules/canvas/canvas_helpers.h"
#include "modules/dynamic_array.h"
#include "modules/bitmap.h"
#include "selection.h"
#include "undo/undo.h"
#include "undo/selection/selection.h"
#include "ui/window.h"

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

static ALLEGRO_BITMAP * merge_clipboard(PA_CANVAS_EDITOR * cep)
{
	ALLEGRO_BITMAP * bp;
	ALLEGRO_STATE old_state;
	int i;

	bp = al_create_bitmap(cep->clipboard.bitmap_stack->width, cep->clipboard.bitmap_stack->height);
	if(bp)
	{
		al_store_state(&old_state, ALLEGRO_STATE_TARGET_BITMAP | ALLEGRO_STATE_TRANSFORM | ALLEGRO_STATE_BLENDER);
		al_set_target_bitmap(bp);
		al_use_shader(cep->premultiplied_alpha_shader);
		al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);
		al_draw_filled_rectangle(0, 0, al_get_bitmap_width(bp), al_get_bitmap_height(bp), al_map_rgba_f(0.0, 0.0, 0.0, 0.0));
		al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA);
		for(i = 0; i < cep->clipboard.layer_max; i++)
		{
			if(cep->clipboard.bitmap_stack->bitmap[i])
			{
				al_draw_bitmap(cep->clipboard.bitmap_stack->bitmap[i], 0, 0, 0);
			}
		}
		al_restore_state(&old_state);
		pa_unpremultiply_bitmap_alpha(bp);
	}
	return bp;
}

void pa_apply_paste_clipboard(PA_CANVAS_EDITOR * cep, int pos, int ox, int oy, bool merge, bool no_undo)
{
	char undo_path[1024];
	ALLEGRO_STATE old_state;
	int x, y, i, c = -1;

	if(cep->clipboard.bitmap_stack && (cep->clipboard.layer >= 0 || cep->clipboard.layer_max == cep->canvas->layer_max))
	{
		al_store_state(&old_state, ALLEGRO_STATE_NEW_BITMAP_PARAMETERS);
		al_set_new_bitmap_flags(ALLEGRO_NO_PREMULTIPLIED_ALPHA);
		if(cep->selection.bitmap_stack)
		{
			pa_unfloat_canvas_editor_selection(cep, &cep->selection.box);
		}
		if(!no_undo)
		{
			if(pa_make_paste_undo(cep, pos, ox, oy, merge, pa_get_undo_path("undo", cep->undo_count, undo_path, 1024)))
			{
				pa_finalize_undo(cep);
			}
		}
		cep->selection.bitmap_stack = pa_create_bitmap_stack(cep->clipboard.bitmap_stack->layers, cep->clipboard.bitmap_stack->width, cep->clipboard.bitmap_stack->height);
		if(cep->selection.bitmap_stack)
		{
			if(merge)
			{
				cep->selection.bitmap_stack->bitmap[cep->current_layer] = merge_clipboard(cep);
				cep->selection.layer_max = cep->clipboard.layer_max;
				cep->selection.layer = cep->current_layer;
			}
			else
			{
				for(i = 0; i < cep->clipboard.layer_max; i++)
				{
					if(cep->clipboard.bitmap_stack->bitmap[i])
					{
						cep->selection.bitmap_stack->bitmap[i] = al_clone_bitmap(cep->clipboard.bitmap_stack->bitmap[i]);
						c = i;
					}
				}
				cep->selection.layer_max = cep->clipboard.layer_max;
				cep->selection.layer = cep->clipboard.layer;
			}
			switch(pos)
			{
				case 0:
				{
					x = cep->view_x + cep->view_width / 2 - cep->selection.bitmap_stack->width / 2;
					y = cep->view_y + cep->view_height / 2 - cep->selection.bitmap_stack->height / 2;
					break;
				}
				case 1:
				{
					x = cep->clipboard.x;
					y = cep->clipboard.y;
					break;
				}
				case 2:
				{
					x = cep->view_x + (t3gui_get_mouse_x() + ox) / cep->view_zoom - cep->selection.bitmap_stack->width / 2;
					y = cep->view_y + (t3gui_get_mouse_y() + oy) / cep->view_zoom - cep->selection.bitmap_stack->height / 2;
					break;
				}
				case 3:
				{
					x = ox;
					y = oy;
					break;
				}
			}
			pa_initialize_box(&cep->selection.box, x, y, cep->selection.bitmap_stack->width, cep->selection.bitmap_stack->height);
			pa_update_box_handles(&cep->selection.box, cep->view_x, cep->view_y, cep->view_zoom, true);
			cep->current_tool = PA_TOOL_SELECTION;
			al_restore_state(&old_state);
			if(cep->selection.filter)
			{
				pa_set_bitmap_stack_flags(cep->selection.bitmap_stack, ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);
			}
			t3f_refresh_menus();
		}
	}
}

void pa_paste_clipboard(PA_CANVAS_EDITOR * cep, int pos, int ox, int oy, bool merge)
{
	t3f_debug_message("Enter pa_menu_edit_paste()\n");

	pa_apply_paste_clipboard(cep, pos, ox, oy, merge, false);
	cep->modified++;
	pa_set_window_message(NULL);
	t3f_debug_message("Exit pa_menu_edit_paste()\n");
}
