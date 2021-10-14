#include "t3f/t3f.h"
#include "ui/canvas_editor/canvas_editor.h"
#include "modules/canvas/canvas_helpers.h"
#include "ui/canvas_editor/undo/undo.h"
#include "ui/canvas_editor/clipboard.h"
#include "ui/canvas_editor/selection.h"
#include "modules/primitives.h"

static bool write_bitmap(ALLEGRO_FILE * fp, ALLEGRO_BITMAP * bp)
{
	if(bp)
	{
		al_fputc(fp, 1);
		al_save_bitmap_f(fp, ".png", bp);
	}
	else
	{
		al_fputc(fp, 0);
	}
	return true;
}

static ALLEGRO_BITMAP * read_bitmap(ALLEGRO_FILE * fp)
{
	if(al_fgetc(fp))
	{
		return al_load_bitmap_flags_f(fp, ".png", ALLEGRO_NO_PREMULTIPLIED_ALPHA);
	}
	return NULL;
}

bool pa_make_paste_undo(PA_CANVAS_EDITOR * cep, int pos, int x, int y, bool merge, const char * fn)
{
	ALLEGRO_FILE * fp = NULL;
	int i;

	t3f_debug_message("Enter pa_make_paste_undo()\n");
	fp = al_fopen(fn, "wb");
	if(!fp)
	{
		printf("fail: %s\n", fn);
		goto fail;
	}
	pa_write_undo_header(fp, cep, PA_UNDO_TYPE_PASTE, "Paste");
	al_fwrite32le(fp, pos);
	al_fwrite32le(fp, x);
	al_fwrite32le(fp, y);
	al_fputc(fp, merge);
	al_fwrite32le(fp, cep->clipboard.layer);
	al_fwrite32le(fp, cep->clipboard.bitmap_stack->width);
	al_fwrite32le(fp, cep->clipboard.bitmap_stack->height);
	if(cep->clipboard.layer < 0)
	{
		for(i = 0; i < cep->clipboard.layer_max; i++)
		{
			write_bitmap(fp, cep->clipboard.bitmap_stack->bitmap[i]);
		}
	}
	else
	{
		write_bitmap(fp, cep->clipboard.bitmap_stack->bitmap[cep->clipboard.layer]);
	}
	al_fclose(fp);

	t3f_debug_message("Exit pa_make_paste_undo()\n");
	return true;

	fail:
	{
		t3f_debug_message("Fail pa_make_paste_undo()\n");
		if(fp)
		{
			al_fclose(fp);
		}
		return false;
	}
}

bool pa_make_paste_redo(PA_CANVAS_EDITOR * cep, int pos, int x, int y, bool merge, const char * fn)
{
	ALLEGRO_FILE * fp = NULL;
	int i;

	t3f_debug_message("Enter pa_make_paste_redo()\n");
	fp = al_fopen(fn, "wb");
	if(!fp)
	{
		printf("fail: %s\n", fn);
		goto fail;
	}
	pa_write_undo_header(fp, cep, PA_UNDO_TYPE_PASTE, "Paste");
	al_fwrite32le(fp, pos);
	al_fwrite32le(fp, x);
	al_fwrite32le(fp, y);
	al_fputc(fp, merge);
	al_fwrite32le(fp, cep->selection.layer);
	al_fwrite32le(fp, cep->selection.bitmap_stack->width);
	al_fwrite32le(fp, cep->selection.bitmap_stack->height);
	if(cep->selection.layer < 0)
	{
		for(i = 0; i < cep->selection.layer_max; i++)
		{
			write_bitmap(fp, cep->selection.bitmap_stack->bitmap[i]);
		}
	}
	else
	{
		write_bitmap(fp, cep->selection.bitmap_stack->bitmap[cep->selection.layer]);
	}
	al_fclose(fp);

	t3f_debug_message("Exit pa_make_paste_redo()\n");
	return true;

	fail:
	{
		t3f_debug_message("Fail pa_make_paste_redo()\n");
		if(fp)
		{
			al_fclose(fp);
		}
		return false;
	}
}

bool pa_apply_paste_undo(PA_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp)
{
	char undo_path[1024];
	int pos, ox, oy, layer, width, height;
	int i;
	char merge;

	t3f_debug_message("Enter pa_apply_paste_undo()\n");
	pos = al_fread32le(fp);
	ox = al_fread32le(fp);
	oy = al_fread32le(fp);
	merge = al_fgetc(fp);
	layer = al_fread32le(fp);
	width = al_fread32le(fp);
	height = al_fread32le(fp);
	if(pa_make_paste_redo(cep, 3, cep->selection.box.start_x, cep->selection.box.start_y, merge, pa_get_undo_path("redo", cep->redo_count, undo_path, 1024)))
	{
		cep->redo_count++;
	}
	pa_free_clipboard(cep);
	cep->clipboard.bitmap_stack = pa_create_bitmap_stack(cep->canvas->layer_max, width, height);
	if(cep->clipboard.bitmap_stack)
	{
		if(layer < 0)
		{
			for(i = 0; i < cep->canvas->layer_max; i++)
			{
				cep->clipboard.bitmap_stack->bitmap[i] = read_bitmap(fp);
			}
		}
		else
		{
			cep->clipboard.bitmap_stack->bitmap[layer] = read_bitmap(fp);
		}
	}
	pa_free_selection(cep);
	cep->selection.box.width = 0;
	cep->selection.box.height = 0;
	t3f_debug_message("Exit pa_apply_paste_redo()\n");

	return true;
}

bool pa_apply_paste_redo(PA_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp)
{
	char undo_path[1024];
	int pos, ox, oy;
	int layer, width, height;
	int i;
	char merge;

	t3f_debug_message("Enter pa_apply_paste_redo()\n");
	pos = al_fread32le(fp);
	ox = al_fread32le(fp);
	oy = al_fread32le(fp);
	merge = al_fgetc(fp);
	layer = al_fread32le(fp);
	width = al_fread32le(fp);
	height = al_fread32le(fp);
	pa_free_clipboard(cep);
	cep->clipboard.bitmap_stack = pa_create_bitmap_stack(cep->canvas->layer_max, width, height);
	if(cep->clipboard.bitmap_stack)
	{
		if(layer < 0)
		{
			for(i = 0; i < cep->canvas->layer_max; i++)
			{
				cep->clipboard.bitmap_stack->bitmap[i] = read_bitmap(fp);
			}
		}
		else
		{
			cep->clipboard.bitmap_stack->bitmap[layer] = read_bitmap(fp);
		}
	}
	if(pa_make_paste_undo(cep, 1, ox, oy, merge, pa_get_undo_path("undo", cep->undo_count, undo_path, 1024)))
	{
		cep->undo_count++;
	}
	pa_free_selection(cep);
	pa_apply_paste_clipboard(cep, pos, ox, oy, merge);
	t3f_debug_message("Exit pa_apply_paste_undo()\n");

	return true;
}
