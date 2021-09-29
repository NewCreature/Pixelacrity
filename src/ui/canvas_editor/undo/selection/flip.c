#include "t3f/t3f.h"
#include "ui/canvas_editor/canvas_editor.h"
#include "ui/canvas_editor/undo/undo.h"
#include "ui/canvas_editor/selection.h"

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
