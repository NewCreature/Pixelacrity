#include "t3f/t3f.h"
#include "modules/box.h"
#include "ui/canvas_editor/canvas_editor.h"

int pa_get_hover_frame(PA_CANVAS_EDITOR * cep)
{
	int i;

	for(i = 0; i < cep->canvas->frame_max; i++)
	{
		pa_setup_box(&cep->canvas->frame[i]->box, cep->canvas->frame[i]->box.start_x, cep->canvas->frame[i]->box.start_y, cep->canvas->frame[i]->box.width, cep->canvas->frame[i]->box.height);
		pa_update_box_handles(&cep->canvas->frame[i]->box, cep->view_x, cep->view_y, cep->view_zoom, cep->selection.bitmap_stack);
		pa_get_box_hover_handle(&cep->canvas->frame[i]->box, cep->editor_element->x, cep->editor_element->y, al_get_bitmap_width(cep->peg_bitmap) / 2);
		if(cep->canvas->frame[i]->box.hover_handle >= 0)
		{
			return i;
		}
		else if(cep->hover_x >= cep->canvas->frame[i]->box.start_x && cep->hover_x <= cep->canvas->frame[i]->box.end_x && cep->hover_y >= cep->canvas->frame[i]->box.start_y && cep->hover_y <= cep->canvas->frame[i]->box.end_y)
		{
			return i;
		}
	}
	return -1;
}

void pa_update_hover_frame(PA_CANVAS_EDITOR * cep, T3GUI_ELEMENT * d)
{
	PA_BOX old_box;
	bool snap = false;

	if(cep->current_tool == PA_TOOL_FRAME && cep->tool_state == PA_TOOL_STATE_OFF)
	{
		cep->hover_frame = pa_get_hover_frame(cep);
	}
	else if(cep->current_tool == PA_TOOL_FRAME)
	{
		pa_update_canvas_editor_box_handles(cep);
	}
	else
	{
		cep->hover_frame = -1;
	}
	if(cep->hover_frame >= 0)
	{
		if(cep->canvas->frame[cep->hover_frame]->box.width > 0 && cep->canvas->frame[cep->hover_frame]->box.height > 0)
		{
			memcpy(&old_box, &cep->canvas->frame[cep->hover_frame]->box, sizeof(PA_BOX));
			pa_update_box_handles(&cep->canvas->frame[cep->hover_frame]->box, cep->view_x, cep->view_y, cep->view_zoom, cep->selection.bitmap_stack);
			if(t3f_key[ALLEGRO_KEY_LCTRL] || t3f_key[ALLEGRO_KEY_RCTRL])
			{
				snap = true;
			}
			pa_box_logic(&cep->canvas->frame[cep->hover_frame]->box, cep->view_x, cep->view_y, cep->view_zoom, d->x, d->y, snap, cep->peg_bitmap, cep->selection.bitmap_stack);
			if(cep->canvas->frame[cep->hover_frame]->box.state == PA_BOX_STATE_HOVER)
			{
				cep->want_cursor = ALLEGRO_SYSTEM_MOUSE_CURSOR_LINK;
			}
			else if(cep->canvas->frame[cep->hover_frame]->box.state == PA_BOX_STATE_HOVER_HANDLE)
			{
				cep->want_cursor = ALLEGRO_SYSTEM_MOUSE_CURSOR_LINK;
			}
			else if(cep->canvas->frame[cep->hover_frame]->box.state == PA_BOX_STATE_MOVING)
			{
				cep->want_cursor = ALLEGRO_SYSTEM_MOUSE_CURSOR_MOVE;
			}
			else if(cep->canvas->frame[cep->hover_frame]->box.state == PA_BOX_STATE_RESIZING)
			{
				cep->want_cursor = ALLEGRO_SYSTEM_MOUSE_CURSOR_LINK;
			}
			else
			{
				cep->want_cursor = ALLEGRO_SYSTEM_MOUSE_CURSOR_PRECISION;
			}
		}
	}
	else
	{
		cep->want_cursor = ALLEGRO_SYSTEM_MOUSE_CURSOR_PRECISION;
	}
}
