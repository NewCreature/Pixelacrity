#include "t3f/t3f.h"
#include "modules/canvas/canvas.h"
#include "modules/canvas/canvas_helpers.h"
#include "canvas_editor.h"
#include "undo.h"

bool quixel_make_frame_undo(QUIXEL_CANVAS_EDITOR * cep, const char * action, const char * fn)
{
	ALLEGRO_FILE * fp = NULL;
	const char * action_name = "Add Frame";
	int i, l;

	fp = al_fopen(fn, "wb");
	if(!fp)
	{
		printf("fail: %s\n", fn);
		goto fail;
	}
	quixel_write_undo_header(fp, QUIXEL_UNDO_TYPE_FRAME, action ? action : action_name);
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

bool quixel_make_frame_redo(QUIXEL_CANVAS_EDITOR * cep, const char * action, const char * fn)
{
	return quixel_make_frame_undo(cep, action, fn);
}

bool quixel_apply_frame_undo(QUIXEL_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp, const char * action, bool revert)
{
	char undo_path[1024];
	int frame_max;
	int i, l;
	int x, y, width, height;
	char frame_name[256] = {0};

	quixel_make_frame_redo(cep, action, quixel_get_undo_path("redo", cep->redo_count, undo_path, 1024));
	cep->redo_count++;
	frame_max = cep->canvas->frame_max;
	for(i = 0; i < frame_max; i++)
	{
		quixel_remove_canvas_frame(cep->canvas, 0);
	}
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
		quixel_add_canvas_frame(cep->canvas, frame_name, x, y, width, height);
	}
	return true;

	fail:
	{
		return false;
	}
}

bool quixel_apply_frame_redo(QUIXEL_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp, const char * action)
{
	char undo_path[1024];
	int frame_max;
	int i, l;
	int x, y, width, height;
	char frame_name[256] = {0};

	quixel_make_frame_undo(cep, action, quixel_get_undo_path("undo", cep->undo_count, undo_path, 1024));
	cep->undo_count++;
	frame_max = cep->canvas->frame_max;
	for(i = 0; i < frame_max; i++)
	{
		quixel_remove_canvas_frame(cep->canvas, 0);
	}
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
		quixel_add_canvas_frame(cep->canvas, frame_name, x, y, width, height);
	}
	return true;

	fail:
	{
		return false;
	}
}
