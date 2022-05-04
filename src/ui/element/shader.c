#include "t3f/t3f.h"
#include "t3gui/t3gui.h"
#include "modules/pixel_shader.h"

int pa_gui_shader_proc(int msg, T3GUI_ELEMENT * d, int c)
{
	switch(msg)
	{
		case MSG_START:
		{
			if(!d->dp2)
			{
				d->dp2 = pa_create_pixel_shader(d->dp);
			}
			break;
		}
		case MSG_END:
		{
			if(d->dp2)
			{
				al_destroy_shader(d->dp2);
			}
			break;
		}
		case MSG_DRAW:
		{
			if(d->dp2)
			{
				al_use_shader(d->dp2);
			}
			break;
		}
	}
	return D_O_K;
}
