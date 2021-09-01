#include "t3f/t3f.h"
#include "modules/box.h"
#include "ui/canvas_editor/canvas_editor.h"

static void update_hover_frame(PA_CANVAS_EDITOR * cep)
{
	int i;
	int old_hover_frame = cep->hover_frame;

	cep->hover_frame = -1;
	for(i = 0; i < cep->canvas->frame_max; i++)
	{
		pa_setup_box(&cep->canvas->frame[i]->box, cep->canvas->frame[i]->box.start_x, cep->canvas->frame[i]->box.start_y, cep->canvas->frame[i]->box.width, cep->canvas->frame[i]->box.height);
		if(cep->hover_x >= cep->canvas->frame[i]->box.start_x && cep->hover_x <= cep->canvas->frame[i]->box.end_x && cep->hover_y >= cep->canvas->frame[i]->box.start_y && cep->hover_y <= cep->canvas->frame[i]->box.end_y)
		{
			cep->hover_frame = i;
		}
	}
	if(cep->hover_frame >= 0)
	{
		pa_update_box_handles(&cep->canvas->frame[cep->hover_frame]->box, cep->view_x, cep->view_y, cep->view_zoom);
		if(cep->hover_frame == old_hover_frame)
		{
			if(cep->hover_frame_time < PA_FRAME_HOVER_THRESHOLD)
			{
				cep->hover_frame_time++;
			}
			else
			{
				cep->hover_frame_time = PA_FRAME_DEHOVER_THRESHOLD;
			}
		}
	}
	else
	{
		if(cep->hover_frame_time > 0)
		{
			cep->hover_frame = old_hover_frame;
			cep->hover_frame_time--;
		}
	}
}

void pa_update_hover_frame(PA_CANVAS_EDITOR * cep, T3GUI_ELEMENT * d)
{
	PA_BOX old_box;
	bool snap = false;

	update_hover_frame(cep);
	if(cep->hover_frame >= 0)
	{
		memcpy(&old_box, &cep->canvas->frame[cep->hover_frame]->box, sizeof(PA_BOX));
		pa_update_box_handles(&cep->canvas->frame[cep->hover_frame]->box, cep->view_x, cep->view_y, cep->view_zoom);
		if(t3f_key[ALLEGRO_KEY_LCTRL] || t3f_key[ALLEGRO_KEY_RCTRL])
		{
			snap = true;
		}
		pa_box_logic(&cep->canvas->frame[cep->hover_frame]->box, cep->view_x, cep->view_y, cep->view_zoom, d->x, d->y, snap, cep->peg_bitmap);
		if(cep->canvas->frame[cep->hover_frame]->box.state == PA_BOX_STATE_HOVER_HANDLE)
		{
			if(cep->current_tool != PA_TOOL_FRAME_EDIT)
			{
				cep->last_tool = cep->current_tool;
				pa_select_canvas_editor_tool(cep, PA_TOOL_FRAME_EDIT);
			}
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
			if(cep->current_tool == PA_TOOL_FRAME_EDIT)
			{
				pa_select_canvas_editor_tool(cep, cep->last_tool);
			}
			cep->want_cursor = ALLEGRO_SYSTEM_MOUSE_CURSOR_PRECISION;
		}
	}
	else
	{
		if(cep->current_tool == PA_TOOL_FRAME_EDIT)
		{
			pa_select_canvas_editor_tool(cep, cep->last_tool);
		}
	}
}