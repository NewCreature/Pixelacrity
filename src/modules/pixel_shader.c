#include "t3f/t3f.h"

static ALLEGRO_SHADER * default_shader = NULL;

ALLEGRO_SHADER * pa_create_pixel_shader(const char * fn)
{
	ALLEGRO_SHADER * shader = NULL;

	shader = al_create_shader(ALLEGRO_SHADER_AUTO);
	if(!shader)
	{
		goto fail;
	}

	if(!al_attach_shader_source(shader, ALLEGRO_VERTEX_SHADER, al_get_default_shader_source(ALLEGRO_SHADER_AUTO, ALLEGRO_VERTEX_SHADER)))
	{
		goto fail;
	}

	if(!al_attach_shader_source_file(shader, ALLEGRO_PIXEL_SHADER, fn))
	{
		goto fail;
	}
	if(!al_build_shader(shader))
	{
		goto fail;
	}

	return shader;

	fail:
	{
		if(shader)
		{
			printf("Shader error:\n\n%s\n", al_get_shader_log(shader));
			al_destroy_shader(shader);
		}
		return NULL;
	}
}

bool pa_set_target_pixel_shader(ALLEGRO_SHADER * sp)
{
	if(sp)
	{
		if(!al_use_shader(sp))
		{
			return false;
		}
	}
	else
	{
		if(!default_shader)
		{
			default_shader = al_create_shader(ALLEGRO_SHADER_AUTO);
			if(!default_shader)
			{
				goto fail;
			}
			if(!al_attach_shader_source(default_shader, ALLEGRO_VERTEX_SHADER, al_get_default_shader_source(ALLEGRO_SHADER_AUTO, ALLEGRO_VERTEX_SHADER)))
			{
				goto fail;
			}
			if(!al_attach_shader_source(default_shader, ALLEGRO_PIXEL_SHADER, al_get_default_shader_source(ALLEGRO_SHADER_AUTO, ALLEGRO_PIXEL_SHADER)))
			{
				goto fail;
			}
			if(!al_build_shader(default_shader))
			{
				goto fail;
			}
		}
		al_use_shader(default_shader);
	}
	return true;

	fail:
	{
		if(default_shader)
		{
			al_destroy_shader(default_shader);
			default_shader = NULL;
		}
		return false;
	}
}
