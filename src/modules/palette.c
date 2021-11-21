#include "t3f/t3f.h"
#include "palette.h"
#include "color.h"

static bool resize_palette(PA_PALETTE * pp, int size)
{
	ALLEGRO_COLOR * new_color = NULL;

	if(size != pp->color_max || !pp->color)
	{
		new_color = malloc(sizeof(ALLEGRO_COLOR) * size);
	}
	if(new_color)
	{
		if(pp->color)
		{
			free(pp->color);
		}
		pp->color = new_color;
		pp->color_max = size;
		return true;
	}
	if(size != pp->color_max)
	{
		return false;
	}
	return true;
}

PA_PALETTE * pa_create_palette(void)
{
	PA_PALETTE * pp;

	pp = malloc(sizeof(PA_PALETTE));
	if(!pp)
	{
		goto fail;
	}
	memset(pp, 0, sizeof(PA_PALETTE));
	return pp;

	fail:
	{
		pa_destroy_palette(pp);
	}
	return NULL;
}

bool pa_load_palette(PA_PALETTE * pp, const char * fn)
{
	ALLEGRO_CONFIG * config;

	config = al_load_config_file(fn);
	if(!config)
	{
		goto fail;
	}
	if(!pa_read_palette(pp, config))
	{
		goto fail;
	}
	al_destroy_config(config);
	return true;

	fail:
	{
		if(config)
		{
			al_destroy_config(config);
		}
	}
	return false;
}

bool pa_save_palette(PA_PALETTE * pp, const char * fn)
{
	ALLEGRO_CONFIG * config;

	config = al_create_config();
	if(!config)
	{
		goto fail;
	}
	if(!pa_write_palette(pp, config))
	{
		goto fail;
	}
	al_save_config_file(fn, config);
	return true;

	fail:
	{
		if(config)
		{
			al_destroy_config(config);
		}
	}
	return false;
}

void pa_destroy_palette(PA_PALETTE * pp)
{
	if(pp)
	{
		if(pp->color)
		{
			free(pp->color);
		}
		free(pp);
	}
}

bool pa_read_palette(PA_PALETTE * pp, const ALLEGRO_CONFIG * config)
{
	int i;
	const char * val;
	char buf[32];

	val = al_get_config_value(config, "Palette", "color_max");
	if(val)
	{
		if(!resize_palette(pp, atoi(val)))
		{
			return false;
		}
	}
	for(i = 0; i < pp->color_max; i++)
	{
		sprintf(buf, "color_%d", i);
		val = al_get_config_value(config, "Palette", buf);
		if(val)
		{
			pp->color[i] = pa_get_color_from_html(val);
		}
	}
	return true;
}

bool pa_write_palette(PA_PALETTE * pp, ALLEGRO_CONFIG * config)
{
	int i;
	char buf[32];
	char val[32];

	sprintf(val, "%d", pp->color_max);
	al_set_config_value(config, "Palette", "color_max", val);
	for(i = 0; i < pp->color_max; i++)
	{
		pa_color_to_html(pp->color[i], val);
		sprintf(buf, "color_%d", i);
		al_set_config_value(config, "Palette", buf, val);
	}
	return true;
}
