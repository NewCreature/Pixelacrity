#include "t3f/t3f.h"
#include "canvas_helpers.h"
#include "canvas_editor.h"
#include "undo.h"

static bool write_undo_header(ALLEGRO_FILE * fp, int type, const char * name)
{
	al_fputc(fp, type);
	al_fwrite16le(fp, strlen(name));
	al_fwrite(fp, name, strlen(name));

	return true;
}

bool quixel_make_tool_undo(QUIXEL_CANVAS_EDITOR * cep, const char * action, int layer, int x, int y, int width, int height, const char * fn)
{
	ALLEGRO_STATE old_state;
	ALLEGRO_BITMAP * bp = NULL;
	ALLEGRO_FILE * fp = NULL;
	const char * action_name;

	if(width <= 0 || height <= 0)
	{
		quixel_get_canvas_dimensions(cep->canvas, &x, &y, &width, &height, 0);
	}
	if(width > 0 && height > 0)
	{
		al_store_state(&old_state, ALLEGRO_STATE_NEW_BITMAP_PARAMETERS);
		al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
		bp = al_create_bitmap(width, height);
		al_restore_state(&old_state);
		if(!bp)
		{
			goto fail;
		}
	}
	if(width > 0 && height > 0)
	{
		quixel_render_canvas_to_bitmap(cep->canvas, layer, layer + 1, x, y, width, height, 0, bp);
	}
	fp = al_fopen(fn, "wb");
	if(!fp)
	{
		printf("fail: %s\n", fn);
		goto fail;
	}
	switch(cep->current_tool)
	{
		case QUIXEL_TOOL_PIXEL:
		{
			action_name = "Draw Pixel";
			break;
		}
		case QUIXEL_TOOL_LINE:
		{
			action_name = "Draw Line";
			break;
		}
		case QUIXEL_TOOL_RECTANGLE:
		{
			action_name = "Draw Rectangle";
			break;
		}
		case QUIXEL_TOOL_FILLED_RECTANGLE:
		{
			action_name = "Draw Filled Rectangle";
			break;
		}
		case QUIXEL_TOOL_OVAL:
		{
			action_name = "Draw Oval";
			break;
		}
		case QUIXEL_TOOL_FILLED_OVAL:
		{
			action_name = "Draw Filled Oval";
			break;
		}
		case QUIXEL_TOOL_FLOOD_FILL:
		{
			action_name = "Flood Fill";
			break;
		}
		default:
		{
			action_name = "Draw";
			break;
		}
	}
	write_undo_header(fp, QUIXEL_UNDO_TYPE_TOOL, action ? action : action_name);
	if(width < 0 || height < 0)
	{
		al_fputc(fp, 0);
	}
	else
	{
		al_fputc(fp, 1);
		al_fwrite32le(fp, layer);
		al_fwrite32le(fp, x);
		al_fwrite32le(fp, y);
		if(!al_save_bitmap_f(fp, ".png", bp))
		{
			printf("fail: %s\n", fn);
			goto fail;
		}
	}
	al_fclose(fp);
	return true;

	fail:
	{
		if(fp)
		{
			al_fclose(fp);
		}
		if(bp)
		{
			al_destroy_bitmap(bp);
		}
		return false;
	}
}

bool quixel_make_tool_redo(QUIXEL_CANVAS_EDITOR * cep, const char * action, int layer, int x, int y, int width, int height, const char * fn)
{
	return quixel_make_tool_undo(cep, action, layer, x, y, width, height, fn);
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

bool quixel_apply_undo(QUIXEL_CANVAS_EDITOR * cep, const char * fn, bool redo, bool revert)
{
	ALLEGRO_FILE * fp = NULL;
	int type;
	int l;
	char buf[1024];
	char undo_path[1024];
	ALLEGRO_BITMAP * bp;
	int layer, x, y;

	fp = al_fopen(fn, "rb");
	if(!fp)
	{
		goto fail;
	}
	type = al_fgetc(fp);
	l = al_fread16le(fp);
	al_fread(fp, buf, l);
	buf[l] = 0;
	switch(type)
	{
		case QUIXEL_UNDO_TYPE_TOOL:
		{
			l = al_fgetc(fp);
			if(l)
			{
				layer = al_fread32le(fp);
				x = al_fread32le(fp);
				y = al_fread32le(fp);
				bp = al_load_bitmap_flags_f(fp, ".png", ALLEGRO_NO_PREMULTIPLIED_ALPHA);
				if(!bp)
				{
					goto fail;
				}
				if(!revert)
				{
					if(!redo)
					{
						quixel_make_tool_redo(cep, buf, layer, x, y, al_get_bitmap_width(bp), al_get_bitmap_height(bp), quixel_get_undo_path("redo", cep->redo_count, undo_path, 1024));
						cep->redo_count++;
					}
					else
					{
						quixel_make_tool_undo(cep, buf, layer, x, y, al_get_bitmap_width(bp), al_get_bitmap_height(bp), quixel_get_undo_path("undo", cep->undo_count, undo_path, 1024));
						cep->undo_count++;
					}
				}
				quixel_import_bitmap_to_canvas(cep->canvas, bp, layer, x, y);
				al_destroy_bitmap(bp);
			}
			break;
		}
	}
	al_fclose(fp);
	return false;

	fail:
	{
		if(fp)
		{
			al_fclose(fp);
		}
		return false;
	}
}

bool quixel_apply_redo(QUIXEL_CANVAS_EDITOR * cep, const char * fn)
{
	return quixel_apply_undo(cep, fn, true, false);
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
