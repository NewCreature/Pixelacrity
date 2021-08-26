#include "t3f/t3f.h"
#include "modules/canvas/canvas.h"
#include "modules/canvas/canvas_helpers.h"
#include "canvas_editor.h"
#include "undo.h"

bool pa_make_add_layer_undo(PA_CANVAS_EDITOR * cep, const char * fn)
{
	ALLEGRO_FILE * fp = NULL;

	t3f_debug_message("Enter pa_make_add_layer_undo()\n");
	fp = al_fopen(fn, "wb");
	if(!fp)
	{
		goto fail;
	}
	pa_write_undo_header(fp, cep, PA_UNDO_TYPE_ADD_LAYER, "Add Layer");
	al_fclose(fp);
	t3f_debug_message("Exit pa_make_add_layer_undo()\n");
	return true;

	fail:
	{
		t3f_debug_message("Fail pa_make_add_layer_undo()\n");
		if(fp)
		{
			al_fclose(fp);
		}
		return false;
	}
}

bool pa_make_add_layer_redo(PA_CANVAS_EDITOR * cep, const char * fn)
{
	ALLEGRO_FILE * fp = NULL;

	t3f_debug_message("Enter pa_make_add_layer_redo()\n");
	fp = al_fopen(fn, "wb");
	if(!fp)
	{
		goto fail;
	}
	pa_write_undo_header(fp, cep, PA_REDO_TYPE_ADD_LAYER, "Add Layer");
	al_fclose(fp);
	t3f_debug_message("Exit pa_make_add_layer_redo()\n");
	return true;

	fail:
	{
		t3f_debug_message("Fail pa_make_add_layer_redo()\n");
		if(fp)
		{
			al_fclose(fp);
		}
		return false;
	}
}

bool pa_make_remove_layer_undo(PA_CANVAS_EDITOR * cep, int layer, const char * fn)
{
	ALLEGRO_FILE * fp = NULL;
	int x, y, width, height;
	ALLEGRO_BITMAP * bp = NULL;
	ALLEGRO_STATE old_state;

	t3f_debug_message("Enter pa_make_remove_layer_undo()\n");
	pa_get_canvas_dimensions(cep->canvas, &x, &y, &width, &height, 0, false);
	if(width > 0 && height > 0)
	{
		al_store_state(&old_state, ALLEGRO_STATE_NEW_BITMAP_PARAMETERS);
		al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
		bp = al_create_bitmap(width, height);
		al_restore_state(&old_state);
		if(!bp)
		{
			goto fail;
		}
		pa_render_canvas_to_bitmap(cep->canvas, layer, layer + 1, x, y, width, height, 0, bp);
	}
	fp = al_fopen(fn, "wb");
	if(!fp)
	{
		goto fail;
	}
	pa_write_undo_header(fp, cep, PA_UNDO_TYPE_REMOVE_LAYER, "Remove Layer");
	al_fwrite16le(fp, layer);
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
	if(bp)
	{
		al_destroy_bitmap(bp);
	}
	al_fclose(fp);
	t3f_debug_message("Exit pa_make_remove_layer_undo()\n");
	return true;

	fail:
	{
		t3f_debug_message("Fail pa_make_remove_layer_undo()\n");
		if(bp)
		{
			al_destroy_bitmap(bp);
		}
		if(fp)
		{
			al_fclose(fp);
		}
		return false;
	}
}

bool pa_make_remove_layer_redo(PA_CANVAS_EDITOR * cep, int layer, const char * fn)
{
	ALLEGRO_FILE * fp = NULL;

	t3f_debug_message("Enter pa_make_remove_layer_redo()\n");
	fp = al_fopen(fn, "wb");
	if(!fp)
	{
		goto fail;
	}
	pa_write_undo_header(fp, cep, PA_REDO_TYPE_REMOVE_LAYER, "Remove Layer");
	al_fwrite16le(fp, layer);
	al_fclose(fp);

	t3f_debug_message("Exit pa_make_remove_layer_redo()\n");
	return true;

	fail:
	{
		t3f_debug_message("Fail pa_make_remove_layer_redo()\n");
		if(fp)
		{
			al_fclose(fp);
		}
		return false;
	}
}

bool pa_apply_add_layer_undo(PA_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp)
{
	char undo_path[1024];

	t3f_debug_message("Enter pa_apply_add_layer_undo()\n");
	pa_make_add_layer_redo(cep, pa_get_undo_path("redo", cep->redo_count, undo_path, 1024));
	cep->redo_count++;
	pa_remove_canvas_layer(cep->canvas, cep->canvas->layer_max - 1);
	if(cep->current_layer >= cep->canvas->layer_max)
	{
		pa_select_canvas_editor_layer(cep, cep->canvas->layer_max - 1);
	}
	t3f_debug_message("Exit pa_apply_add_layer_undo()\n");

	return true;
}

bool pa_apply_add_layer_redo(PA_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp)
{
	char undo_path[1024];

	t3f_debug_message("Enter pa_apply_add_layer_redo()\n");
	pa_make_add_layer_undo(cep, pa_get_undo_path("undo", cep->undo_count, undo_path, 1024));
	cep->undo_count++;
	pa_add_canvas_layer(cep->canvas, -1);
	pa_select_canvas_editor_layer(cep, cep->canvas->layer_max - 1);
	t3f_debug_message("Exit pa_apply_add_layer_redo()\n");

	return true;
}

bool pa_apply_remove_layer_undo(PA_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp)
{
	int x, y, layer, l;
	ALLEGRO_BITMAP * bp = NULL;
	char undo_path[1024];

	t3f_debug_message("Enter pa_apply_remove_layer_undo()\n");
	layer = al_fread16le(fp);
	pa_make_remove_layer_redo(cep, layer, pa_get_undo_path("redo", cep->redo_count, undo_path, 1024));
	cep->redo_count++;
	pa_add_canvas_layer(cep->canvas, layer);
	l = al_fgetc(fp);
	if(l)
	{
		x = al_fread32le(fp);
		y = al_fread32le(fp);
		bp = al_load_bitmap_flags_f(fp, ".png", ALLEGRO_NO_PREMULTIPLIED_ALPHA);
		if(!bp)
		{
			goto fail;
		}
		pa_import_bitmap_to_canvas(cep->canvas, bp, layer, x, y);
		al_destroy_bitmap(bp);
	}
	pa_select_canvas_editor_layer(cep, layer);
	t3f_debug_message("Exit pa_apply_remove_layer_undo()\n");
	return true;

	fail:
	{
		t3f_debug_message("Fail pa_apply_remove_layer_undo()\n");
		if(bp)
		{
			al_destroy_bitmap(bp);
		}
		return false;
	}
}

bool pa_apply_remove_layer_redo(PA_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp)
{
	int layer;
	char undo_path[1024];

	t3f_debug_message("Enter pa_apply_remove_layer_redo()\n");
	layer = al_fread16le(fp);
	pa_make_remove_layer_undo(cep, layer, pa_get_undo_path("undo", cep->undo_count, undo_path, 1024));
	cep->undo_count++;
	pa_remove_canvas_layer(cep->canvas, layer);
	if(cep->current_layer >= cep->canvas->layer_max)
	{
		pa_select_canvas_editor_layer(cep, cep->canvas->layer_max - 1);
	}

	t3f_debug_message("Exit pa_apply_remove_layer_redo()\n");
	return true;
}
