#include "t3f/t3f.h"
#include "MSG_GOTMOUSE.h"
#include "MSG_LOSTMOUSE.h"
#include "MSG_MOUSEDOWN.h"
#include "MSG_MOUSEUP.h"
#include "MSG_MOUSEMOVE.h"
#include "MSG_WHEEL.h"
#include "MSG_IDLE.h"
#include "MSG_DRAW.h"

int pa_gui_canvas_editor_proc(int msg, T3GUI_ELEMENT * d, int c)
{
	switch(msg)
	{
		case MSG_GOTMOUSE:
		{
			pa_canvas_editor_MSG_GOTMOUSE(d, c);
			break;
		}
		case MSG_LOSTMOUSE:
		{
			pa_canvas_editor_MSG_LOSTMOUSE(d, c);
			break;
		}
		case MSG_MOUSEDOWN:
		{
			pa_canvas_editor_MSG_MOUSEDOWN(d, c);
			break;
		}
		case MSG_MOUSEUP:
		{
			pa_canvas_editor_MSG_MOUSEUP(d, c);
			break;
		}
		case MSG_MOUSEMOVE:
		{
			pa_canvas_editor_MSG_MOUSEMOVE(d, c);
			break;
		}
		case MSG_WHEEL:
		{
			pa_canvas_editor_MSG_WHEEL(d, c);
			break;
		}
		case MSG_IDLE:
		{
			pa_canvas_editor_MSG_IDLE(d, c);
			break;
		}
		case MSG_DRAW:
		{
			pa_canvas_editor_MSG_DRAW(d, c);
			break;
		}
	}
	return D_O_K;
}
