#include "t3f/t3f.h"
#include "ui/canvas_editor/canvas_editor.h"
#include "modules/canvas/canvas_helpers.h"
#include "selection.h"
#include "ui/canvas_editor/undo/undo.h"
#include "ui/canvas_editor/selection.h"
#include "modules/primitives.h"

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

static bool load_selection(PA_CANVAS_EDITOR * cep, int type, const char * action, ALLEGRO_FILE * fp)
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

	if(pa_make_delete_selection_redo(cep, type, action, b, multi, pa_get_undo_path("redo", cep->redo_count, undo_path, 1024)))
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

bool pa_make_delete_selection_undo(PA_CANVAS_EDITOR * cep, int type, const char * action, bool multi, const char * fn)
{
	ALLEGRO_FILE * fp = NULL;

	t3f_debug_message("Enter pa_make_delete_selection_undo()\n");
	fp = al_fopen(fn, "wb");
	if(!fp)
	{
		printf("fail: %s\n", fn);
		return false;
	}
	pa_write_undo_header(fp, cep, type, action);
	if(!save_selection(cep, multi, fp))
	{
		return false;
	}
	al_fclose(fp);
	t3f_debug_message("Exit pa_make_delete_selection_undo()\n");
	return true;
}

bool pa_make_delete_selection_redo(PA_CANVAS_EDITOR * cep, int type, const char * action, bool f, bool multi, const char * fn)
{
	ALLEGRO_FILE * fp = NULL;

	t3f_debug_message("Enter pa_make_delete_selection_redo()\n");
	fp = al_fopen(fn, "wb");
	if(!fp)
	{
		printf("fail: %s\n", fn);
		goto fail;
	}
	pa_write_undo_header(fp, cep, type, action);
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

bool pa_apply_delete_selection_undo(PA_CANVAS_EDITOR * cep, int type, const char * action, ALLEGRO_FILE * fp)
{
	t3f_debug_message("Enter pa_apply_delete_selection_undo()\n");
	if(!load_selection(cep, type, action, fp))
	{
		return false;
	}
	t3f_debug_message("Exit pa_apply_delete_selection_undo()\n");

	return true;
}

bool pa_apply_delete_selection_redo(PA_CANVAS_EDITOR * cep, int type, const char * action, ALLEGRO_FILE * fp)
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
	if(pa_make_delete_selection_undo(cep, type, action, multi, pa_get_undo_path("undo", cep->undo_count, undo_path, 1024)))
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
