#include "canvas.h"
#include "canvas_helpers.h"
#include "canvas_file.h"

static const char canvas_header[4] = {'Q', 'X', 'L', 1};

static bool load_canvas_full_f(QUIXEL_CANVAS * cp, ALLEGRO_FILE * fp, const char * format)
{
	int max_layers, max_width, max_height;
	int i, j, k;
	char load;

	max_width = al_fread32le(fp);
	max_height = al_fread32le(fp);
	max_layers = al_fread32le(fp);
	for(i = 0; i < max_layers; i++)
	{
		if(!quixel_add_canvas_layer(cp))
		{
			goto fail;
		}
		cp->layer[i]->flags = al_fread32le(fp);
		for(j = 0; j < max_height; j++)
		{
			for(k = 0; k < max_width; k++)
			{
				load = al_fgetc(fp);
				if(load)
				{
					cp->layer[i]->bitmap[j][k] = al_load_bitmap_flags_f(fp, format, ALLEGRO_NO_PREMULTIPLIED_ALPHA);
					if(!cp->layer[i]->bitmap[j][k])
					{
						goto fail;
					}
				}
			}
		}
	}
	return true;

	fail:
	{
		return false;
	}
}

static bool load_canvas_minimal_f(QUIXEL_CANVAS * cp, ALLEGRO_FILE * fp, const char * format)
{
	ALLEGRO_BITMAP * bp;
	int max_layers;
	int i;

	max_layers = al_fread32le(fp);
	for(i = 0; i < max_layers; i++)
	{
		if(!quixel_add_canvas_layer(cp))
		{
			goto fail;
		}
		cp->layer[i]->flags = al_fread32le(fp);
		cp->export_offset_x = al_fread32le(fp);
		cp->export_offset_y = al_fread32le(fp);
		bp = al_load_bitmap_flags_f(fp, format, ALLEGRO_NO_PREMULTIPLIED_ALPHA);
		if(!bp)
		{
			goto fail;
		}
		quixel_import_bitmap_to_canvas(cp, bp, i, cp->export_offset_x, cp->export_offset_y);
		al_destroy_bitmap(bp);
	}
	return true;

	fail:
	{
		return false;
	}
}

static QUIXEL_CANVAS * quixel_load_canvas_f(ALLEGRO_FILE * fp, int bitmap_max)
{
	QUIXEL_CANVAS * cp = NULL;
	ALLEGRO_STATE old_state;
	char header[4] = {0};
	char format[16] = {0};
	int method;

	al_store_state(&old_state, ALLEGRO_STATE_NEW_BITMAP_PARAMETERS);
	al_set_new_bitmap_flags(0);

	al_fread(fp, header, 4);
	if(memcmp(header, canvas_header, 4))
	{
		goto fail;
	}
	al_fread(fp, format, 16);
	cp = quixel_create_canvas(bitmap_max);
	if(!cp)
	{
		goto fail;
	}
	method = al_fread32le(fp);
	switch(method)
	{
		case QUIXEL_CANVAS_SAVE_FULL:
		{
			if(!load_canvas_full_f(cp, fp, format))
			{
				goto fail;
			}
			break;
		}
		case QUIXEL_CANVAS_SAVE_MINIMAL:
		{
			if(!load_canvas_minimal_f(cp, fp, format))
			{
				goto fail;
			}
			break;
		}
		default:
		{
			goto fail;
		}
	}
	al_restore_state(&old_state);
	return cp;

	fail:
	{
		quixel_destroy_canvas(cp);
		al_restore_state(&old_state);
		return NULL;
	}
}

QUIXEL_CANVAS * quixel_load_canvas(const char * fn, int bitmap_max)
{
	ALLEGRO_FILE * fp;
	QUIXEL_CANVAS * cp = NULL;
	ALLEGRO_PATH * pp;

	fp = al_fopen(fn, "rb");
	if(!fp)
	{
		goto fail;
	}
	cp = quixel_load_canvas_f(fp, bitmap_max);
	al_fclose(fp);
	fp = NULL;
	if(!cp)
	{
		goto fail;
	}

	pp = al_create_path(fn);
	if(pp)
	{
		al_set_path_extension(pp, ".ini");
		cp->config = al_load_config_file(al_path_cstr(pp, '/'));
		if(!cp->config)
		{
			cp->config = al_create_config();
			if(!cp->config)
			{
				goto fail;
			}
		}
		al_destroy_path(pp);
	}

	return cp;

	fail:
	{
		if(fp)
		{
			al_fclose(fp);
		}
		quixel_destroy_canvas(cp);
		return NULL;
	}
}

