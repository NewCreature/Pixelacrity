#include "t3f/t3f.h"

void pa_snap_coordinates(float start_x, float start_y, float * end_x, float * end_y, int type, float interval)
{
	float angle, length, a, b;
	float dx, dy;
	float size;

	switch(type)
	{
		case 0:
		{
			a = *end_x - start_x;
			b = *end_y - start_y;
			angle = atan2(b, a) + ALLEGRO_PI * 2.0;
			angle = angle + interval / 2.0 - fmod(angle + interval / 2.0, interval);
			length = sqrt(a * a + b * b);
			*end_x = start_x + (cos(angle) * length) + 0.5;
			*end_y = start_y + (sin(angle) * length) + 0.5;
			break;
		}
		case 1:
		{
			dx = *end_x - start_x;
			dy = *end_y - start_y;
			if(fabs(dx) > fabs(dy))
			{
				size = fabs(dx);
			}
			else
			{
				size = fabs(dy);
			}
			*end_x = start_x + size * (dx < 0 ? -1.0 : 1.0);
			*end_y = start_y + size * (dy < 0 ? -1.0 : 1.0);
			break;
		}
	}
}
