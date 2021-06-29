#include "t3f/t3f.h"

typedef struct
{

	ALLEGRO_BITMAP * bitmap;
	ALLEGRO_COLOR color;

} BRUSH_DATA;

void pa_sort_coordinates(int * x1, int * x2)
{
	int x3;

	if(*x1 >= *x2)
	{
		x3 = *x1;
		*x1 = *x2;
		*x2 = x3;
	}
}

void pa_do_line(int x1, int y1, int x2, int y2, void * dp, void (*proc)(int, int, void *))
{
	int dx = x2 - x1;
	int dy = y2-y1;
	int i1, i2;
	int x, y;
	int dd;

	/* worker macro */
	#define DO_LINE(pri_sign, pri_c, pri_cond, sec_sign, sec_c, sec_cond)     \
	{                                                                         \
		if(d##pri_c == 0)                                                       \
		{                                                                       \
			proc(x1, y1, dp);                                                     \
			return;                                                               \
		}                                                                       \
		i1 = 2 * d##sec_c;                                                      \
		dd = i1 - (sec_sign (pri_sign d##pri_c));                               \
		i2 = dd - (sec_sign (pri_sign d##pri_c));                               \
		x = x1;                                                                 \
		y = y1;                                                                 \
		while (pri_c pri_cond pri_c##2)                                         \
		{                                                                       \
			proc(x, y, dp);                                                       \
			if(dd sec_cond 0)                                                     \
			{                                                                     \
				sec_c = sec_c sec_sign 1;                                           \
				dd += i2;                                                           \
			}                                                                     \
			else                                                                  \
			{                                                                     \
				dd += i1;                                                           \
				pri_c = pri_c pri_sign 1;                                           \
			}                                                                     \
		}                                                                       \
	}

	if(dx >= 0)
	{
		if(dy >= 0)
		{
			if(dx >= dy)
			{
				/* (x1 <= x2) && (y1 <= y2) && (dx >= dy) */
				DO_LINE(+, x, <=, +, y, >=);
			}
			else
			{
				/* (x1 <= x2) && (y1 <= y2) && (dx < dy) */
				DO_LINE(+, y, <=, +, x, >=);
			}
		}
		else
		{
			if(dx >= -dy)
			{
				/* (x1 <= x2) && (y1 > y2) && (dx >= dy) */
				DO_LINE(+, x, <=, -, y, <=);
			}
			else
			{
				/* (x1 <= x2) && (y1 > y2) && (dx < dy) */
				DO_LINE(-, y, >=, +, x, >=);
			}
		}
	}
	else
	{
		if (dy >= 0)
		{
			if (-dx >= dy)
			{
				/* (x1 > x2) && (y1 <= y2) && (dx >= dy) */
				DO_LINE(-, x, >=, +, y, >=);
			}
			else
			{
				/* (x1 > x2) && (y1 <= y2) && (dx < dy) */
				DO_LINE(+, y, <=, -, x, <=);
			}
		}
		else
		{
			if(-dx >= -dy)
			{
				/* (x1 > x2) && (y1 > y2) && (dx >= dy) */
				DO_LINE(-, x, >=, -, y, <=);
			}
			else
			{
				/* (x1 > x2) && (y1 > y2) && (dx < dy) */
				DO_LINE(-, y, >=, -, x, <=);
			}
		}
	}
	#undef DO_LINE
}

static void brush_draw(int x, int y, void * dp)
{
	BRUSH_DATA * brush_data = (BRUSH_DATA *)dp;

	al_draw_bitmap(brush_data->bitmap, x, y, 0);
}

void pa_draw_line(int x1, int y1, int x2, int y2, ALLEGRO_BITMAP * bp, ALLEGRO_COLOR color)
{
	BRUSH_DATA brush_data;

	if(bp)
	{
		brush_data.bitmap = bp;
		brush_data.color = color;
		pa_do_line(x1, y1, x2, y2, &brush_data, brush_draw);
	}
	else
	{
		al_draw_pixel(x1 + 0.5, y1 + 0.5, color);
		al_draw_pixel(x2 + 0.5, y2 + 0.5, color);
		al_draw_line((float)x1 + 0.5, (float)y1 + 0.5, (float)x2 + 0.5, (float)y2 + 0.5, color, 0.0);
	}
}

void pa_draw_rectangle(int x1, int y1, int x2, int y2, ALLEGRO_BITMAP * bp, ALLEGRO_COLOR color)
{
	pa_sort_coordinates(&x1, &x2);
	pa_sort_coordinates(&y1, &y2);
	al_draw_rectangle((float)x1 + 0.5, (float)y1 + 0.5, (float)x2 + 0.5, (float)y2 + 0.5, color, 0.0);
}

void pa_draw_filled_rectangle(int x1, int y1, int x2, int y2, ALLEGRO_BITMAP * bp, ALLEGRO_COLOR color)
{
	pa_sort_coordinates(&x1, &x2);
	pa_sort_coordinates(&y1, &y2);
	al_draw_rectangle((float)x1 + 0.5, (float)y1 + 0.5, (float)x2 + 0.5, (float)y2 + 0.5, color, 1.0);
	al_draw_filled_rectangle((float)x1 + 0.5, (float)y1 + 0.5, (float)x2 + 0.5, (float)y2 + 0.5, color);
}

void pa_draw_oval(int x1, int y1, int x2, int y2, ALLEGRO_BITMAP * bp, ALLEGRO_COLOR color)
{
	float cx, cy, rx, ry;

	pa_sort_coordinates(&x1, &x2);
	pa_sort_coordinates(&y1, &y2);
	rx = (float)(x2 - x1) / 2.0;
	ry = (float)(y2 - y1) / 2.0;
	cx = x1 + rx + 0.5;
	cy = y1 + ry + 0.5;
	if(x1 == x2 || y1 == y2)
	{
		al_draw_pixel(x1, y1, color);
		al_draw_pixel(x2, y2, color);
		al_draw_line((float)x1 + 0.5, (float)y1 + 0.5, (float)x2 + 0.5, (float)y2 + 0.5, color, 0.0);
	}
	else if(abs(x1 - x2) < 2 || abs(y1 - y2) < 2)
	{
		al_draw_rectangle((float)x1 + 0.5, (float)y1 + 0.5, (float)x2 + 0.5, (float)y2 + 0.5, color, 0.0);
	}
	else
	{
		al_draw_ellipse(cx, cy, rx, ry, color, 0.0);
	}
}

void pa_draw_filled_oval(int x1, int y1, int x2, int y2, ALLEGRO_BITMAP * bp, ALLEGRO_COLOR color)
{
	float cx, cy, rx, ry;

	pa_sort_coordinates(&x1, &x2);
	pa_sort_coordinates(&y1, &y2);
	rx = (float)(x2 - x1) / 2.0;
	ry = (float)(y2 - y1) / 2.0;
	cx = x1 + rx + 0.5;
	cy = y1 + ry + 0.5;
	if(x1 == x2 || y1 == y2)
	{
		al_draw_pixel(x1, y1, color);
		al_draw_pixel(x2, y2, color);
		al_draw_line((float)x1 + 0.5, (float)y1 + 0.5, (float)x2 + 0.5, (float)y2 + 0.5, color, 1.0);
	}
	else if(abs(x1 - x2) < 2 || abs(y1 - y2) < 2)
	{
		al_draw_rectangle((float)x1 + 0.5, (float)y1 + 0.5, (float)x2 + 0.5, (float)y2 + 0.5, color, 0.0);
	}
	else
	{
		al_draw_ellipse(cx, cy, rx, ry, color, 1.0);
		al_draw_filled_ellipse(cx, cy, rx, ry, color);
	}
}

void pa_draw_quad(int x1, int y1, int x2, int y2, ALLEGRO_BITMAP * bp, ALLEGRO_COLOR color)
{
	pa_sort_coordinates(&x1, &x2);
	pa_sort_coordinates(&y1, &y2);
	al_draw_bitmap_region(bp, 0, 0, x2 - x1, y2 - y1, x1, y1, 0);
}
