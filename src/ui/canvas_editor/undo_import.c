#include "t3f/t3f.h"
#include "t3f/file.h"
#include "modules/canvas/canvas.h"
#include "modules/canvas/canvas_helpers.h"
#include "canvas_editor.h"
#include "undo.h"

bool quixel_make_import_undo(QUIXEL_CANVAS_EDITOR * cep, int layer, const char * import_fn, const char * fn)
{
	ALLEGRO_FILE * fp = NULL;

	fp = al_fopen(fn, "wb");
	if(!fp)
	{
		printf("fail: %s\n", fn);
		goto fail;
	}
	quixel_write_undo_header(fp, QUIXEL_UNDO_TYPE_IMPORT_IMAGE, "Import Image");
	al_fwrite32le(fp, layer);
	t3f_save_string_f(fp, import_fn);
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

bool quixel_make_import_redo(QUIXEL_CANVAS_EDITOR * cep, int layer, const char * import_fn, const char * fn)
{
	return quixel_make_import_undo(cep, layer, import_fn, fn);
}

bool quixel_apply_import_undo(QUIXEL_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp)
{
	char undo_path[1024];
	char * import_fn = NULL;

	cep->current_layer = al_fread32le(fp);
	import_fn = t3f_load_string_f(fp);
	if(import_fn)
	{
		quixel_make_import_redo(cep, cep->current_layer, import_fn,  quixel_get_undo_path("redo", cep->redo_count, undo_path, 1024));
		cep->redo_count++;
		free(import_fn);
	}
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
	cep->selection.box.width = 0;
	cep->selection.box.height = 0;
	return true;
}

bool quixel_apply_import_redo(QUIXEL_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp)
{
	char undo_path[1024];
	char * import_fn = NULL;

	cep->current_layer = al_fread32le(fp);
	import_fn = t3f_load_string_f(fp);
	if(import_fn)
	{
		quixel_make_import_undo(cep, cep->current_layer, import_fn,  quixel_get_undo_path("undo", cep->undo_count, undo_path, 1024));
		quixel_import_image(cep, import_fn);
		cep->undo_count++;
		free(import_fn);
	}
	return true;
}
