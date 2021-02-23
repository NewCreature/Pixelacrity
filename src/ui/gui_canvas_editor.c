#include "t3f/t3f.h"
#include "canvas.h"
#include "canvas_editor.h"
#include "gui_canvas_editor.h"

int quixel_gui_canvas_editor_proc(int msg, T3GUI_ELEMENT * d, int c)
{
	QUIXEL_CANVAS_EDITOR * canvas_editor = (QUIXEL_CANVAS_EDITOR *)d->dp;

	switch(msg)
	{
		case MSG_MOUSEDOWN:
		{
			d->flags |= D_TRACKMOUSE;
			break;
		}
		case MSG_MOUSEUP:
		{
			d->flags &= ~D_TRACKMOUSE;
			break;
		}
		case MSG_IDLE:
		{
			quixel_canvas_editor_logic(canvas_editor);
			break;
		}
		case MSG_DRAW:
		{
			quixel_canvas_editor_render(canvas_editor);
			break;
		}
	}
	return D_O_K;
}
