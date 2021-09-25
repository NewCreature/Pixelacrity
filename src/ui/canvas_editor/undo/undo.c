#include "t3f/t3f.h"
#include "modules/canvas/canvas.h"
#include "modules/canvas/canvas_helpers.h"
#include "ui/canvas_editor/canvas_editor.h"
#include "undo.h"
#include "tool.h"
#include "frame.h"
#include "selection.h"
#include "layer.h"
#include "flood_fill.h"
#include "import.h"
#include "ui/window.h"

bool pa_write_undo_header(ALLEGRO_FILE * fp, PA_CANVAS_EDITOR * cep, int type, const char * name)
{
	int i;

	t3f_debug_message("Write undo (%d): %s\n", type, name);
	al_fputc(fp, type);
	al_fwrite16le(fp, strlen(name));
	al_fwrite(fp, name, strlen(name));
	al_fwrite32le(fp, cep->canvas->layer_max);
	for(i = 0; i < cep->canvas->layer_max; i++)
	{
		al_fwrite32le(fp, cep->canvas->layer[i]->offset_x);
		al_fwrite32le(fp, cep->canvas->layer[i]->offset_y);
		al_fwrite32le(fp, cep->canvas->layer[i]->width);
		al_fwrite32le(fp, cep->canvas->layer[i]->height);
	}

	return true;
}

static void destroy_undo_header(PA_UNDO_HEADER * hp)
{
	if(hp)
	{
		if(hp->layer_height)
		{
			free(hp->layer_height);
		}
		if(hp->layer_width)
		{
			free(hp->layer_width);
		}
		if(hp->layer_offset_y)
		{
			free(hp->layer_offset_y);
		}
		if(hp->layer_offset_x)
		{
			free(hp->layer_offset_x);
		}
		if(hp->name)
		{
			free(hp->name);
		}
		free(hp);
	}
}

PA_UNDO_HEADER * pa_read_undo_header(ALLEGRO_FILE * fp)
{
	PA_UNDO_HEADER * hp;
	int i, l;

	hp = malloc(sizeof(PA_UNDO_HEADER));
	if(!hp)
	{
		goto fail;
	}
	memset(hp, 0, sizeof(PA_UNDO_HEADER));
	hp->type = al_fgetc(fp);
	l = al_fread16le(fp);
	hp->name = malloc(l + 1);
	if(!hp->name)
	{
		goto fail;
	}
	al_fread(fp, hp->name, l);
	hp->name[l] = 0;
	hp->layer_max = al_fread32le(fp);
	hp->layer_offset_x = malloc(sizeof(int) * hp->layer_max);
	if(!hp->layer_offset_x)
	{
		goto fail;
	}
	hp->layer_offset_y = malloc(sizeof(int) * hp->layer_max);
	if(!hp->layer_offset_y)
	{
		goto fail;
	}
	hp->layer_width = malloc(sizeof(int) * hp->layer_max);
	if(!hp->layer_width)
	{
		goto fail;
	}
	hp->layer_height = malloc(sizeof(int) * hp->layer_max);
	if(!hp->layer_height)
	{
		goto fail;
	}
	for(i = 0; i < hp->layer_max; i++)
	{
		hp->layer_offset_x[i] = al_fread32le(fp);
		hp->layer_offset_y[i] = al_fread32le(fp);
		hp->layer_width[i] = al_fread32le(fp);
		hp->layer_height[i] = al_fread32le(fp);
	}
	return hp;

	fail:
	{
		destroy_undo_header(hp);
	}
	return NULL;
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
		case PA_UNDO_TYPE_SWAP_LAYER:
		{
			return pa_apply_swap_layer_undo(cep, fp);
		}
		case PA_UNDO_TYPE_FLIP_HORIZONTAL:
		case PA_UNDO_TYPE_FLIP_VERTICAL:
		{
			return pa_apply_flip_selection_undo(cep, fp);
		}
		default:
		{
			printf("No handler for undo type %d!\n", type);
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
		case PA_UNDO_TYPE_SWAP_LAYER:
		{
			return pa_apply_swap_layer_redo(cep, fp);
		}
		case PA_UNDO_TYPE_FLIP_HORIZONTAL:
		case PA_UNDO_TYPE_FLIP_VERTICAL:
		{
			return pa_apply_flip_selection_redo(cep, fp);
		}
		default:
		{
			printf("No handler for undo type %d!\n", type);
		}
	}
	t3f_debug_message("Exit apply_redo_type()\n");
	return false;
}

static void update_canvas_size(PA_CANVAS_EDITOR * cep, PA_UNDO_HEADER * hp)
{
	int i;

	for(i = 0; i < hp->layer_max; i++)
	{
		cep->canvas->layer[i]->offset_x = hp->layer_offset_x[i];
		cep->canvas->layer[i]->offset_y = hp->layer_offset_y[i];
		cep->canvas->layer[i]->width = hp->layer_width[i];
		cep->canvas->layer[i]->height = hp->layer_height[i];
	}
}

bool pa_apply_undo(PA_CANVAS_EDITOR * cep, const char * fn, bool revert)
{
	ALLEGRO_FILE * fp = NULL;
	bool ret = false;
	PA_UNDO_HEADER * hp;

	t3f_debug_message("Enter pa_apply_undo(cep, %s, %d)\n", fn, revert);
	fp = al_fopen(fn, "rb");
	if(!fp)
	{
		t3f_debug_message("Failed to open undo file\n");
		return false;
	}
	hp = pa_read_undo_header(fp);
	if(hp)
	{
		ret = apply_undo_type(cep, fp, hp->type, hp->name, revert);
		if(ret)
		{
			update_canvas_size(cep, hp);
			cep->modified--;
			pa_set_window_message(NULL);
		}
		t3f_debug_message("Exit pa_apply_undo()\n");
	}
	al_fclose(fp);
	return ret;
}

bool pa_apply_redo(PA_CANVAS_EDITOR * cep, const char * fn)
{
	ALLEGRO_FILE * fp = NULL;
	bool ret = false;
	PA_UNDO_HEADER * hp;

	t3f_debug_message("Enter pa_apply_redo(cep, %s)\n", fn);
	fp = al_fopen(fn, "rb");
	if(!fp)
	{
		t3f_debug_message("Failed to open redo file\n");
		return false;
	}
	hp = pa_read_undo_header(fp);
	if(hp)
	{
		ret = apply_redo_type(cep, fp, hp->type, hp->name);
		if(ret)
		{
			update_canvas_size(cep, hp);
			cep->modified++;
			pa_set_window_message(NULL);
		}
		t3f_debug_message("Exit pa_apply_redo()\n");
	}
	al_fclose(fp);
	return ret;
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
