#include "t3f/t3f.h"
#include "ui/canvas_editor/canvas_editor.h"
#include "ui/canvas_editor/undo/undo.h"
#include "modules/canvas/canvas_helpers.h"
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
	ALLEGRO_FILE * fp = NULL;

	t3f_debug_message("Enter pa_make_unfloat_selection_redo()\n");
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
	if(pa_make_unfloat_selection_redo(cep, pa_get_undo_path("redo", cep->redo_count, undo_path, 1024)))
	{
		cep->redo_count++;
	}
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

bool pa_apply_unfloat_selection_redo(PA_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp, const char * action)
{
	char undo_path[1024];
	int layer;
	int i;

	t3f_debug_message("Enter pa_apply_unfloat_selection_redo()\n");
	layer = al_fread32le(fp);
	cep->selection.box.start_x = al_fread32le(fp);
	cep->selection.box.start_y = al_fread32le(fp);
	cep->selection.box.width = al_fread32le(fp);
	cep->selection.box.height = al_fread32le(fp);
	if(pa_make_unfloat_selection_undo(cep, pa_get_undo_path("undo", cep->undo_count, undo_path, 1024)))
	{
		cep->undo_count++;
	}
	pa_handle_unfloat_canvas_editor_selection(cep, &cep->selection.box, true);
	pa_shift_canvas_editor_variables(cep, cep->shift_x * cep->canvas->bitmap_size, cep->shift_y * cep->canvas->bitmap_size);
	pa_clear_canvas_editor_selection(cep);
	t3f_debug_message("Exit pa_apply_unfloat_selection_redo()\n");
	return true;

	fail:
	{
		t3f_debug_message("Fail pa_apply_unfloat_selection_redo()\n");
		pa_free_selection(cep);
		return false;
	}
}
