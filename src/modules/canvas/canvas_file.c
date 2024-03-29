#include "canvas.h"
#include "canvas_helpers.h"
#include "canvas_file.h"
#include "ui/window.h"

static const char canvas_header[4] = {'Q', 'X', 'L', 2};

static bool load_canvas_full_f_1(PA_CANVAS * cp, ALLEGRO_FILE * fp, const char * format)
{
	int max_layers;
	int i, j, k;
	char load;

	t3f_debug_message("Enter load_canvas_fill_f()\n");
	cp->layer_width = al_fread32le(fp);
	cp->layer_height = al_fread32le(fp);
	max_layers = al_fread32le(fp);
	for(i = 0; i < max_layers; i++)
	{
		if(!pa_add_canvas_layer(cp, -1))
		{
			goto fail;
		}
		cp->layer[i]->flags = al_fread32le(fp);
		for(j = 0; j < cp->layer_height; j++)
		{
			for(k = 0; k < cp->layer_width; k++)
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
	t3f_debug_message("Exit load_canvas_fill_f()\n");
	return true;

	fail:
	{
		t3f_debug_message("Fail load_canvas_fill_f()\n");
		return false;
	}
}

static bool load_canvas_minimal_f_1(PA_CANVAS * cp, ALLEGRO_FILE * fp, const char * format)
{
	ALLEGRO_BITMAP * bp;
	int max_layers;
	int i;

	t3f_debug_message("Enter load_canvas_minimal_f()\n");
	max_layers = al_fread32le(fp);
	for(i = 0; i < max_layers; i++)
	{
		if(!pa_add_canvas_layer(cp, -1))
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
		pa_import_bitmap_to_canvas(cp, bp, i, cp->export_offset_x, cp->export_offset_y);
		al_destroy_bitmap(bp);
	}
	t3f_debug_message("Exit load_canvas_minimal_f()\n");
	return true;

	fail:
	{
		t3f_debug_message("Fail load_canvas_minimal_f()\n");
		return false;
	}
}

static bool load_canvas_full_f(PA_CANVAS * cp, ALLEGRO_FILE * fp, const char * format)
{
	int max_layers;
	int i, j, k;
	char load;

	t3f_debug_message("Enter load_canvas_fill_f()\n");
	cp->layer_width = al_fread32le(fp);
	cp->layer_height = al_fread32le(fp);
	max_layers = al_fread32le(fp);
	pa_set_window_message("Updating layers...");
	for(i = 0; i < max_layers; i++)
	{
		if(!pa_add_canvas_layer(cp, -1))
		{
			goto fail;
		}
		cp->layer[i]->flags = al_fread32le(fp);
		cp->layer[i]->offset_x = al_fread32le(fp);
		cp->layer[i]->offset_y = al_fread32le(fp);
		cp->layer[i]->width = al_fread32le(fp);
		cp->layer[i]->height = al_fread32le(fp);
		for(j = 0; j < cp->layer_height; j++)
		{
			for(k = 0; k < cp->layer_width; k++)
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
	t3f_debug_message("Exit load_canvas_fill_f()\n");
	return true;

	fail:
	{
		t3f_debug_message("Fail load_canvas_fill_f()\n");
		return false;
	}
}

static bool load_canvas_minimal_f(PA_CANVAS * cp, ALLEGRO_FILE * fp, const char * format)
{
	ALLEGRO_BITMAP * bp;
	int max_layers;
	int i;

	t3f_debug_message("Enter load_canvas_minimal_f()\n");
	max_layers = al_fread32le(fp);
	pa_set_window_message("Updating layers...");
	for(i = 0; i < max_layers; i++)
	{
		if(!pa_add_canvas_layer(cp, -1))
		{
			goto fail;
		}
		cp->layer[i]->flags = al_fread32le(fp);
		cp->layer[i]->offset_x = al_fread32le(fp);
		cp->layer[i]->offset_y = al_fread32le(fp);
		cp->layer[i]->width = al_fread32le(fp);
		cp->layer[i]->height = al_fread32le(fp);
		bp = al_load_bitmap_flags_f(fp, format, ALLEGRO_NO_PREMULTIPLIED_ALPHA);
		if(!bp)
		{
			goto fail;
		}
		pa_import_bitmap_to_canvas(cp, bp, i, cp->export_offset_x, cp->export_offset_y);
		al_destroy_bitmap(bp);
	}
	t3f_debug_message("Exit load_canvas_minimal_f()\n");
	return true;

	fail:
	{
		t3f_debug_message("Fail load_canvas_minimal_f()\n");
		return false;
	}
}

static PA_CANVAS * pa_load_canvas_f(ALLEGRO_FILE * fp, int bitmap_max)
{
	PA_CANVAS * cp = NULL;
	ALLEGRO_STATE old_state;
	char header[4] = {0};
	char format[16] = {0};
	int method;
	int frame_count = 0;
	char * frame_name_buf = NULL;
	int i, l, x, y, w, h;

	t3f_debug_message("Enter load_canvas_f()\n");
	al_store_state(&old_state, ALLEGRO_STATE_NEW_BITMAP_PARAMETERS);
	al_set_new_bitmap_flags(0);

	al_fread(fp, header, 4);
	if(memcmp(header, canvas_header, 3))
	{
		goto fail;
	}
	cp = pa_create_canvas(bitmap_max);
	if(!cp)
	{
		goto fail;
	}
	switch(header[3])
	{
		case 1:
		{
			frame_count = al_fread32le(fp);
			for(i = 0; i < frame_count; i++)
			{
				l = al_fread32le(fp);
				if(l > 0)
				{
					frame_name_buf = malloc(l + 1);
					memset(frame_name_buf, 0, l + 1);
					al_fread(fp, frame_name_buf, l);
				}
				x = al_fread32le(fp);
				y = al_fread32le(fp);
				w = al_fread32le(fp);
				h = al_fread32le(fp);
				if(!pa_add_canvas_frame(cp, frame_name_buf, x, y, w, h))
				{
					goto fail;
				}
				if(frame_name_buf)
				{
					free(frame_name_buf);
					frame_name_buf = NULL;
				}
			}

			al_fread(fp, format, 16);
			method = al_fread32le(fp);
			switch(method)
			{
				case PA_CANVAS_SAVE_FULL:
				{
					if(!load_canvas_full_f_1(cp, fp, format))
					{
						goto fail;
					}
					break;
				}
				case PA_CANVAS_SAVE_MINIMAL:
				{
					if(!load_canvas_minimal_f_1(cp, fp, format))
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
			break;
		}
		case 2:
		{
			frame_count = al_fread32le(fp);
			for(i = 0; i < frame_count; i++)
			{
				l = al_fread32le(fp);
				if(l > 0)
				{
					frame_name_buf = malloc(l + 1);
					memset(frame_name_buf, 0, l + 1);
					al_fread(fp, frame_name_buf, l);
				}
				x = al_fread32le(fp);
				y = al_fread32le(fp);
				w = al_fread32le(fp);
				h = al_fread32le(fp);
				if(!pa_add_canvas_frame(cp, frame_name_buf, x, y, w, h))
				{
					goto fail;
				}
				if(frame_name_buf)
				{
					free(frame_name_buf);
					frame_name_buf = NULL;
				}
			}

			al_fread(fp, format, 16);
			method = al_fread32le(fp);
			switch(method)
			{
				case PA_CANVAS_SAVE_FULL:
				{
					if(!load_canvas_full_f(cp, fp, format))
					{
						goto fail;
					}
					break;
				}
				case PA_CANVAS_SAVE_MINIMAL:
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
			break;
		}
	}
	al_restore_state(&old_state);
	t3f_debug_message("Exit load_canvas_f()\n");
	return cp;

	fail:
	{
		t3f_debug_message("Fail load_canvas_f()\n");
		pa_destroy_canvas(cp);
		al_restore_state(&old_state);
		return NULL;
	}
}

PA_CANVAS * pa_load_canvas(const char * fn, int bitmap_max)
{
	ALLEGRO_FILE * fp;
	PA_CANVAS * cp = NULL;

	fp = al_fopen(fn, "rb");
	if(!fp)
	{
		goto fail;
	}
	cp = pa_load_canvas_f(fp, bitmap_max);
	al_fclose(fp);
	fp = NULL;
	if(!cp)
	{
		goto fail;
	}

	return cp;

	fail:
	{
		if(fp)
		{
			al_fclose(fp);
		}
		pa_destroy_canvas(cp);
		return NULL;
	}
}

static int autodetect_method(PA_CANVAS * cp)
{
	return PA_CANVAS_SAVE_FULL;
}

static bool save_canvas_full_f(PA_CANVAS * cp, ALLEGRO_FILE * fp, const char * format)
{
	int i, j, k;

	t3f_debug_message("Enter save_canvas_full_f()\n");
	if(!al_fwrite32le(fp, cp->layer_width))
	{
		goto fail;
	}
	if(!al_fwrite32le(fp, cp->layer_height))
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
		if(!al_fwrite32le(fp, cp->layer[i]->offset_x))
		{
			goto fail;
		}
		if(!al_fwrite32le(fp, cp->layer[i]->offset_y))
		{
			goto fail;
		}
		if(!al_fwrite32le(fp, cp->layer[i]->width))
		{
			goto fail;
		}
		if(!al_fwrite32le(fp, cp->layer[i]->height))
		{
			goto fail;
		}
		for(j = 0; j < cp->layer_height; j++)
		{
			for(k = 0; k < cp->layer_width; k++)
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
	t3f_debug_message("Exit save_canvas_full_f()\n");
	return true;

	fail:
	{
		t3f_debug_message("Fail save_canvas_full_f()\n");
		return false;
	}
}

static bool save_canvas_minimal_f(PA_CANVAS * cp, ALLEGRO_FILE * fp, const char * format)
{
	ALLEGRO_BITMAP * bp;
	int i;

	t3f_debug_message("Enter save_canvas_minimal_f()\n");
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
		if(!al_fwrite32le(fp, cp->layer[i]->offset_x))
		{
			goto fail;
		}
		if(!al_fwrite32le(fp, cp->layer[i]->offset_y))
		{
			goto fail;
		}
		if(!al_fwrite32le(fp, cp->layer[i]->width))
		{
			goto fail;
		}
		if(!al_fwrite32le(fp, cp->layer[i]->height))
		{
			goto fail;
		}
		bp = pa_get_bitmap_from_canvas(cp, i, i + 1, 0, false);
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
	t3f_debug_message("Exit save_canvas_minimal_f()\n");
	return true;

	fail:
	{
		t3f_debug_message("Fail save_canvas_minimal_f()\n");
		return false;
	}
}

static bool pa_save_canvas_f(PA_CANVAS * cp, ALLEGRO_FILE * fp, const char * format, int method)
{
	char format_string[16] = {0};
	int i, l;

	t3f_debug_message("Enter save_canvas_f()\n");
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
	al_fwrite32le(fp, cp->frame_max);
	for(i = 0; i < cp->frame_max; i++)
	{
		if(cp->frame[i]->name)
		{
			l = strlen(cp->frame[i]->name);
		}
		else
		{
			l = 0;
		}
		al_fwrite32le(fp, l);
		if(l > 0)
		{
			if(al_fwrite(fp, cp->frame[i]->name, l) < l)
			{
				goto fail;
			}
			al_fwrite32le(fp, cp->frame[i]->box.start_x);
			al_fwrite32le(fp, cp->frame[i]->box.start_y);
			al_fwrite32le(fp, cp->frame[i]->box.width);
			al_fwrite32le(fp, cp->frame[i]->box.height);
		}
	}

	if(al_fwrite(fp, format_string, 16) < 16)
	{
		goto fail;
	}
	if(method == PA_CANVAS_SAVE_AUTO)
	{
		method = autodetect_method(cp);
	}
	al_fwrite32le(fp, method);
	switch(method)
	{
		case PA_CANVAS_SAVE_FULL:
		{
			save_canvas_full_f(cp, fp, format_string);
			break;
		}
		case PA_CANVAS_SAVE_MINIMAL:
		{
			save_canvas_minimal_f(cp, fp, format_string);
			break;
		}
		default:
		{
			goto fail;
		}
	}
	t3f_debug_message("Exit save_canvas_f()\n");
	return true;

	fail:
	{
		t3f_debug_message("Fail save_canvas_f()\n");
		return false;
	}
}

bool pa_save_canvas(PA_CANVAS * cp, const char * fn, const char * format, int method)
{
	ALLEGRO_FILE * fp;
	bool ret;

	fp = al_fopen(fn, "wb");
	if(!fp)
	{
		goto fail;
	}
	ret = pa_save_canvas_f(cp, fp, format, method);
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
