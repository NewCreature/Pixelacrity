#include "canvas.h"

static const char canvas_header[4] = {'Q', 'X', 'L', 1};

static QUIXEL_CANVAS * quixel_load_canvas_f(ALLEGRO_FILE * fp, int bitmap_max)
{
	QUIXEL_CANVAS * cp = NULL;
	ALLEGRO_STATE old_state;
	char header[4] = {0};
	char format[16] = {0};
	int max_width, max_height, max_layers;
	char load;
	int i, j, k;

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
	QUIXEL_CANVAS * cp;

	fp = al_fopen(fn, "rb");
	if(!fp)
	{
		goto fail;
	}
	cp = quixel_load_canvas_f(fp, bitmap_max);
	al_fclose(fp);

	return cp;

	fail:
	{
		if(fp)
		{
			al_fclose(fp);
		}
		return NULL;
	}
}

static bool quixel_save_canvas_f(QUIXEL_CANVAS * cp, ALLEGRO_FILE * fp, const char * format)
{
	char format_string[16] = {0};
	int i, j, k;

	if(al_fwrite(fp, canvas_header, 4) < 4)
	{
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
		return false;
	}
	if(!al_fwrite32le(fp, QUIXEL_CANVAS_MAX_WIDTH))
	{
		return false;
	}
	if(!al_fwrite32le(fp, QUIXEL_CANVAS_MAX_HEIGHT))
	{
		return false;
	}
	if(!al_fwrite32le(fp, cp->layer_max))
	{
		return false;
	}
	for(i = 0; i < cp->layer_max; i++)
	{
		for(j = 0; j < QUIXEL_CANVAS_MAX_HEIGHT; j++)
		{
			for(k = 0; k < QUIXEL_CANVAS_MAX_WIDTH; k++)
			{
				if(cp->layer[i]->bitmap[j][k])
				{
					if(al_fputc(fp, 1) == EOF)
					{
						return false;
					}
					if(!al_save_bitmap_f(fp, format, cp->layer[i]->bitmap[j][k]))
					{
						return false;
					}
				}
				else
				{
					if(al_fputc(fp, 0) == EOF)
					{
						return false;
					}
				}
			}
		}
	}
	return true;
}

bool quixel_save_canvas(QUIXEL_CANVAS * cp, const char * fn, const char * format)
{
	ALLEGRO_FILE * fp;
	bool ret;

	fp = al_fopen(fn, "wb");
	if(!fp)
	{
		goto fail;
	}
	ret = quixel_save_canvas_f(cp, fp, format);
	al_fclose(fp);
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
