#include "t3f/t3f.h"

void pa_snap_coordinates(float start_x, float start_y, float * end_x, float * end_y, int type, float interval)
{
	float angle, length, a, b;

	switch(type)
	{
		case 0:
		{
			a = *end_x - start_x;
			b = *end_y - start_y;
			angle = atan2(b, a);
			angle = angle - interval / 2.0 - fmod(angle - interval / 2.0, interval);
			length = sqrt(a * a + b * b);
			*end_x = start_x + (cos(angle) * length) + 0.5;
			*end_y = start_y + (sin(angle) * length) + 0.5;
			break;
		}
	}
}
