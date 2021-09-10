#include "t3f/t3f.h"
#include "t3gui/t3gui.h"
#include "modules/canvas/canvas_helpers.h"
#include "modules/queue.h"
#include "modules/canvas/flood_fill.h"
#include "ui/canvas_editor/canvas_editor.h"
#include "ui/canvas_editor/tools/tool_pixel.h"
#include "ui/canvas_editor/tools/tool_line.h"
#include "ui/canvas_editor/tools/tool_rectangle.h"
#include "ui/canvas_editor/tools/tool_filled_rectangle.h"
#include "ui/canvas_editor/tools/tool_oval.h"
#include "ui/canvas_editor/tools/tool_filled_oval.h"
#include "ui/canvas_editor/tools/tool_dropper.h"
#include "ui/canvas_editor/tools/tool_selection.h"
#include "ui/canvas_editor/undo/undo.h"
#include "ui/canvas_editor/undo/flood_fill.h"
#include "ui/canvas_editor/selection.h"
#include "ui/window.h"

static int mouse_button = 0;

static void click_on_canvas(PA_CANVAS_EDITOR * cep, int button, int x, int y)
{
	mouse_button = button;
	cep->click_x = x;
	cep->click_y = y;
	cep->click_button = button;
	if(button == 1)
	{
		cep->click_color = cep->left_color.color;
		cep->click_outline_color = cep->right_color.color;
	}
	else
	{
		cep->click_color = cep->right_color.color;
		cep->click_outline_color = cep->left_color.color;
	}
}

static bool create_flood_fill_undo(PA_CANVAS_EDITOR * cep, ALLEGRO_COLOR color, PA_QUEUE * qp)
{
	char undo_path[1024];

	pa_get_undo_path("undo", cep->undo_count, undo_path, 1024);
	if(pa_make_flood_fill_undo(cep, cep->current_layer, color, qp, undo_path))
	{
		return true;
	}
	return false;
}

