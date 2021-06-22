#include "t3f/t3f.h"
#include "t3f/file.h"
#include "modules/canvas/canvas.h"
#include "modules/canvas/canvas_helpers.h"
#include "canvas_editor.h"
#include "undo.h"

bool pa_make_import_undo(PA_CANVAS_EDITOR * cep, int layer, const char * import_fn, const char * fn)
{
	ALLEGRO_FILE * fp = NULL;

	t3f_debug_message("Enter pa_make_import_undo()\n");
	fp = al_fopen(fn, "wb");
	if(!fp)
	{
		printf("fail: %s\n", fn);
		goto fail;
	}
	pa_write_undo_header(fp, PA_UNDO_TYPE_IMPORT_IMAGE, "Import Image");
	al_fwrite32le(fp, layer);
	t3f_save_string_f(fp, import_fn);
	al_fclose(fp);
	t3f_debug_message("Exit pa_make_import_undo()\n");
	return true;

	fail:
	{
		t3f_debug_message("Fail pa_make_import_undo()\n");
		if(fp)
		{
			al_fclose(fp);
		}
		return false;
	}
}

bool pa_make_import_redo(PA_CANVAS_EDITOR * cep, int layer, const char * import_fn, const char * fn)
{
	return pa_make_import_undo(cep, layer, import_fn, fn);
}

bool pa_apply_import_undo(PA_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp)
{
	char undo_path[1024];
	char * import_fn = NULL;

	t3f_debug_message("Enter pa_apply_import_undo()\n");
	cep->current_layer = al_fread32le(fp);
	import_fn = t3f_load_string_f(fp);
	if(import_fn)
	{
		pa_make_import_redo(cep, cep->current_layer, import_fn,  pa_get_undo_path("redo", cep->redo_count, undo_path, 1024));
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
	t3f_debug_message("Exit pa_apply_import_undo()\n");
	return true;
}

bool pa_apply_import_redo(PA_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp)
{
	char undo_path[1024];
	char * import_fn = NULL;

	t3f_debug_message("Enter pa_apply_import_redo()\n");
	cep->current_layer = al_fread32le(fp);
	import_fn = t3f_load_string_f(fp);
	if(import_fn)
	{
		pa_make_import_undo(cep, cep->current_layer, import_fn,  pa_get_undo_path("undo", cep->undo_count, undo_path, 1024));
		pa_import_image(cep, import_fn);
		cep->undo_count++;
		free(import_fn);
	}
	t3f_debug_message("Exit pa_apply_import_redo()\n");
	return true;
}
