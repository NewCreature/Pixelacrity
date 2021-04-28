#include "t3f/t3f.h"

void quixel_sort_coordinates(int * x1, int * x2)
{
	int x3;

	if(*x1 >= *x2)
	{
		x3 = *x1;
		*x1 = *x2;
		*x2 = x3;
	}
}

void quixel_draw_line(int x1, int y1, int x2, int y2, ALLEGRO_BITMAP * bp, ALLEGRO_COLOR color)
{
	al_draw_pixel(x1 + 0.5, y1 + 0.5, color);
	al_draw_pixel(x2 + 0.5, y2 + 0.5, color);
	al_draw_line((float)x1 + 0.5, (float)y1 + 0.5, (float)x2 + 0.5, (float)y2 + 0.5, color, 1.0);
}

void quixel_draw_rectangle(int x1, int y1, int x2, int y2, ALLEGRO_BITMAP * bp, ALLEGRO_COLOR color)
{
	quixel_sort_coordinates(&x1, &x2);
	quixel_sort_coordinates(&y1, &y2);
	al_draw_rectangle((float)x1 + 0.5, (float)y1 + 0.5, (float)x2 + 0.5, (float)y2 + 0.5, color, 1.0);
}

void quixel_draw_filled_rectangle(int x1, int y1, int x2, int y2, ALLEGRO_BITMAP * bp, ALLEGRO_COLOR color)
{
	quixel_sort_coordinates(&x1, &x2);
	quixel_sort_coordinates(&y1, &y2);
	al_draw_rectangle((float)x1 + 0.5, (float)y1 + 0.5, (float)x2 + 0.5, (float)y2 + 0.5, color, 1.0);
	al_draw_filled_rectangle((float)x1 + 0.5, (float)y1 + 0.5, (float)x2 + 0.5, (float)y2 + 0.5, color);
}

void quixel_draw_oval(int x1, int y1, int x2, int y2, ALLEGRO_BITMAP * bp, ALLEGRO_COLOR color)
{
	float cx, cy, rx, ry;

	quixel_sort_coordinates(&x1, &x2);
	quixel_sort_coordinates(&y1, &y2);
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
		al_draw_rectangle((float)x1 + 0.5, (float)y1 + 0.5, (float)x2 + 0.5, (float)y2 + 0.5, color, 1.0);
	}
	else
	{
		al_draw_ellipse(cx, cy, rx, ry, color, 1.0);
	}
}

void quixel_draw_filled_oval(int x1, int y1, int x2, int y2, ALLEGRO_BITMAP * bp, ALLEGRO_COLOR color)
{
	float cx, cy, rx, ry;

	quixel_sort_coordinates(&x1, &x2);
	quixel_sort_coordinates(&y1, &y2);
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
		al_draw_rectangle((float)x1 + 0.5, (float)y1 + 0.5, (float)x2 + 0.5, (float)y2 + 0.5, color, 1.0);
	}
	else
	{
		al_draw_ellipse(cx, cy, rx, ry, color, 1.0);
		al_draw_filled_ellipse(cx, cy, rx, ry, color);
	}
}

void quixel_draw_quad(int x1, int y1, int x2, int y2, ALLEGRO_BITMAP * bp, ALLEGRO_COLOR color)
{
	quixel_sort_coordinates(&x1, &x2);
	quixel_sort_coordinates(&y1, &y2);
	al_draw_bitmap_region(bp, 0, 0, x2 - x1, y2 - y1, x1, y1, 0);
}
