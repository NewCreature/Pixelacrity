#include "t3f/t3f.h"

ALLEGRO_SHADER * quixel_create_pixel_shader(const char * fn)
{
	ALLEGRO_SHADER * shader;

	shader = al_create_shader(ALLEGRO_SHADER_AUTO);
	if(!shader)
	{
		return NULL;
	}

	if(!al_attach_shader_source(shader, ALLEGRO_VERTEX_SHADER, al_get_default_shader_source(ALLEGRO_SHADER_AUTO, ALLEGRO_VERTEX_SHADER)))
	{
		return NULL;
	}

	if(!al_attach_shader_source_file(shader, ALLEGRO_PIXEL_SHADER, fn))
	{
		return NULL;
	}
	if(!al_build_shader(shader))
	{
		return NULL;
	}

	return shader;
}
