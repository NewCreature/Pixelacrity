#include "t3f/t3f.h"
#include "t3gui/t3gui.h"
#include "modules/pixel_shader.h"

int pa_gui_shader_proc(int msg, T3GUI_ELEMENT * d, int c)
{
	switch(msg)
	{
		case MSG_START:
		{
			if(!d->dp4 && d->dp)
			{
				d->dp4 = pa_create_pixel_shader(d->dp);
			}
			break;
		}
		case MSG_END:
		{
			if(d->dp4)
			{
				al_destroy_shader(d->dp4);
			}
			break;
		}
		case MSG_DRAW:
		{
			if(d->dp2)
			{
				al_use_shader(d->dp2);
			}
			else if(d->dp4)
			{
				al_use_shader(d->dp4);
			}
			break;
		}
	}
	return D_O_K;
}
