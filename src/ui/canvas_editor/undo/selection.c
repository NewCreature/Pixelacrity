#include "t3f/t3f.h"
#include "modules/canvas/canvas.h"
#include "modules/canvas/canvas_helpers.h"
#include "modules/primitives.h"
#include "modules/dynamic_array.h"
#include "ui/canvas_editor/canvas_editor.h"
#include "undo.h"
#include "selection.h"
#include "ui/canvas_editor/clipboard.h"
#include "ui/canvas_editor/selection.h"

bool pa_make_unfloat_selection_undo(PA_CANVAS_EDITOR * cep, const char * fn)
{
	ALLEGRO_STATE old_state;
	ALLEGRO_BITMAP * bp = NULL;
	ALLEGRO_FILE * fp = NULL;
	int i;

	t3f_debug_message("Enter pa_make_unfloat_selection_undo()\n");
	al_store_state(&old_state, ALLEGRO_STATE_NEW_BITMAP_PARAMETERS);
	al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
	bp = al_create_bitmap(cep->selection.box.width, cep->selection.box.height);
	al_restore_state(&old_state);
	if(!bp)
	{
		goto fail;
	}
	pa_get_canvas_shift(cep->canvas, cep->tool_left, cep->tool_top, &cep->shift_x, &cep->shift_y);
	fp = al_fopen(fn, "wb");
	if(!fp)
	{
		printf("fail: %s\n", fn);
		goto fail;
	}
	pa_write_undo_header(fp, cep, PA_UNDO_TYPE_UNFLOAT_SELECTION, "Unfloat Selection");
	al_fwrite32le(fp, cep->current_layer);
	al_fwrite32le(fp, cep->selection.box.start_x);
	al_fwrite32le(fp, cep->selection.box.start_y);
	al_fwrite32le(fp, cep->selection.box.width);
	al_fwrite32le(fp, cep->selection.box.height);
	al_fwrite32le(fp, cep->selection.layer_max);
	al_fwrite32le(fp, cep->selection.layer);
	for(i = 0; i < cep->selection.layer_max; i++)
	{
		if(cep->selection.bitmap_stack->bitmap[i])
		{
			al_fputc(fp, 1);
			pa_render_canvas_to_bitmap(cep->canvas, i, i + 1, cep->selection.box.start_x, cep->selection.box.start_y, cep->selection.box.width, cep->selection.box.height, 0, bp, NULL);
			if(!al_save_bitmap_f(fp, ".png", bp))
			{
				printf("float fail\n");
				goto fail;
			}
			if(!al_save_bitmap_f(fp, ".png", cep->selection.bitmap_stack->bitmap[i]))
			{
				printf("float fail\n");
				goto fail;
			}
		}
		else
		{
			al_fputc(fp, 0);
		}
	}
	al_destroy_bitmap(bp);
	al_fclose(fp);
	t3f_debug_message("Exit pa_make_unfloat_selection_undo()\n");
	return true;

	fail:
	{
		t3f_debug_message("Fail pa_make_unfloat_selection_undo()\n");
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

bool pa_make_unfloat_selection_redo(PA_CANVAS_EDITOR * cep, const char * fn)
{
	ALLEGRO_BITMAP * bp = NULL;
	ALLEGRO_FILE * fp = NULL;

	t3f_debug_message("Enter pa_make_unfloat_selection_redo()\n");
	bp = al_create_bitmap(cep->selection.box.width, cep->selection.box.height);
	if(!bp)
	{
		goto fail;
	}
	fp = al_fopen(fn, "wb");
	if(!fp)
	{
		printf("fail: %s\n", fn);
		goto fail;
	}
	pa_write_undo_header(fp, cep, PA_REDO_TYPE_UNFLOAT_SELECTION, "Unfloat Selection");
	al_fwrite32le(fp, cep->current_layer);
	al_fwrite32le(fp, cep->selection.box.start_x);
	al_fwrite32le(fp, cep->selection.box.start_y);
	al_fwrite32le(fp, cep->selection.box.width);
	al_fwrite32le(fp, cep->selection.box.height);
	if(!al_save_bitmap_f(fp, ".png", bp))
	{
		printf("fail: %s\n", fn);
		goto fail;
	}
	al_destroy_bitmap(bp);
	al_fclose(fp);
	t3f_debug_message("Exit pa_make_unfloat_selection_redo()\n");
	return true;

	fail:
	{
		t3f_debug_message("Fail pa_make_unfloat_selection_redo()\n");
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

bool pa_make_float_selection_undo(PA_CANVAS_EDITOR * cep, PA_BOX * box, const char * fn)
{
	ALLEGRO_STATE old_state;
	ALLEGRO_BITMAP * bp = NULL;
	ALLEGRO_FILE * fp = NULL;

	t3f_debug_message("Enter pa_make_float_selection_undo()\n");
	al_store_state(&old_state, ALLEGRO_STATE_NEW_BITMAP_PARAMETERS);
	al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
	bp = al_create_bitmap(box->width, box->height);
	al_restore_state(&old_state);
	if(!bp)
	{
		goto fail;
	}
	pa_render_canvas_to_bitmap(cep->canvas, cep->current_layer, cep->current_layer + 1, box->start_x, box->start_y, box->width, box->height, 0, bp, NULL);
	fp = al_fopen(fn, "wb");
	if(!fp)
	{
		printf("fail: %s\n", fn);
		goto fail;
	}
	pa_write_undo_header(fp, cep, PA_UNDO_TYPE_FLOAT_SELECTION, "Float Selection");
	al_fwrite32le(fp, cep->current_layer);
	al_fwrite32le(fp, box->start_x);
	al_fwrite32le(fp, box->start_y);
	if(!al_save_bitmap_f(fp, ".png", bp))
	{
		printf("fail: %s\n", fn);
		goto fail;
	}
	al_destroy_bitmap(bp);
	al_fclose(fp);
	t3f_debug_message("Exit pa_make_float_selection_undo()\n");
	return true;

	fail:
	{
		t3f_debug_message("Fail pa_make_float_selection_undo()\n");
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

bool pa_make_float_selection_redo(PA_CANVAS_EDITOR * cep, int new_x, int new_y, ALLEGRO_BITMAP * bp, const char * fn)
{
	ALLEGRO_STATE old_state;
	ALLEGRO_FILE * fp = NULL;

	t3f_debug_message("Enter pa_make_float_selection_redo()\n");
	al_store_state(&old_state, ALLEGRO_STATE_NEW_BITMAP_PARAMETERS);
	al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
	fp = al_fopen(fn, "wb");
	if(!fp)
	{
		printf("fail: %s\n", fn);
		goto fail;
	}
	pa_write_undo_header(fp, cep, PA_REDO_TYPE_FLOAT_SELECTION, "Float Selection");
	al_fwrite32le(fp, cep->current_layer);
	al_fwrite32le(fp, new_x);
	al_fwrite32le(fp, new_y);
	al_fwrite32le(fp, cep->selection.box.start_x);
	al_fwrite32le(fp, cep->selection.box.start_y);
	if(!al_save_bitmap_f(fp, ".png", bp))
	{
		printf("fail: %s\n", fn);
		goto fail;
	}
	al_fclose(fp);

	t3f_debug_message("Exit pa_make_float_selection_redo()\n");
	return true;

	fail:
	{
		t3f_debug_message("Fail pa_make_float_selection_redo()\n");
		if(fp)
		{
			al_fclose(fp);
		}
		return false;
	}
}

bool pa_make_flip_selection_undo(PA_CANVAS_EDITOR * cep, bool horizontal, bool vertical, bool multi, const char * fn)
{
	ALLEGRO_FILE * fp = NULL;

	t3f_debug_message("Enter pa_make_flip_selection_undo()\n");
	fp = al_fopen(fn, "wb");
	if(!fp)
	{
		printf("fail: %s\n", fn);
		goto fail;
	}
	pa_write_undo_header(fp, cep, horizontal ? PA_UNDO_TYPE_FLIP_HORIZONTAL : PA_UNDO_TYPE_FLIP_VERTICAL, horizontal ? "Flip Horizontal" : "Flip Vertical");
	al_fwrite32le(fp, cep->selection.box.start_x);
	al_fwrite32le(fp, cep->selection.box.start_y);
	al_fwrite32le(fp, cep->selection.box.width);
	al_fwrite32le(fp, cep->selection.box.height);
	al_fputc(fp, horizontal);
	al_fputc(fp, vertical);
	al_fputc(fp, multi);
	al_fclose(fp);

	t3f_debug_message("Exit pa_make_flip_selection_undo()\n");
	return true;

	fail:
	{
		t3f_debug_message("Fail pa_make_flip_selection_undo()\n");
		if(fp)
		{
			al_fclose(fp);
		}
		return false;
	}
}

bool pa_make_flip_selection_redo(PA_CANVAS_EDITOR * cep, bool horizontal, bool vertical, bool multi, const char * fn)
{
	return pa_make_flip_selection_undo(cep, horizontal, vertical, multi, fn);
}

bool pa_make_turn_selection_undo(PA_CANVAS_EDITOR * cep, int amount, bool multi, const char * fn)
{
	ALLEGRO_FILE * fp = NULL;

	t3f_debug_message("Enter pa_make_turn_selection_undo()\n");
	fp = al_fopen(fn, "wb");
	if(!fp)
	{
		printf("fail: %s\n", fn);
		goto fail;
	}
	pa_write_undo_header(fp, cep, amount > 0 ? PA_UNDO_TYPE_TURN_CLOCKWISE : PA_UNDO_TYPE_TURN_COUNTER_CLOCKWISE, amount > 0 ? "Turn Clockwise" : "Turn Counter-Clockwise");
	al_fwrite32le(fp, cep->selection.box.start_x);
	al_fwrite32le(fp, cep->selection.box.start_y);
	al_fwrite32le(fp, cep->selection.box.width);
	al_fwrite32le(fp, cep->selection.box.height);
	al_fputc(fp, cep->selection.bitmap_stack ? 1 : 0);
	al_fputc(fp, amount);
	al_fputc(fp, multi);
	al_fclose(fp);

	t3f_debug_message("Exit pa_make_turn_selection_undo()\n");
	return true;

	fail:
	{
		t3f_debug_message("Fail pa_make_turn_selection_undo()\n");
		if(fp)
		{
			al_fclose(fp);
		}
		return false;
	}
}

bool pa_make_turn_selection_redo(PA_CANVAS_EDITOR * cep, int amount, bool multi, const char * fn)
{
	return pa_make_turn_selection_undo(cep, amount, multi, fn);
}

static bool save_selection(PA_CANVAS_EDITOR * cep, bool multi, ALLEGRO_FILE * fp)
{
	ALLEGRO_STATE old_state;
	ALLEGRO_BITMAP * bp;
	int i;

	al_fwrite32le(fp, cep->selection.layer);
	al_fwrite32le(fp, cep->selection.box.start_x);
	al_fwrite32le(fp, cep->selection.box.start_y);
	al_fwrite32le(fp, cep->selection.box.width);
	al_fwrite32le(fp, cep->selection.box.height);
	al_fputc(fp, cep->selection.bitmap_stack ? 1 : 0);
	al_fputc(fp, multi);

	/* save the stack if we have one */
	if(cep->selection.bitmap_stack)
	{
		for(i = 0; i < cep->canvas->layer_max; i++)
		{
			if(cep->selection.bitmap_stack->bitmap[i])
			{
				al_fputc(fp, 1);
				al_save_bitmap_f(fp, ".png", cep->selection.bitmap_stack->bitmap[i]);
			}
			else
			{
				al_fputc(fp, 0);
			}
		}
	}

	/* otherwise, save the bitmap data from the canvas */
	else
	{
		al_store_state(&old_state, ALLEGRO_STATE_NEW_BITMAP_PARAMETERS);
		al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
		bp = al_create_bitmap(cep->selection.box.width, cep->selection.box.height);
		al_restore_state(&old_state);
		if(!bp)
		{
			return false;
		}
		if(multi)
		{
			for(i = 0; i < cep->canvas->layer_max; i++)
			{
				pa_render_canvas_to_bitmap(cep->canvas, i, i + 1, cep->selection.box.start_x, cep->selection.box.start_y, cep->selection.box.width, cep->selection.box.height, 0, bp, NULL);
				al_save_bitmap_f(fp, ".png", bp);
			}
		}
		else
		{
			pa_render_canvas_to_bitmap(cep->canvas, cep->selection.layer, cep->selection.layer + 1, cep->selection.box.start_x, cep->selection.box.start_y, cep->selection.box.width, cep->selection.box.height, 0, bp, NULL);
			al_save_bitmap_f(fp, ".png", bp);
		}
		al_destroy_bitmap(bp);
	}
	return true;
}

static bool load_selection(PA_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp)
{
	char undo_path[1024];
	ALLEGRO_BITMAP * bp;
	char b, multi;
	int i;

	cep->selection.layer = al_fread32le(fp);
	cep->selection.box.start_x = al_fread32le(fp);
	cep->selection.box.start_y = al_fread32le(fp);
	cep->selection.box.width = al_fread32le(fp);
	cep->selection.box.height = al_fread32le(fp);
	b = al_fgetc(fp);
	multi = al_fgetc(fp);

	if(pa_make_delete_selection_redo(cep, b, multi, pa_get_undo_path("redo", cep->redo_count, undo_path, 1024)))
	{
		cep->redo_count++;
	}
	if(b)
	{
		pa_free_selection(cep);
		cep->selection.bitmap_stack = pa_create_bitmap_stack(cep->canvas->layer_max, cep->selection.box.width, cep->selection.box.height);
		if(!cep->selection.bitmap_stack)
		{
			return false;
		}
		for(i = 0; i < cep->canvas->layer_max; i++)
		{
			if(al_fgetc(fp))
			{
				cep->selection.bitmap_stack->bitmap[i] = al_load_bitmap_flags_f(fp, ".png", ALLEGRO_NO_PREMULTIPLIED_ALPHA);
			}
		}
		pa_initialize_box(&cep->selection.box, cep->selection.box.start_x, cep->selection.box.start_y, cep->selection.box.width, cep->selection.box.height);
	}
	else
	{
		if(multi)
		{
			for(i = 0; i < cep->canvas->layer_max; i++)
			{
				bp = al_load_bitmap_flags_f(fp, ".png", ALLEGRO_NO_PREMULTIPLIED_ALPHA);
				if(!bp)
				{
					return false;
				}
				pa_import_bitmap_to_canvas(cep->canvas, bp, i, cep->selection.box.start_x, cep->selection.box.start_y);
				al_destroy_bitmap(bp);
			}
		}
		else
		{
			bp = al_load_bitmap_flags_f(fp, ".png", ALLEGRO_NO_PREMULTIPLIED_ALPHA);
			if(!bp)
			{
				return false;
			}
			pa_import_bitmap_to_canvas(cep->canvas, bp, cep->selection.layer, cep->selection.box.start_x, cep->selection.box.start_y);
			al_destroy_bitmap(bp);
		}
	}
	return true;
}

bool pa_make_delete_selection_undo(PA_CANVAS_EDITOR * cep, bool multi, const char * fn)
{
	ALLEGRO_FILE * fp = NULL;

	t3f_debug_message("Enter pa_make_delete_selection_undo()\n");
	fp = al_fopen(fn, "wb");
	if(!fp)
	{
		printf("fail: %s\n", fn);
		return false;
	}
	pa_write_undo_header(fp, cep, PA_UNDO_TYPE_DELETE_SELECTION, "Delete Selection");
	if(!save_selection(cep, multi, fp))
	{
		return false;
	}
	al_fclose(fp);
	t3f_debug_message("Exit pa_make_delete_selection_undo()\n");
	return true;
}

bool pa_make_delete_selection_redo(PA_CANVAS_EDITOR * cep, bool f, bool multi, const char * fn)
{
	ALLEGRO_FILE * fp = NULL;

	t3f_debug_message("Enter pa_make_delete_selection_redo()\n");
	fp = al_fopen(fn, "wb");
	if(!fp)
	{
		printf("fail: %s\n", fn);
		goto fail;
	}
	pa_write_undo_header(fp, cep, PA_UNDO_TYPE_DELETE_SELECTION, "Delete Selection");
	al_fwrite32le(fp, cep->selection.layer);
	al_fwrite32le(fp, cep->selection.box.start_x);
	al_fwrite32le(fp, cep->selection.box.start_y);
	al_fwrite32le(fp, cep->selection.box.width);
	al_fwrite32le(fp, cep->selection.box.height);
	al_fputc(fp, f);
	al_fputc(fp, multi);
	al_fclose(fp);

	t3f_debug_message("Exit pa_make_delete_selection_redo()\n");
	return true;

	fail:
	{
		t3f_debug_message("Fail pa_make_delete_selection_redo()\n");
		if(fp)
		{
			al_fclose(fp);
		}
		return false;
	}
}

bool pa_apply_unfloat_selection_undo(PA_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp, const char * action, bool revert)
{
	char undo_path[1024];
	ALLEGRO_BITMAP * bp = NULL;
	int layer;
	int shift_x = 0;
	int shift_y = 0;
	int i;

	t3f_debug_message("Enter pa_apply_unfloat_selection_undo()\n");
	layer = al_fread32le(fp);
	cep->selection.box.start_x = al_fread32le(fp);
	cep->selection.box.start_y = al_fread32le(fp);
	cep->selection.box.width = al_fread32le(fp);
	cep->selection.box.height = al_fread32le(fp);
	pa_get_canvas_shift(cep->canvas, cep->selection.box.start_x, cep->selection.box.start_y, &shift_x, &shift_y);
	pa_make_unfloat_selection_redo(cep, pa_get_undo_path("redo", cep->redo_count, undo_path, 1024));
	cep->redo_count++;
	cep->selection.layer_max = al_fread32le(fp);
	cep->selection.layer = al_fread32le(fp);
	cep->selection.bitmap_stack = pa_create_bitmap_stack(cep->selection.layer_max, 0, 0);
	if(cep->selection.bitmap_stack)
	{
		for(i = 0; i < cep->selection.layer_max; i++)
		{
			if(al_fgetc(fp))
			{
				bp = al_load_bitmap_flags_f(fp, ".png", ALLEGRO_NO_PREMULTIPLIED_ALPHA);
				if(!bp)
				{
					goto fail;
				}
				pa_import_bitmap_to_canvas(cep->canvas, bp, i, cep->selection.box.start_x + shift_x * cep->canvas->bitmap_size, cep->selection.box.start_y + shift_y * cep->canvas->bitmap_size);
				al_destroy_bitmap(bp);
				bp = al_load_bitmap_flags_f(fp, ".png", ALLEGRO_NO_PREMULTIPLIED_ALPHA);
				if(!bp)
				{
					goto fail;
				}
				cep->selection.bitmap_stack->width = al_get_bitmap_width(bp);
				cep->selection.bitmap_stack->height = al_get_bitmap_height(bp);
				cep->selection.bitmap_stack->bitmap[i] = bp;
			}
		}
	}

	pa_initialize_box(&cep->selection.box, cep->selection.box.start_x + shift_x * cep->canvas->bitmap_size, cep->selection.box.start_y + shift_y * cep->canvas->bitmap_size, cep->selection.box.width, cep->selection.box.height);
	if(shift_x || shift_y)
	{
		pa_shift_canvas_bitmap_array(cep->canvas, -shift_x, -shift_y);
		pa_shift_canvas_editor_variables(cep, -shift_x * cep->canvas->bitmap_size, -shift_y * cep->canvas->bitmap_size);
	}
	pa_update_box_handles(&cep->selection.box, cep->view_x, cep->view_y, cep->view_zoom);
	pa_select_canvas_editor_tool(cep, PA_TOOL_SELECTION);
	t3f_debug_message("Exit pa_apply_unfloat_selection_undo()\n");
	return true;

	fail:
	{
		t3f_debug_message("Fail pa_apply_unfloat_selection_undo()\n");
		if(bp)
		{
			al_destroy_bitmap(bp);
		}
		pa_free_clipboard(cep);
		return false;
	}
}

bool pa_apply_float_selection_undo(PA_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp, const char * action, bool revert)
{
	char undo_path[1024];
	ALLEGRO_BITMAP * bp = NULL;
	int layer;
	int new_x, new_y, new_width, new_height;
	int shift_x, shift_y;

	t3f_debug_message("Enter pa_apply_float_selection_undo()\n");
	layer = al_fread32le(fp);
	new_x = al_fread32le(fp);
	new_y = al_fread32le(fp);
	bp = al_load_bitmap_flags_f(fp, ".png", ALLEGRO_NO_PREMULTIPLIED_ALPHA);
	if(!bp)
	{
		goto fail;
	}
	new_width = al_get_bitmap_width(bp);
	new_height = al_get_bitmap_height(bp);
	pa_get_canvas_shift(cep->canvas, new_x, new_y, &shift_x, &shift_y);
	pa_make_float_selection_redo(cep, new_x, new_y, bp,  pa_get_undo_path("redo", cep->redo_count, undo_path, 1024));
	cep->redo_count++;
	pa_import_bitmap_to_canvas(cep->canvas, bp, layer, new_x, new_y);
	al_destroy_bitmap(bp);
	if(shift_x || shift_y)
	{
		pa_shift_canvas_bitmap_array(cep->canvas, -shift_x, -shift_y);
	}
	pa_free_clipboard(cep);
	pa_free_selection(cep);
	pa_initialize_box(&cep->selection.box, new_x, new_y, new_width, new_height);
	pa_update_box_handles(&cep->selection.box, cep->view_x, cep->view_y, cep->view_zoom);
	t3f_debug_message("Exit pa_apply_float_selection_undo()\n");
	return true;

	fail:
	{
		t3f_debug_message("Fail pa_apply_float_selection_undo()\n");
		if(bp)
		{
			al_destroy_bitmap(bp);
		}
		return false;
	}
}

bool pa_apply_unfloat_selection_redo(PA_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp, const char * action)
{
	char undo_path[1024];
	ALLEGRO_BITMAP * bp = NULL;
	int layer;
	int i;

	t3f_debug_message("Enter pa_apply_unfloat_selection_redo()\n");
	layer = al_fread32le(fp);
	cep->selection.box.start_x = al_fread32le(fp);
	cep->selection.box.start_y = al_fread32le(fp);
	cep->selection.layer_max = al_fread32le(fp);
	cep->selection.layer = al_fread32le(fp);
	cep->selection.bitmap_stack = pa_create_bitmap_stack(cep->selection.layer_max, 0, 0);
	if(cep->selection.bitmap_stack)
	{
		for(i = 0; i < cep->selection.layer_max; i++)
		{
			if(al_fgetc(fp))
			{
				cep->selection.bitmap_stack->bitmap[i] = al_load_bitmap_flags_f(fp, ".png", ALLEGRO_NO_PREMULTIPLIED_ALPHA);
				if(!cep->selection.bitmap_stack->bitmap[i])
				{
					goto fail;
				}
			}
		}
	}
	cep->selection.box.width = al_fread32le(fp);
	cep->selection.box.height = al_fread32le(fp);
	pa_make_unfloat_selection_undo(cep, pa_get_undo_path("undo", cep->undo_count, undo_path, 1024));
	cep->undo_count++;
	pa_shift_canvas_editor_variables(cep, cep->shift_x * cep->canvas->bitmap_size, cep->shift_y * cep->canvas->bitmap_size);
	pa_import_bitmap_to_canvas(cep->canvas, bp, layer, cep->selection.box.start_x, cep->selection.box.start_y);
	al_destroy_bitmap(bp);
	pa_clear_canvas_editor_selection(cep);
	t3f_debug_message("Exit pa_apply_unfloat_selection_redo()\n");
	return true;

	fail:
	{
		t3f_debug_message("Fail pa_apply_unfloat_selection_redo()\n");
		if(bp)
		{
			al_destroy_bitmap(bp);
		}
		pa_free_clipboard(cep);
		return false;
	}
}

bool pa_apply_float_selection_redo(PA_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp, const char * action)
{
	char undo_path[1024];
	int layer;
	int new_x, new_y;
	int i;

	t3f_debug_message("Enter pa_apply_float_selection_redo()\n");
	pa_make_float_selection_undo(cep, &cep->selection.box, pa_get_undo_path("undo", cep->undo_count, undo_path, 1024));
	cep->undo_count++;
	layer = al_fread32le(fp);
	new_x = al_fread32le(fp);
	new_y = al_fread32le(fp);
	cep->selection.box.start_x = al_fread32le(fp);
	cep->selection.box.start_y = al_fread32le(fp);
	cep->selection.layer_max = al_fread32le(fp);
	cep->selection.layer = al_fread32le(fp);
	cep->selection.bitmap_stack = pa_create_bitmap_stack(cep->selection.layer_max, 0, 0);
	if(cep->selection.bitmap_stack)
	{
		for(i = 0; i < cep->selection.layer_max; i++)
		{
			if(al_fgetc(fp))
			{
				cep->selection.bitmap_stack->bitmap[i] = al_load_bitmap_flags_f(fp, ".png", ALLEGRO_NO_PREMULTIPLIED_ALPHA);
				if(!cep->selection.bitmap_stack->bitmap[i])
				{
					goto fail;
				}
			}
		}
	}
	pa_initialize_box(&cep->selection.box, cep->selection.box.start_x, cep->selection.box.start_y, cep->selection.box.width, cep->selection.box.height);
	pa_draw_primitive_to_canvas(cep->canvas, layer, new_x, new_y, new_x + cep->selection.box.width - 1, new_y + cep->selection.box.height - 1, NULL, al_map_rgba_f(0.0, 0.0, 0.0, 0.0), NULL, PA_RENDER_COPY, NULL, pa_draw_filled_rectangle);
	pa_update_box_handles(&cep->selection.box, cep->view_x, cep->view_y, cep->view_zoom);

	t3f_debug_message("Exit pa_apply_float_selection_redo()\n");
	return true;

	fail:
	{
		t3f_debug_message("Fail pa_apply_float_selection_redo()\n");
		pa_free_clipboard(cep);
		return false;
	}
}

bool pa_apply_flip_selection_undo(PA_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp)
{
	char undo_path[1024];
	char horizontal, vertical, multi;
	int x, y, w, h;

	t3f_debug_message("Enter pa_apply_flip_selection_undo()\n");
	x = al_fread32le(fp);
	y = al_fread32le(fp);
	w = al_fread32le(fp);
	h = al_fread32le(fp);
	horizontal = al_fgetc(fp);
	vertical = al_fgetc(fp);
	multi = al_fgetc(fp);
	if(pa_make_flip_selection_redo(cep, horizontal, vertical, multi,  pa_get_undo_path("redo", cep->redo_count, undo_path, 1024)))
	{
		cep->redo_count++;
	}
	pa_initialize_box(&cep->selection.box, x, y, w, h);
	pa_handle_flip_selection(cep, horizontal, vertical, multi, true);
	t3f_debug_message("Exit pa_apply_flip_selection_redo()\n");

	return true;
}

bool pa_apply_flip_selection_redo(PA_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp)
{
	char undo_path[1024];
	char horizontal, vertical, multi;
	int x, y, w, h;

	t3f_debug_message("Enter pa_apply_flip_selection_redo()\n");
	x = al_fread32le(fp);
	y = al_fread32le(fp);
	w = al_fread32le(fp);
	h = al_fread32le(fp);
	horizontal = al_fgetc(fp);
	vertical = al_fgetc(fp);
	multi = al_fgetc(fp);
	if(pa_make_flip_selection_undo(cep, horizontal, vertical, multi,  pa_get_undo_path("undo", cep->undo_count, undo_path, 1024)))
	{
		cep->undo_count++;
	}
	pa_initialize_box(&cep->selection.box, x, y, w, h);
	pa_handle_flip_selection(cep, horizontal, vertical, multi, true);
	t3f_debug_message("Exit pa_apply_flip_selection_redo()\n");

	return true;
}

bool pa_apply_turn_selection_undo(PA_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp)
{
	char undo_path[1024];
	char amount, multi;
	int x, y, w, h, b;

	t3f_debug_message("Enter pa_apply_turn_selection_undo()\n");
	x = al_fread32le(fp);
	y = al_fread32le(fp);
	w = al_fread32le(fp);
	h = al_fread32le(fp);
	b = al_fgetc(fp);
	amount = al_fgetc(fp);
	multi = al_fgetc(fp);
	if(pa_make_turn_selection_redo(cep, amount, multi,  pa_get_undo_path("redo", cep->redo_count, undo_path, 1024)))
	{
		cep->redo_count++;
	}
	pa_initialize_box(&cep->selection.box, x, y, w, h);
	pa_handle_turn_selection(cep, -amount, multi, true);
	if(!b)
	{
		pa_handle_unfloat_canvas_editor_selection(cep, &cep->selection.box, true);
	}
	t3f_debug_message("Exit pa_apply_turn_selection_undo()\n");

	return true;
}

bool pa_apply_turn_selection_redo(PA_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp)
{
	char undo_path[1024];
	char amount, multi;
	int x, y, w, h, b;

	t3f_debug_message("Enter pa_apply_turn_selection_redo()\n");
	x = al_fread32le(fp);
	y = al_fread32le(fp);
	w = al_fread32le(fp);
	h = al_fread32le(fp);
	b = al_fgetc(fp);
	amount = al_fgetc(fp);
	multi = al_fgetc(fp);
	if(pa_make_turn_selection_undo(cep, amount, multi,  pa_get_undo_path("undo", cep->undo_count, undo_path, 1024)))
	{
		cep->undo_count++;
	}
	pa_initialize_box(&cep->selection.box, x, y, w, h);
	pa_handle_turn_selection(cep, amount, multi, true);
	t3f_debug_message("Exit pa_apply_turn_selection_redo()\n");

	return true;
}

bool pa_apply_delete_selection_undo(PA_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp)
{
	t3f_debug_message("Enter pa_apply_delete_selection_undo()\n");
	if(!load_selection(cep, fp))
	{
		return false;
	}
	t3f_debug_message("Exit pa_apply_delete_selection_undo()\n");

	return true;
}

bool pa_apply_delete_selection_redo(PA_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp)
{
	char undo_path[1024];
	char multi;
	int layer, x, y, w, h, b, i;

	t3f_debug_message("Enter pa_apply_delete_selection_redo()\n");
	layer = al_fread32le(fp);
	x = al_fread32le(fp);
	y = al_fread32le(fp);
	w = al_fread32le(fp);
	h = al_fread32le(fp);
	b = al_fgetc(fp);
	multi = al_fgetc(fp);
	if(pa_make_delete_selection_undo(cep, multi,  pa_get_undo_path("undo", cep->undo_count, undo_path, 1024)))
	{
		cep->undo_count++;
	}
	if(b)
	{
		pa_free_selection(cep);
	}
	else
	{
		if(multi)
		{
			for(i = 0; i < cep->canvas->layer_max; i++)
			{
				pa_draw_primitive_to_canvas(cep->canvas, i, x, y, x + w - 1, y + h - 1, NULL, al_map_rgba_f(0.0, 0.0, 0.0, 0.0), NULL, PA_RENDER_COPY, NULL, pa_draw_filled_rectangle);
			}
		}
		else
		{
			pa_draw_primitive_to_canvas(cep->canvas, layer, x, y, x + w - 1, y + h - 1, NULL, al_map_rgba_f(0.0, 0.0, 0.0, 0.0), NULL, PA_RENDER_COPY, NULL, pa_draw_filled_rectangle);
		}
	}
	cep->selection.box.width = 0;
	cep->selection.box.height = 0;
	t3f_debug_message("Exit pa_apply_delete_selection_redo()\n");

	return true;
}
