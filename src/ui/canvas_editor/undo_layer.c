#include "t3f/t3f.h"
#include "modules/canvas/canvas.h"
#include "modules/canvas/canvas_helpers.h"
#include "canvas_editor.h"
#include "undo.h"

bool quixel_make_add_layer_undo(QUIXEL_CANVAS_EDITOR * cep, const char * fn)
{
	ALLEGRO_FILE * fp = NULL;

	fp = al_fopen(fn, "wb");
	if(!fp)
	{
		goto fail;
	}
	quixel_write_undo_header(fp, QUIXEL_UNDO_TYPE_ADD_LAYER, "Add Layer");
	al_fclose(fp);
	return true;

	fail:
	{
		if(fp)
		{
			al_fclose(fp);
		}
		return false;
	}
}

bool quixel_make_add_layer_redo(QUIXEL_CANVAS_EDITOR * cep, const char * fn)
{
	ALLEGRO_FILE * fp = NULL;

	fp = al_fopen(fn, "wb");
	if(!fp)
	{
		goto fail;
	}
	quixel_write_undo_header(fp, QUIXEL_REDO_TYPE_ADD_LAYER, "Add Layer");
	al_fclose(fp);
	return true;

	fail:
	{
		if(fp)
		{
			al_fclose(fp);
		}
		return false;
	}
}

bool quixel_make_remove_layer_undo(QUIXEL_CANVAS_EDITOR * cep, int layer, const char * fn)
{
	ALLEGRO_FILE * fp = NULL;
	int x, y, width, height;
	ALLEGRO_BITMAP * bp = NULL;
	ALLEGRO_STATE old_state;

	quixel_get_canvas_dimensions(cep->canvas, &x, &y, &width, &height, 0);
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
		quixel_render_canvas_to_bitmap(cep->canvas, layer, layer + 1, x, y, width, height, 0, bp);
	}
	fp = al_fopen(fn, "wb");
	if(!fp)
	{
		goto fail;
	}
	quixel_write_undo_header(fp, QUIXEL_UNDO_TYPE_REMOVE_LAYER, "Remove Layer");
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
	return true;

	fail:
	{
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

bool quixel_make_remove_layer_redo(QUIXEL_CANVAS_EDITOR * cep, int layer, const char * fn)
{
	ALLEGRO_FILE * fp = NULL;

	fp = al_fopen(fn, "wb");
	if(!fp)
	{
		goto fail;
	}
	quixel_write_undo_header(fp, QUIXEL_REDO_TYPE_REMOVE_LAYER, "Remove Layer");
	al_fwrite16le(fp, layer);
	al_fclose(fp);

	return true;

	fail:
	{
		if(fp)
		{
			al_fclose(fp);
		}
		return false;
	}
}

bool quixel_apply_add_layer_undo(QUIXEL_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp)
{
	char undo_path[1024];

	quixel_make_add_layer_redo(cep, quixel_get_undo_path("redo", cep->redo_count, undo_path, 1024));
	cep->redo_count++;
	quixel_remove_canvas_layer(cep->canvas, cep->canvas->layer_max - 1);
	if(cep->current_layer >= cep->canvas->layer_max)
	{
		cep->current_layer = cep->canvas->layer_max - 1;
	}

	return true;
}

bool quixel_apply_add_layer_redo(QUIXEL_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp)
{
	char undo_path[1024];

	quixel_make_add_layer_undo(cep, quixel_get_undo_path("undo", cep->undo_count, undo_path, 1024));
	cep->undo_count++;
	quixel_add_canvas_layer(cep->canvas, -1);
	cep->current_layer = cep->canvas->layer_max - 1;

	return true;
}

bool quixel_apply_remove_layer_undo(QUIXEL_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp)
{
	int x, y, layer, l;
	ALLEGRO_BITMAP * bp = NULL;
	char undo_path[1024];

	layer = al_fread16le(fp);
	quixel_make_remove_layer_redo(cep, layer, quixel_get_undo_path("redo", cep->redo_count, undo_path, 1024));
	cep->redo_count++;
	quixel_add_canvas_layer(cep->canvas, layer);
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
		quixel_import_bitmap_to_canvas(cep->canvas, bp, layer, x, y);
		al_destroy_bitmap(bp);
	}
	cep->current_layer = layer;
	return true;

	fail:
	{
		if(bp)
		{
			al_destroy_bitmap(bp);
		}
		return false;
	}
}

bool quixel_apply_remove_layer_redo(QUIXEL_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp)
{
	int layer;
	char undo_path[1024];

	layer = al_fread16le(fp);
	quixel_make_remove_layer_undo(cep, layer, quixel_get_undo_path("undo", cep->undo_count, undo_path, 1024));
	cep->undo_count++;
	quixel_remove_canvas_layer(cep->canvas, layer);
	if(cep->current_layer >= cep->canvas->layer_max)
	{
		cep->current_layer = cep->canvas->layer_max - 1;
	}

	return true;
}
