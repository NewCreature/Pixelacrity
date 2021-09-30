#include "t3f/t3f.h"
#include "ui/canvas_editor/canvas_editor.h"
#include "modules/canvas/canvas_helpers.h"
#include "ui/canvas_editor/undo/undo.h"
#include "ui/canvas_editor/clipboard.h"
#include "ui/canvas_editor/selection.h"
#include "modules/primitives.h"

bool pa_make_float_selection_undo(PA_CANVAS_EDITOR * cep, PA_BOX * box, bool multi, const char * fn)
{
	ALLEGRO_STATE old_state;
	ALLEGRO_BITMAP * bp = NULL;
	ALLEGRO_FILE * fp = NULL;
	int i;

	t3f_debug_message("Enter pa_make_float_selection_undo()\n");
	al_store_state(&old_state, ALLEGRO_STATE_NEW_BITMAP_PARAMETERS);
	al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
	bp = al_create_bitmap(box->width, box->height);
	al_restore_state(&old_state);
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
	pa_write_undo_header(fp, cep, PA_UNDO_TYPE_FLOAT_SELECTION, "Float Selection");
	al_fputc(fp, multi);
	al_fwrite32le(fp, cep->current_layer);
	al_fwrite32le(fp, box->start_x);
	al_fwrite32le(fp, box->start_y);
	if(multi)
	{
		for(i = 0; i < cep->canvas->layer_max; i++)
		{
			pa_render_canvas_to_bitmap(cep->canvas, i, i + 1, box->start_x, box->start_y, box->width, box->height, 0, bp, NULL);
			if(!al_save_bitmap_f(fp, ".png", bp))
			{
				printf("fail: %s\n", fn);
				goto fail;
			}
		}
	}
	else
	{
		pa_render_canvas_to_bitmap(cep->canvas, cep->current_layer, cep->current_layer + 1, box->start_x, box->start_y, box->width, box->height, 0, bp, NULL);
		if(!al_save_bitmap_f(fp, ".png", bp))
		{
			printf("fail: %s\n", fn);
			goto fail;
		}
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

bool pa_apply_float_selection_undo(PA_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp, const char * action, bool revert)
{
	char undo_path[1024];
	ALLEGRO_BITMAP * bp = NULL;
	int layer;
	int new_x, new_y, new_width, new_height;
	int shift_x, shift_y;
	char multi;
	int i;

	t3f_debug_message("Enter pa_apply_float_selection_undo()\n");
	multi = al_fgetc(fp);
	layer = al_fread32le(fp);
	new_x = al_fread32le(fp);
	new_y = al_fread32le(fp);
	pa_get_canvas_shift(cep->canvas, new_x, new_y, &shift_x, &shift_y);
	if(multi)
	{
		for(i = 0; i < cep->canvas->layer_max; i++)
		{
			bp = al_load_bitmap_flags_f(fp, ".png", ALLEGRO_NO_PREMULTIPLIED_ALPHA);
			if(!bp)
			{
				goto fail;
			}
			new_width = al_get_bitmap_width(bp);
			new_height = al_get_bitmap_height(bp);
			pa_import_bitmap_to_canvas(cep->canvas, bp, i, new_x, new_y);
			al_destroy_bitmap(bp);
		}
	}
	else
	{
		bp = al_load_bitmap_flags_f(fp, ".png", ALLEGRO_NO_PREMULTIPLIED_ALPHA);
		if(!bp)
		{
			goto fail;
		}
		new_width = al_get_bitmap_width(bp);
		new_height = al_get_bitmap_height(bp);
		pa_import_bitmap_to_canvas(cep->canvas, bp, layer, new_x, new_y);
		al_destroy_bitmap(bp);
	}
//	pa_make_float_selection_redo(cep, new_x, new_y, bp, pa_get_undo_path("redo", cep->redo_count, undo_path, 1024));
//	cep->redo_count++;
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

bool pa_apply_float_selection_redo(PA_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp, const char * action)
{
	char undo_path[1024];
	int layer;
	int new_x, new_y;
	int i;
	char multi;

	t3f_debug_message("Enter pa_apply_float_selection_redo()\n");
	multi = al_fgetc(fp);
	layer = al_fread32le(fp);
	new_x = al_fread32le(fp);
	new_y = al_fread32le(fp);
	pa_make_float_selection_undo(cep, &cep->selection.box, multi,  pa_get_undo_path("undo", cep->undo_count, undo_path, 1024));
	cep->undo_count++;
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
