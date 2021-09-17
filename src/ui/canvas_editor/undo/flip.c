#include "t3f/t3f.h"
#include "modules/canvas/canvas.h"
#include "modules/canvas/canvas_helpers.h"
#include "ui/canvas_editor/canvas_editor.h"
#include "undo.h"

bool pa_make_flip_undo(PA_CANVAS_EDITOR * cep, int type, const char * fn)
{
	ALLEGRO_FILE * fp;
	const char * action_name;

	t3f_debug_message("Enter pa_make_flip_undo()\n");
	switch(type)
	{
		case PA_UNDO_TYPE_FLIP_HORIZONTAL:
		{
			action_name = "Flip Horizontal";
			break;
		}
		case PA_UNDO_TYPE_FLIP_VERTICAL:
		{
			action_name = "Flip Vertical";
			break;
		}
		default:
		{
			action_name = "Flip";
			break;
		}
	}
	fp = al_fopen(fn, "wb");
	if(!fp)
	{
		printf("fail: %s\n", fn);
		goto fail;
	}
	pa_write_undo_header(fp, cep, type, action_name);
	al_fputc(fp, type);
	al_fwrite32le(fp, cep->selection.box.start_x);
	al_fwrite32le(fp, cep->selection.box.start_y);
	al_fwrite32le(fp, cep->selection.box.width);
	al_fwrite32le(fp, cep->selection.box.height);
	al_fclose(fp);
	t3f_debug_message("Exit pa_make_flip_undo()\n");
	return true;

	fail:
	{
		t3f_debug_message("Fail pa_make_flip_undo()\n");
		if(fp)
		{
			al_fclose(fp);
		}
		return false;
	}
}

bool pa_make_flip_redo(PA_CANVAS_EDITOR * cep, int type, const char * fn)
{
	return pa_make_flip_undo(cep, type, fn);
}

bool pa_apply_flip_undo(PA_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp, const char * action, bool revert)
{
	char undo_path[1024];
	int type;

	t3f_debug_message("Enter pa_apply_flip_undo()\n");
	type = al_fgetc(fp);
	cep->selection.box.start_x = al_fread32le(fp);
	cep->selection.box.start_y = al_fread32le(fp);
	cep->selection.box.width = al_fread32le(fp);
	cep->selection.box.height = al_fread32le(fp);
	pa_get_undo_path("redo", cep->redo_count, undo_path, 1024);
	if(pa_make_flip_redo(cep, type, undo_path))
	{
		cep->redo_count++;
	}
	switch(type)
	{
		case PA_UNDO_TYPE_FLIP_HORIZONTAL:
		{
			break;
		}
		case PA_UNDO_TYPE_FLIP_VERTICAL:
		{
			break;
		}
	}
	t3f_debug_message("Exit pa_apply_flip_undo()\n");
	return true;
}

bool pa_apply_flip_redo(PA_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp, const char * action)
{
	char undo_path[1024];
	int type;

	t3f_debug_message("Enter pa_apply_flip_redo()\n");
	type = al_fgetc(fp);
	cep->selection.box.start_x = al_fread32le(fp);
	cep->selection.box.start_y = al_fread32le(fp);
	cep->selection.box.width = al_fread32le(fp);
	cep->selection.box.height = al_fread32le(fp);
	pa_get_undo_path("undo", cep->undo_count, undo_path, 1024);
	if(pa_make_flip_undo(cep, type, undo_path))
	{
		cep->undo_count++;
	}
	switch(type)
	{
		case PA_UNDO_TYPE_FLIP_HORIZONTAL:
		{
			break;
		}
		case PA_UNDO_TYPE_FLIP_VERTICAL:
		{
			break;
		}
	}
	t3f_debug_message("Exit pa_apply_flip_redo()\n");
	return true;
}
