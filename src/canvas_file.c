#include "canvas.h"
#include "canvas_helpers.h"

static const char canvas_header[4] = {'Q', 'X', 'L', 1};

static QUIXEL_CANVAS * quixel_load_canvas_f(ALLEGRO_FILE * fp, int bitmap_max)
{
	QUIXEL_CANVAS * cp = NULL;
	ALLEGRO_STATE old_state;
	ALLEGRO_BITMAP * bp;
	char header[4] = {0};
	char format[16] = {0};
	int max_width, max_height, max_layers;
	int i, x, y;

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
		bp = al_load_bitmap_flags_f(fp, format, ALLEGRO_NO_PREMULTIPLIED_ALPHA);
		if(!bp)
		{
			goto fail;
		}
		x = (max_width * cp->bitmap_size) / 2 - al_get_bitmap_width(bp) / 2;
		y = (max_height * cp->bitmap_size) / 2 - al_get_bitmap_height(bp) / 2;
		quixel_import_bitmap_to_canvas(cp, bp, i, x, y);
		al_destroy_bitmap(bp);
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

static bool quixel_save_canvas_f(QUIXEL_CANVAS * cp, ALLEGRO_FILE * fp, const char * format)
{
	ALLEGRO_BITMAP * bp = NULL;
	char format_string[16] = {0};
	int i;

	if(al_fwrite(fp, canvas_header, 4) < 4)
	{
		goto fail;
		return false;
	}
	if(strlen(format) < 16)
	{
		strcpy(format_string, format);
	}
	else
	{
		printf("Invalid format: %s!\n", format);
	}
	if(al_fwrite(fp, format_string, 16) < 16)
	{
		goto fail;
	}
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
		bp = quixel_render_canvas_to_bitmap(cp, i, i + 1, 0);
		if(!bp)
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
		if(bp)
		{
			al_destroy_bitmap(bp);
		}
		return false;
	}
}

bool quixel_save_canvas(QUIXEL_CANVAS * cp, const char * fn, const char * format)
{
	ALLEGRO_FILE * fp;
	bool ret;
	ALLEGRO_PATH * pp;

	fp = al_fopen(fn, "wb");
	if(!fp)
	{
		goto fail;
	}
	ret = quixel_save_canvas_f(cp, fp, format);
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
