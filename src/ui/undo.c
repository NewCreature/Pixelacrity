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

bool quixel_make_tool_undo(QUIXEL_CANVAS_EDITOR * cep, int layer, int x, int y, int width, int height, const char * fn)
{
	ALLEGRO_STATE old_state;
	ALLEGRO_BITMAP * bp = NULL;
	ALLEGRO_FILE * fp = NULL;

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
	write_undo_header(fp, QUIXEL_UNDO_TYPE_TOOL, "Undo Draw");
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

bool quixel_make_tool_redo(QUIXEL_CANVAS_EDITOR * cep, int layer, int x, int y, int width, int height, const char * fn)
{
	return quixel_make_tool_undo(cep, layer, x, y, width, height, fn);
}

static char undo_name_buffer[1024] = {0};

const char * quixel_get_undo_name(const char * fn)
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
	if(l >= 1024 || l < 1)
	{
		goto fail;
	}
	al_fread(fp, undo_name_buffer, l);
	al_fclose(fp);

	return undo_name_buffer;

	fail:
	{
		if(fp)
		{
			al_fclose(fp);
		}
		return NULL;
	}
}

static char undo_path[4096] = {0};
const char * quixel_get_undo_path(int count)
{
	char buf[256];

	sprintf(buf, "undo.%04d", count);
	return t3f_get_filename(t3f_data_path, buf, undo_path, 4096);
}

bool quixel_apply_undo(QUIXEL_CANVAS_EDITOR * cep, const char * fn)
{
	ALLEGRO_FILE * fp = NULL;
	int type;
	int l;
	char buf[1024];
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
	return false;
}
