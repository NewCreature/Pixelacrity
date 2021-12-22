#include "t3f/t3f.h"
#include "modules/canvas/canvas.h"
#include "modules/canvas/canvas_helpers.h"
#include "ui/canvas_editor/canvas_editor.h"
#include "undo.h"

bool pa_make_frame_undo(PA_CANVAS_EDITOR * cep, const char * action, const char * fn)
{
	ALLEGRO_FILE * fp = NULL;
	const char * action_name = "Add Frame";
	int i, l;

	t3f_debug_message("Enter pa_make_frame_undo()\n");
	fp = al_fopen(fn, "wb");
	if(!fp)
	{
		printf("fail: %s\n", fn);
		goto fail;
	}
	pa_write_undo_header(fp, cep, PA_UNDO_TYPE_FRAME, action ? action : action_name);
	al_fwrite32le(fp, cep->current_frame);
	al_fwrite32le(fp, cep->canvas->frame_max);
	for(i = 0; i < cep->canvas->frame_max; i++)
	{
		l = strlen(cep->canvas->frame[i]->name);
		al_fwrite16le(fp, l);
		al_fwrite(fp, cep->canvas->frame[i]->name, l);
		al_fwrite32le(fp, cep->canvas->frame[i]->box.start_x);
		al_fwrite32le(fp, cep->canvas->frame[i]->box.start_y);
		al_fwrite32le(fp, cep->canvas->frame[i]->box.width);
		al_fwrite32le(fp, cep->canvas->frame[i]->box.height);
	}
	al_fclose(fp);
	t3f_debug_message("Exit pa_make_frame_undo()\n");
	return true;

	fail:
	{
		t3f_debug_message("Fail pa_make_frame_undo()\n");
		if(fp)
		{
			al_fclose(fp);
		}
		return false;
	}
}

bool pa_make_frame_redo(PA_CANVAS_EDITOR * cep, const char * action, const char * fn)
{
	return pa_make_frame_undo(cep, action, fn);
}

bool pa_apply_frame_undo(PA_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp, const char * action, bool revert)
{
	char undo_path[1024];
	int frame_max;
	int i, l;
	int x, y, width, height;
	char frame_name[256] = {0};

	t3f_debug_message("Enter pa_apply_frame_undo()\n");
	pa_make_frame_redo(cep, action, pa_get_undo_path("redo", cep->redo_count, undo_path, 1024));
	cep->redo_count++;
	frame_max = cep->canvas->frame_max;
	for(i = 0; i < frame_max; i++)
	{
		pa_remove_canvas_frame(cep->canvas, 0);
	}
	cep->current_frame = al_fread32le(fp);
	frame_max = al_fread32le(fp);
	for(i = 0; i < frame_max; i++)
	{
		l = al_fread16le(fp);
		al_fread(fp, frame_name, l);
		if(l >= 256)
		{
			goto fail;
		}
		frame_name[l] = 0;
		x = al_fread32le(fp);
		y = al_fread32le(fp);
		width = al_fread32le(fp);
		height = al_fread32le(fp);
		pa_add_canvas_frame(cep->canvas, frame_name, x, y, width, height);
	}
	pa_reload_canvas_editor_state(cep);
	t3f_debug_message("Exit pa_apply_frame_undo()\n");
	return true;

	fail:
	{
		t3f_debug_message("Fail pa_apply_frame_undo()\n");
		return false;
	}
}

bool pa_apply_frame_redo(PA_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp, const char * action)
{
	char undo_path[1024];
	int frame_max;
	int i, l;
	int x, y, width, height;
	char frame_name[256] = {0};

	t3f_debug_message("Enter pa_apply_frame_redo()\n");
	pa_make_frame_undo(cep, action, pa_get_undo_path("undo", cep->undo_count, undo_path, 1024));
	cep->undo_count++;
	frame_max = cep->canvas->frame_max;
	for(i = 0; i < frame_max; i++)
	{
		pa_remove_canvas_frame(cep->canvas, 0);
	}
	cep->current_frame = al_fread32le(fp);
	frame_max = al_fread32le(fp);
	for(i = 0; i < frame_max; i++)
	{
		l = al_fread16le(fp);
		al_fread(fp, frame_name, l);
		if(l >= 256)
		{
			goto fail;
		}
		frame_name[l] = 0;
		x = al_fread32le(fp);
		y = al_fread32le(fp);
		width = al_fread32le(fp);
		height = al_fread32le(fp);
		pa_add_canvas_frame(cep->canvas, frame_name, x, y, width, height);
	}
	pa_reload_canvas_editor_state(cep);
	t3f_debug_message("Exit pa_apply_frame_redo()\n");
	return true;

	fail:
	{
		t3f_debug_message("Fail pa_apply_frame_redo()\n");
		return false;
	}
}
