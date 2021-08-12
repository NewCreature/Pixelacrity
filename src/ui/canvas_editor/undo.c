#include "t3f/t3f.h"
#include "modules/canvas/canvas.h"
#include "modules/canvas/canvas_helpers.h"
#include "canvas_editor.h"
#include "undo.h"
#include "undo_tool.h"
#include "undo_frame.h"
#include "undo_selection.h"
#include "undo_layer.h"
#include "undo_flood_fill.h"
#include "undo_import.h"
#include "ui/window.h"

bool pa_write_undo_header(ALLEGRO_FILE * fp, int type, const char * name)
{
	t3f_debug_message("Write undo (%d): %s\n", type, name);
	al_fputc(fp, type);
	al_fwrite16le(fp, strlen(name));
	al_fwrite(fp, name, strlen(name));

	return true;
}

const char * pa_get_undo_name(const char * fn, char * out, int out_size)
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

const char * pa_get_undo_path(const char * base, int count, char * out, int out_size)
{
	char fn[256];

	sprintf(fn, "%s.%04d", base, count);
	return t3f_get_filename(t3f_data_path, fn, out, out_size);
}

void pa_update_undo_name(PA_CANVAS_EDITOR * cep)
{
	char undo_path[1024];
	char uname[256];

	pa_get_undo_path("undo", cep->undo_count - 1, undo_path, 1024);
	if(pa_get_undo_name(undo_path, uname, 256))
	{
		sprintf(cep->undo_name, "Undo %s", uname);
	}
	else
	{
		sprintf(cep->undo_name, "Undo");
	}
}

void pa_update_redo_name(PA_CANVAS_EDITOR * cep)
{
	char redo_path[1024];
	char rname[256];

	pa_get_undo_path("redo", cep->redo_count - 1, redo_path, 1024);
	if(pa_get_undo_name(redo_path, rname, 256))
	{
		sprintf(cep->redo_name, "Redo %s", rname);
	}
	else
	{
		sprintf(cep->redo_name, "Redo");
	}
}

void pa_finalize_undo(PA_CANVAS_EDITOR * cep)
{
	t3f_debug_message("Enter pa_finalize_undo()\n");
	cep->undo_count++;
	cep->redo_count = 0;
	pa_update_undo_name(cep);
	pa_update_redo_name(cep);
	t3f_refresh_menus();
	t3f_debug_message("Exit pa_finalize_undo()\n");
}

static bool apply_undo_type(PA_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp, int type, const char * action, bool revert)
{
	t3f_debug_message("Enter apply_undo_type()\n");
	switch(type)
	{
		case PA_UNDO_TYPE_TOOL:
		{
			return pa_apply_tool_undo(cep, fp, action, revert);
		}
		case PA_UNDO_TYPE_UNFLOAT_SELECTION:
		{
			return pa_apply_unfloat_selection_undo(cep, fp, action, revert);
		}
		case PA_UNDO_TYPE_FLOAT_SELECTION:
		{
			return pa_apply_float_selection_undo(cep, fp, action, revert);
		}
		case PA_UNDO_TYPE_FRAME:
		{
			return pa_apply_frame_undo(cep, fp, action, revert);
		}
		case PA_UNDO_TYPE_ADD_LAYER:
		{
			return pa_apply_add_layer_undo(cep, fp);
		}
		case PA_UNDO_TYPE_REMOVE_LAYER:
		{
			return pa_apply_remove_layer_undo(cep, fp);
		}
		case PA_UNDO_TYPE_FLOOD_FILL:
		{
			return pa_apply_flood_fill_undo(cep, fp);
		}
		case PA_UNDO_TYPE_IMPORT_IMAGE:
		{
			return pa_apply_import_undo(cep, fp);
		}
	}
	t3f_debug_message("Exit apply_undo_type()\n");
	return false;
}

static bool apply_redo_type(PA_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp, int type, const char * action)
{
	t3f_debug_message("Enter apply_redo_type()\n");
	switch(type)
	{
		case PA_UNDO_TYPE_TOOL:
		{
			return pa_apply_tool_redo(cep, fp, action);
		}
		case PA_REDO_TYPE_UNFLOAT_SELECTION:
		{
			return pa_apply_unfloat_selection_redo(cep, fp, action);
		}
		case PA_REDO_TYPE_FLOAT_SELECTION:
		{
			return pa_apply_float_selection_redo(cep, fp, action);
		}
		case PA_UNDO_TYPE_FRAME:
		{
			return pa_apply_frame_redo(cep, fp, action);
		}
		case PA_REDO_TYPE_ADD_LAYER:
		{
			return pa_apply_add_layer_redo(cep, fp);
		}
		case PA_REDO_TYPE_REMOVE_LAYER:
		{
			return pa_apply_remove_layer_redo(cep, fp);
		}
		case PA_UNDO_TYPE_FLOOD_FILL:
		{
			return pa_apply_flood_fill_redo(cep, fp);
		}
		case PA_UNDO_TYPE_IMPORT_IMAGE:
		{
			return pa_apply_import_redo(cep, fp);
		}
	}
	t3f_debug_message("Exit apply_redo_type()\n");
	return false;
}

bool pa_apply_undo(PA_CANVAS_EDITOR * cep, const char * fn, bool revert)
{
	ALLEGRO_FILE * fp = NULL;
	int type;
	int l;
	char buf[1024];
	bool ret;

	t3f_debug_message("Enter pa_apply_undo(cep, %s, %d)\n", fn, revert);
	fp = al_fopen(fn, "rb");
	if(!fp)
	{
		t3f_debug_message("Failed to open undo file\n");
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
		pa_set_window_message(NULL);
	}
	al_fclose(fp);
	t3f_debug_message("Exit pa_apply_undo()\n");
	return true;
}

bool pa_apply_redo(PA_CANVAS_EDITOR * cep, const char * fn)
{
	ALLEGRO_FILE * fp = NULL;
	int type;
	int l;
	char buf[1024];
	bool ret;

	t3f_debug_message("Enter pa_apply_redo(cep, %s)\n", fn);
	fp = al_fopen(fn, "rb");
	if(!fp)
	{
		t3f_debug_message("Failed to open redo file\n");
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
		pa_set_window_message(NULL);
	}
	al_fclose(fp);
	t3f_debug_message("Exit pa_apply_redo()\n");
	return true;
}

void pa_undo_clean_up(PA_CANVAS_EDITOR * cep)
{
	char undo_path[1024];
	int i;

	for(i = 0; i < cep->undo_count; i++)
	{
		al_remove_filename(pa_get_undo_path("undo", i, undo_path, 1024));
	}
	for(i = 0; i < cep->redo_count; i++)
	{
		al_remove_filename(pa_get_undo_path("redo", i, undo_path, 1024));
	}
}
