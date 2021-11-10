#include "t3f/t3f.h"

bool pa_color_equal(ALLEGRO_COLOR c1, ALLEGRO_COLOR c2)
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

ALLEGRO_COLOR pa_shade_color(ALLEGRO_COLOR color, float l)
{
	float h, s, old_l;
	float r, g, b;

	al_unmap_rgb_f(color, &r, &g, &b);
	al_color_rgb_to_hsl(r, g, b, &h, &s, &old_l);

	return al_color_hsl(h, s, l);
}

ALLEGRO_COLOR pa_alpha_color(ALLEGRO_COLOR color, float alpha)
{
	float r, g, b;

	al_unmap_rgb_f(color, &r, &g, &b);

	return al_map_rgba_f(r, g, b, alpha);
}

float pa_get_color_shade(ALLEGRO_COLOR color)
{
	float r, g, b, h, s, l;

	al_unmap_rgb_f(color, &r, &g, &b);
	al_color_rgb_to_hsl(r, g, b, &h, &s, &l);

	return l;
}

float pa_get_color_alpha(ALLEGRO_COLOR color)
{
	float r, a;

	al_unmap_rgba_f(color, &r, &r, &r, &a);

	return a;
}

void pa_print_color(ALLEGRO_COLOR color)
{
	unsigned char r, g, b, a;

	al_unmap_rgba(color, &r, &g, &b, &a);
	printf("%d, %d, %d, %d\n", r, g, b, a);
}

ALLEGRO_COLOR pa_get_real_color(ALLEGRO_COLOR color, ALLEGRO_BITMAP * scratch)
{
	ALLEGRO_TRANSFORM identity;
	ALLEGRO_STATE old_state;

	al_store_state(&old_state, ALLEGRO_STATE_TARGET_BITMAP | ALLEGRO_STATE_TRANSFORM | ALLEGRO_STATE_BLENDER);
	al_set_target_bitmap(scratch);
	al_identity_transform(&identity);
	al_use_transform(&identity);
	al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);
	al_draw_filled_rectangle(0, 0, al_get_bitmap_width(scratch), al_get_bitmap_height(scratch), color);
	al_restore_state(&old_state);

	return al_get_pixel(scratch, 0, 0);
}

ALLEGRO_COLOR pa_get_color_from_html(const char * html)
{
	char buf[3] = {0};
	int i, l;
	int c[4] = {255, 255, 255, 255};

	l = strlen(html);
	for(i = 0; i < 4; i++)
	{
		if(i * 2 + 1 < l)
		{
			buf[0] = html[i * 2];
			buf[1] = html[i * 2 + 1];
			c[i] = strtol(buf, NULL, 16);
		}
	}
	return al_map_rgba(c[0], c[1], c[2], c[3]);
}

void pa_color_to_html(ALLEGRO_COLOR color, char * buffer)
{
	unsigned char r, g, b, a;

	al_unmap_rgba(color, &r, &g, &b, &a);
	sprintf(buffer, "%02X%02X%02X%02X", r, g, b, a);
}
