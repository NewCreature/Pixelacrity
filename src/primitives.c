#include "t3f/t3f.h"

void quixel_draw_line(int x1, int y1, int x2, int y2, ALLEGRO_COLOR color)
{
	al_draw_pixel(x1, y1, color);
	al_draw_pixel(x2, y2, color);
	al_draw_line((float)x1 + 0.5, (float)y1 + 0.5, (float)x2 + 0.5, (float)y2 + 0.5, color, 1.0);
}

void quixel_draw_rectangle(int x1, int y1, int x2, int y2, ALLEGRO_COLOR color)
{
	int real_x1, real_x2, real_y1, real_y2;

	if(x1 < x2)
	{
		real_x1 = x1;
		real_x2 = x2;
	}
	else
	{
		real_x1 = x2;
		real_x2 = x1;
	}
	if(y1 < y2)
	{
		real_y1 = y1;
		real_y2 = y2;
	}
	else
	{
		real_y1 = y2;
		real_y2 = y1;
	}
	al_draw_rectangle((float)real_x1 + 0.5, (float)real_y1 + 0.5, (float)real_x2 + 0.5, (float)real_y2 + 0.5, color, 1.0);
}

void quixel_draw_filled_rectangle(int x1, int y1, int x2, int y2, ALLEGRO_COLOR color)
{
	int real_x1, real_x2, real_y1, real_y2;

	if(x1 < x2)
	{
		real_x1 = x1;
		real_x2 = x2;
	}
	else
	{
		real_x1 = x2;
		real_x2 = x1;
	}
	if(y1 < y2)
	{
		real_y1 = y1;
		real_y2 = y2;
	}
	else
	{
		real_y1 = y2;
		real_y2 = y1;
	}
	al_draw_rectangle((float)real_x1 + 0.5, (float)real_y1 + 0.5, (float)real_x2 + 0.5, (float)real_y2 + 0.5, color, 1.0);
	al_draw_filled_rectangle((float)real_x1 + 0.5, (float)real_y1 + 0.5, (float)real_x2 + 0.5, (float)real_y2 + 0.5, color);
}

void quixel_draw_oval(int x1, int y1, int x2, int y2, ALLEGRO_COLOR color)
{
	int real_x1, real_x2, real_y1, real_y2;
	float cx, cy, rx, ry;

	if(x1 < x2)
	{
		real_x1 = x1;
		real_x2 = x2;
	}
	else
	{
		real_x1 = x2;
		real_x2 = x1;
	}
	if(y1 < y2)
	{
		real_y1 = y1;
		real_y2 = y2;
	}
	else
	{
		real_y1 = y2;
		real_y2 = y1;
	}
	rx = (float)(real_x2 - real_x1) / 2.0;
	ry = (float)(real_y2 - real_y1) / 2.0;
	cx = real_x1 + rx + 0.5;
	cy = real_y1 + ry + 0.5;
	if(real_x1 == real_x2 || real_y1 == real_y2)
	{
		al_draw_pixel(real_x1, real_y1, color);
		al_draw_pixel(real_x2, real_y2, color);
		al_draw_line((float)real_x1 + 0.5, (float)real_y1 + 0.5, (float)real_x2 + 0.5, (float)real_y2 + 0.5, color, 1.0);
	}
	else if(abs(real_x1 - real_x2) < 2 || abs(real_y1 - real_y2) < 2)
	{
		al_draw_rectangle((float)real_x1 + 0.5, (float)real_y1 + 0.5, (float)real_x2 + 0.5, (float)real_y2 + 0.5, color, 1.0);
	}
	else
	{
		al_draw_ellipse(cx, cy, rx, ry, color, 1.0);
	}
}

void quixel_draw_filled_oval(int x1, int y1, int x2, int y2, ALLEGRO_COLOR color)
{

}
