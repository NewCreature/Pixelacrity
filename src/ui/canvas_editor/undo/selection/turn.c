#include "t3f/t3f.h"
#include "ui/canvas_editor/canvas_editor.h"
#include "ui/canvas_editor/undo/undo.h"
#include "ui/canvas_editor/selection.h"

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
