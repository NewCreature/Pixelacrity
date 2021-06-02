#include "t3f/t3f.h"
#include "modules/canvas/canvas.h"
#include "modules/canvas/canvas_helpers.h"
#include "canvas_editor.h"
#include "undo.h"
#include "undo_tool.h"
#include "undo_frame.h"
#include "undo_selection.h"
#include "undo_layer.h"

bool quixel_write_undo_header(ALLEGRO_FILE * fp, int type, const char * name)
{
	al_fputc(fp, type);
	al_fwrite16le(fp, strlen(name));
	al_fwrite(fp, name, strlen(name));

	return true;
}

const char * quixel_get_undo_name(const char * fn, char * out, int out_size)
{
	ALLEGRO_FILE * fp = NULL;
	int l;

	fp = al_fopen(fn, "rb");
	if(!fp)
	{
		goto fail;
	}
	al_fgetc(fp);
	l = al_fread16le(fp);
	if(l >= out_size || l < 1)
	{
		goto fail;
	}
	al_fread(fp, out, l);
	out[l] = 0;
	al_fclose(fp);

	return out;

	fail:
	{
		if(fp)
		{
			al_fclose(fp);
		}
		return NULL;
	}
}

const char * quixel_get_undo_path(const char * base, int count, char * out, int out_size)
{
	char fn[256];

	sprintf(fn, "%s.%04d", base, count);
	return t3f_get_filename(t3f_data_path, fn, out, out_size);
}

void quixel_update_undo_name(QUIXEL_CANVAS_EDITOR * cep)
{
	char undo_path[1024];
	char uname[256];

	quixel_get_undo_path("undo", cep->undo_count - 1, undo_path, 1024);
	if(quixel_get_undo_name(undo_path, uname, 256))
	{
		sprintf(cep->undo_name, "Undo %s", uname);
	}
	else
	{
		sprintf(cep->undo_name, "Undo");
	}
}

void quixel_update_redo_name(QUIXEL_CANVAS_EDITOR * cep)
{
	char redo_path[1024];
	char rname[256];

	quixel_get_undo_path("redo", cep->redo_count - 1, redo_path, 1024);
	if(quixel_get_undo_name(redo_path, rname, 256))
	{
		sprintf(cep->redo_name, "Redo %s", rname);
	}
	else
	{
		sprintf(cep->redo_name, "Redo");
	}
}

void quixel_finalize_undo(QUIXEL_CANVAS_EDITOR * cep)
{
	cep->undo_count++;
	cep->redo_count = 0;
	quixel_update_undo_name(cep);
	quixel_update_redo_name(cep);
	t3f_refresh_menus();
}

static bool apply_undo_type(QUIXEL_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp, int type, const char * action, bool revert)
{
	switch(type)
	{
		case QUIXEL_UNDO_TYPE_TOOL:
		{
			return quixel_apply_tool_undo(cep, fp, action, revert);
		}
		case QUIXEL_UNDO_TYPE_UNFLOAT_SELECTION:
		{
			return quixel_apply_unfloat_selection_undo(cep, fp, action, revert);
		}
		case QUIXEL_UNDO_TYPE_FLOAT_SELECTION:
		{
			return quixel_apply_float_selection_undo(cep, fp, action, revert);
		}
		case QUIXEL_UNDO_TYPE_FRAME:
		{
			return quixel_apply_frame_undo(cep, fp, action, revert);
		}
		case QUIXEL_UNDO_TYPE_ADD_LAYER:
		{
			return quixel_apply_add_layer_undo(cep, fp);
		}
		case QUIXEL_UNDO_TYPE_REMOVE_LAYER:
		{
			return quixel_apply_remove_layer_undo(cep, fp);
		}
	}
	return false;
}

static bool apply_redo_type(QUIXEL_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp, int type, const char * action)
{
	switch(type)
	{
		case QUIXEL_UNDO_TYPE_TOOL:
		{
			return quixel_apply_tool_redo(cep, fp, action);
		}
		case QUIXEL_REDO_TYPE_UNFLOAT_SELECTION:
		{
			return quixel_apply_unfloat_selection_redo(cep, fp, action);
		}
		case QUIXEL_REDO_TYPE_FLOAT_SELECTION:
		{
			return quixel_apply_float_selection_redo(cep, fp, action);
		}
		case QUIXEL_UNDO_TYPE_FRAME:
		{
			return quixel_apply_frame_redo(cep, fp, action);
		}
		case QUIXEL_REDO_TYPE_ADD_LAYER:
		{
			return quixel_apply_add_layer_redo(cep, fp);
		}
		case QUIXEL_REDO_TYPE_REMOVE_LAYER:
		{
			return quixel_apply_remove_layer_redo(cep, fp);
		}
	}
	return false;
}

bool quixel_apply_undo(QUIXEL_CANVAS_EDITOR * cep, const char * fn, bool revert)
{
	ALLEGRO_FILE * fp = NULL;
	int type;
	int l;
	char buf[1024];
	bool ret;

	fp = al_fopen(fn, "rb");
	if(!fp)
	{
		return false;
	}
	type = al_fgetc(fp);
	l = al_fread16le(fp);
	al_fread(fp, buf, l);
	buf[l] = 0;
	ret = apply_undo_type(cep, fp, type, buf, revert);
	if(ret)
	{
		cep->modified--;
		cep->update_title = true;
	}
	al_fclose(fp);
	return true;
}

bool quixel_apply_redo(QUIXEL_CANVAS_EDITOR * cep, const char * fn)
{
	ALLEGRO_FILE * fp = NULL;
	int type;
	int l;
	char buf[1024];
	bool ret;

	fp = al_fopen(fn, "rb");
	if(!fp)
	{
		return false;
	}
	type = al_fgetc(fp);
	l = al_fread16le(fp);
	al_fread(fp, buf, l);
	buf[l] = 0;
	ret = apply_redo_type(cep, fp, type, buf);
	if(ret)
	{
		cep->modified++;
		cep->update_title = true;
	}
	al_fclose(fp);
	return true;
}

void quixel_undo_clean_up(QUIXEL_CANVAS_EDITOR * cep)
{
	char undo_path[1024];
	int i;

	for(i = 0; i < cep->undo_count; i++)
	{
		al_remove_filename(quixel_get_undo_path("undo", i, undo_path, 1024));
	}
	for(i = 0; i < cep->redo_count; i++)
	{
		al_remove_filename(quixel_get_undo_path("redo", i, undo_path, 1024));
	}
}
