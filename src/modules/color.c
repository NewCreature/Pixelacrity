#include "t3f/t3f.h"

bool quixel_color_equal(ALLEGRO_COLOR c1, ALLEGRO_COLOR c2)
{
	unsigned char r[2], g[2], b[2], a[2];

	al_unmap_rgba(c1, &r[0], &g[0], &b[0], &a[0]);
	al_unmap_rgba(c2, &r[1], &g[1], &b[1], &a[1]);
	if(r[0] != r[1] || g[0] != g[1] || b[0] != b[1] || a[0] != a[1])
	{
		return false;
	}
	return true;
}

ALLEGRO_COLOR quixel_shade_color(ALLEGRO_COLOR color, float l)
{
	float h, s, old_l;
	float r, g, b;

	al_unmap_rgb_f(color, &r, &g, &b);
	al_color_rgb_to_hsl(r, g, b, &h, &s, &old_l);

	return al_color_hsl(h, s, l);
}

ALLEGRO_COLOR quixel_alpha_color(ALLEGRO_COLOR color, float alpha)
{
	float r, g, b;

	al_unmap_rgb_f(color, &r, &g, &b);

	return al_map_rgba_f(r, g, b, alpha);
}

float quixel_get_color_shade(ALLEGRO_COLOR color)
{
	float r, g, b, h, s, l;

	al_unmap_rgb_f(color, &r, &g, &b);
	al_color_rgb_to_hsl(r, g, b, &h, &s, &l);

	return l;
}

float quixel_get_color_alpha(ALLEGRO_COLOR color)
{
	float r, a;

	al_unmap_rgba_f(color, &r, &r, &r, &a);

	return a;
}