static int autodetect_method(QUIXEL_CANVAS * cp)
{
	return QUIXEL_CANVAS_SAVE_MINIMAL;
}

static bool save_canvas_full_f(QUIXEL_CANVAS * cp, ALLEGRO_FILE * fp, const char * format)
{
	int i, j, k;

	if(!al_fwrite32le(fp, QUIXEL_CANVAS_MAX_WIDTH))
	{
		goto fail;
	}
	if(!al_fwrite32le(fp, QUIXEL_CANVAS_MAX_HEIGHT))
	{
		goto fail;
	}
	if(!al_fwrite32le(fp, cp->layer_max))
	{
		goto fail;
	}
	for(i = 0; i < cp->layer_max; i++)
	{
		if(!al_fwrite32le(fp, cp->layer[i]->flags))
		{
			goto fail;
		}
		for(j = 0; j < QUIXEL_CANVAS_MAX_HEIGHT; j++)
		{
			for(k = 0; k < QUIXEL_CANVAS_MAX_WIDTH; k++)
			{
				if(cp->layer[i]->bitmap[j][k])
				{
					if(al_fputc(fp, 1) == EOF)
					{
						goto fail;
					}
					if(!al_save_bitmap_f(fp, format, cp->layer[i]->bitmap[j][k]))
					{
						goto fail;
					}
				}
				else
				{
					if(al_fputc(fp, 0) == EOF)
					{
						goto fail;
					}
				}
			}
		}
	}
	return true;

	fail:
	{
		return false;
	}
}

static bool save_canvas_minimal_f(QUIXEL_CANVAS * cp, ALLEGRO_FILE * fp, const char * format)
{
	ALLEGRO_BITMAP * bp;
	int i;

	if(!al_fwrite32le(fp, cp->layer_max))
	{
		goto fail;
	}
	for(i = 0; i < cp->layer_max; i++)
	{
		if(!al_fwrite32le(fp, cp->layer[i]->flags))
		{
			goto fail;
		}
		bp = quixel_render_canvas_to_bitmap(cp, i, i + 1, 0);
		if(!bp)
		{
			goto fail;
		}
		if(!al_fwrite32le(fp, cp->export_offset_x))
		{
			goto fail;
		}
		if(!al_fwrite32le(fp, cp->export_offset_y))
		{
			goto fail;
		}
		if(!al_save_bitmap_f(fp, format, bp))
		{
			goto fail;
		}
		al_destroy_bitmap(bp);
	}
	return true;

	fail:
	{
		return false;
	}
}

static bool quixel_save_canvas_f(QUIXEL_CANVAS * cp, ALLEGRO_FILE * fp, const char * format, int method)
{
	char format_string[16] = {0};

	if(strlen(format) < 16)
	{
		strcpy(format_string, format);
	}
	else
	{
		printf("Invalid format: %s!\n", format);
	}

	if(al_fwrite(fp, canvas_header, 4) < 4)
	{
		goto fail;
		return false;
	}
	if(al_fwrite(fp, format_string, 16) < 16)
	{
		goto fail;
	}
	if(method == QUIXEL_CANVAS_SAVE_AUTO)
	{
		method = autodetect_method(cp);
	}
	al_fwrite32le(fp, method);
	switch(method)
	{
		case QUIXEL_CANVAS_SAVE_FULL:
		{
			save_canvas_full_f(cp, fp, format_string);
			break;
		}
		case QUIXEL_CANVAS_SAVE_MINIMAL:
		{
			save_canvas_minimal_f(cp, fp, format_string);
			break;
		}
		default:
		{
			goto fail;
		}
	}
	return true;

	fail:
	{
		return false;
	}
}

bool quixel_save_canvas(QUIXEL_CANVAS * cp, const char * fn, const char * format, int method)
{
	ALLEGRO_FILE * fp;
	bool ret;
	ALLEGRO_PATH * pp;

	fp = al_fopen(fn, "wb");
	if(!fp)
	{
		goto fail;
	}
	ret = quixel_save_canvas_f(cp, fp, format, method);
	al_fclose(fp);

	pp = al_create_path(fn);
	if(pp)
	{
		al_set_path_extension(pp, ".ini");
		al_save_config_file(al_path_cstr(pp, '/'), cp->config);
		al_destroy_path(pp);
	}

	return ret;

	fail:
	{
		if(fp)
		{
			al_fclose(fp);
		}
		return false;
	}
}