void pa_canvas_editor_MSG_MOUSEDOWN(T3GUI_ELEMENT * d, int c)
{
	PA_CANVAS_EDITOR * canvas_editor = (PA_CANVAS_EDITOR *)d->dp;
	int tx, ty;
	PA_QUEUE * flood_fill_queue = NULL;
	ALLEGRO_COLOR color = t3f_color_black;
	bool made_undo = false;
	char buf[64];

	pa_update_mouse_variables(canvas_editor);

	d->flags |= D_TRACKMOUSE;
	if(canvas_editor->tool_state == PA_TOOL_STATE_OFF)
	{
		t3f_debug_message("Begin tool %d\n", canvas_editor->current_tool);
		click_on_canvas(canvas_editor, c, canvas_editor->hover_x, canvas_editor->hover_y);
		switch(canvas_editor->current_tool)
		{
			case PA_TOOL_PIXEL:
			{
				pa_initialize_tool_variables(canvas_editor);
				pa_tool_pixel_start(canvas_editor);
				canvas_editor->tool_state = PA_TOOL_STATE_DRAWING;
				break;
			}
			case PA_TOOL_LINE:
			{
				pa_initialize_tool_variables(canvas_editor);
				pa_tool_line_logic(canvas_editor);
				canvas_editor->tool_state = PA_TOOL_STATE_DRAWING;
				break;
			}
			case PA_TOOL_RECTANGLE:
			{
				pa_initialize_tool_variables(canvas_editor);
				pa_tool_rectangle_logic(canvas_editor);
				canvas_editor->tool_state = PA_TOOL_STATE_DRAWING;
				break;
			}
			case PA_TOOL_FILLED_RECTANGLE:
			{
				pa_initialize_tool_variables(canvas_editor);
				pa_tool_filled_rectangle_logic(canvas_editor);
				canvas_editor->tool_state = PA_TOOL_STATE_DRAWING;
				break;
			}
			case PA_TOOL_OVAL:
			{
				pa_initialize_tool_variables(canvas_editor);
				pa_tool_oval_logic(canvas_editor);
				canvas_editor->tool_state = PA_TOOL_STATE_DRAWING;
				break;
			}
			case PA_TOOL_FILLED_OVAL:
			{
				pa_initialize_tool_variables(canvas_editor);
				pa_tool_filled_oval_logic(canvas_editor);
				canvas_editor->tool_state = PA_TOOL_STATE_DRAWING;
				break;
			}
			case PA_TOOL_FLOOD_FILL:
			{
				pa_set_window_message("Processing flood fill...");
				tx = canvas_editor->hover_x / canvas_editor->canvas->bitmap_size;
				ty = canvas_editor->hover_y / canvas_editor->canvas->bitmap_size;
				if(tx >= 0 && tx < canvas_editor->canvas->layer_width && ty >= 0 && ty < canvas_editor->canvas->layer_height && canvas_editor->canvas->layer[canvas_editor->current_layer]->bitmap[ty][tx])
				{
					flood_fill_queue = pa_create_queue();
					if(flood_fill_queue)
					{
						color = pa_get_canvas_pixel(canvas_editor->canvas, canvas_editor->current_layer, canvas_editor->hover_x, canvas_editor->hover_y);
						if(pa_flood_fill_canvas(canvas_editor->canvas, canvas_editor->current_layer, canvas_editor->hover_x, canvas_editor->hover_y, c == 1 ? canvas_editor->left_color.color : canvas_editor->right_color.color, flood_fill_queue))
						{
							made_undo = create_flood_fill_undo(canvas_editor, color, flood_fill_queue);
							if(made_undo)
							{
								pa_finalize_undo(canvas_editor);
							}
							canvas_editor->modified++;
						}
						pa_destroy_queue(flood_fill_queue);
					}
				}
				break;
			}
			case PA_TOOL_DROPPER:
			{
				pa_tool_dropper_logic(canvas_editor, c);
				break;
			}
			case PA_TOOL_ERASER:
			{
				canvas_editor->click_color = al_map_rgba_f(0.0, 0.0, 0.0, 0.0);
				pa_initialize_tool_variables(canvas_editor);
				pa_tool_pixel_start(canvas_editor);
				canvas_editor->tool_state = PA_TOOL_STATE_DRAWING;
				break;
			}
			case PA_TOOL_SELECTION:
			{
				switch(canvas_editor->selection.box.state)
				{
					/* start creating a new selection if we are not currently
						 interacting with an existing selection */
					case PA_BOX_STATE_IDLE:
					{
						if(canvas_editor->selection.bitmap)
						{
							pa_unfloat_canvas_editor_selection(canvas_editor, &canvas_editor->selection.box);
						}
						pa_initialize_box(&canvas_editor->selection.box, canvas_editor->click_x, canvas_editor->click_y, 1, 1);
						canvas_editor->selection.box.hover_handle = 0;
						canvas_editor->selection.box.state = PA_BOX_STATE_DRAWING;
						canvas_editor->selection.layer = canvas_editor->current_layer;
						break;
					}
					/* set box moving logic in motion */
					case PA_BOX_STATE_HOVER:
					{
						canvas_editor->selection.box.click_start_x = canvas_editor->selection.box.start_x;
						canvas_editor->selection.box.click_start_y = canvas_editor->selection.box.start_y;
						canvas_editor->selection.box.click_x = canvas_editor->selection.box.hover_x;
						canvas_editor->selection.box.click_y = canvas_editor->selection.box.hover_y;
						canvas_editor->selection.box.state = PA_BOX_STATE_MOVING;
						break;
					}
					/* set box resizing logic in motion */
					case PA_BOX_STATE_HOVER_HANDLE:
					{
						if(canvas_editor->selection.bitmap)
						{
							pa_unfloat_canvas_editor_selection(canvas_editor, &canvas_editor->selection.box);
						}
						canvas_editor->selection.box.state = PA_BOX_STATE_RESIZING;
						break;
					}
				}
				canvas_editor->tool_state = PA_TOOL_STATE_EDITING;
				break;
			}
			case PA_TOOL_FRAME:
			{
				if(canvas_editor->hover_frame < 0)
				{
					sprintf(buf, "Frame %d", canvas_editor->frame_id);
					canvas_editor->frame_id++;
					if(pa_add_canvas_frame(canvas_editor->canvas, buf, canvas_editor->hover_x, canvas_editor->hover_y, 1, 1))
					{
						canvas_editor->hover_frame = canvas_editor->canvas->frame_max - 1;
						canvas_editor->current_frame = canvas_editor->hover_frame;
					}
				}
				if(canvas_editor->hover_frame >= 0)
				{
					switch(canvas_editor->canvas->frame[canvas_editor->hover_frame]->box.state)
					{
						/* start creating a new selection if we are not currently
							 interacting with an existing selection */
						case PA_BOX_STATE_IDLE:
						{
							pa_initialize_box(&canvas_editor->canvas->frame[canvas_editor->hover_frame]->box, canvas_editor->click_x, canvas_editor->click_y, 1, 1);
							canvas_editor->canvas->frame[canvas_editor->hover_frame]->box.hover_handle = 0;
							canvas_editor->canvas->frame[canvas_editor->hover_frame]->box.state = PA_BOX_STATE_DRAWING;
							break;
						}
						/* set box moving logic in motion */
						case PA_BOX_STATE_HOVER:
						{
							canvas_editor->canvas->frame[canvas_editor->hover_frame]->box.click_start_x = canvas_editor->canvas->frame[canvas_editor->hover_frame]->box.start_x;
							canvas_editor->canvas->frame[canvas_editor->hover_frame]->box.click_start_y = canvas_editor->canvas->frame[canvas_editor->hover_frame]->box.start_y;
							canvas_editor->canvas->frame[canvas_editor->hover_frame]->box.click_x = canvas_editor->canvas->frame[canvas_editor->hover_frame]->box.hover_x;
							canvas_editor->canvas->frame[canvas_editor->hover_frame]->box.click_y = canvas_editor->canvas->frame[canvas_editor->hover_frame]->box.hover_y;
							canvas_editor->canvas->frame[canvas_editor->hover_frame]->box.state = PA_BOX_STATE_MOVING;
							canvas_editor->current_frame = canvas_editor->hover_frame;
							break;
						}
						/* set box resizing logic in motion */
						case PA_BOX_STATE_HOVER_HANDLE:
						{
							canvas_editor->canvas->frame[canvas_editor->hover_frame]->box.state = PA_BOX_STATE_RESIZING;
							break;
						}
					}
					canvas_editor->tool_state = PA_TOOL_STATE_EDITING;
				}
				break;
			}
		}
	}
}
