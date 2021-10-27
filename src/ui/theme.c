#include "t3f/t3f.h"
#include "t3gui/t3gui.h"
#include "theme.h"

PA_UI_THEME * pa_load_theme(const char * fn)
{
	PA_UI_THEME * tp;

	tp = malloc(sizeof(PA_UI_THEME));
	if(!tp)
	{
		goto fail;
	}
	tp->path = al_create_path(fn);
	if(!tp->path)
	{
		goto fail;
	}
	tp->config = al_load_config_file(fn);
	if(!tp->config)
	{
		goto fail;
	}
	tp->theme[PA_UI_THEME_BOX] = pa_load_theme_theme(tp, "box");
	tp->theme[PA_UI_THEME_BUTTON] = pa_load_theme_theme(tp, "button");
	return tp;

	fail:
	{
		pa_destroy_theme(tp);
		return NULL;
	}
}

void pa_destroy_theme(PA_UI_THEME * tp)
{
	int i;

	if(tp)
	{
		for(i = 0; i < PA_UI_THEME_MAX; i++)
		{
			if(tp->theme[i])
			{
				t3gui_destroy_theme(tp->theme[i]);
			}
		}
		if(tp->config)
		{
			al_destroy_config(tp->config);
		}
		if(tp->path)
		{
			al_destroy_path(tp->path);
		}
		free(tp);
	}
}

ALLEGRO_BITMAP * pa_load_theme_bitmap(PA_UI_THEME * tp, const char * name)
{
	ALLEGRO_BITMAP * bp = NULL;
	const char * val;

	val = al_get_config_value(tp->config, "Graphics", name);
	if(val)
	{
		bp = al_load_bitmap_flags(val, ALLEGRO_NO_PREMULTIPLIED_ALPHA);
		if(!bp)
		{
			al_set_path_filename(tp->path, val);
			bp = al_load_bitmap_flags(al_path_cstr(tp->path, '/'), ALLEGRO_NO_PREMULTIPLIED_ALPHA);
		}
	}
	return bp;
}

T3GUI_THEME * pa_load_theme_theme(PA_UI_THEME * tp, const char * name)
{
	T3GUI_THEME * ttp = NULL;
	const char * val;

	val = al_get_config_value(tp->config, "Themes", name);
	if(val)
	{
		ttp = t3gui_load_theme(val, 16);
		if(!ttp)
		{
			al_set_path_filename(tp->path, val);
			ttp = t3gui_load_theme(al_path_cstr(tp->path, '/'), 16);
		}
	}
	return ttp;
}
