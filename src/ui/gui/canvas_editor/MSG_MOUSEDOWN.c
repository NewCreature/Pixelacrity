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
#include "ui/canvas_editor/undo_flood_fill.h"
#include "ui/canvas_editor/undo.h"
#include "ui/canvas_editor/selection.h"

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
	}
	else
	{
		cep->click_color = cep->right_color.color;
	}
}

static void set_up_tool_variables(PA_CANVAS_EDITOR * cep)
{
	cep->scratch_offset_x = cep->view_x;
	cep->scratch_offset_y = cep->view_y;
	cep->tool_top = cep->hover_y;
	cep->tool_bottom = cep->hover_y;
	cep->tool_left = cep->hover_x;
	cep->tool_right = cep->hover_x;
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

	d->flags |= D_TRACKMOUSE;
	if(canvas_editor->tool_state == PA_TOOL_STATE_OFF)
	{
		t3f_debug_message("Begin tool %d\n", canvas_editor->current_tool);
		click_on_canvas(canvas_editor, c, canvas_editor->hover_x, canvas_editor->hover_y);
		switch(canvas_editor->current_tool)
		{
			case PA_TOOL_PIXEL:
			{
				set_up_tool_variables(canvas_editor);
				pa_tool_pixel_start(canvas_editor);
				canvas_editor->tool_state = PA_TOOL_STATE_DRAWING;
				break;
			}
			case PA_TOOL_LINE:
			{
				set_up_tool_variables(canvas_editor);
				pa_tool_line_logic(canvas_editor);
				canvas_editor->tool_state = PA_TOOL_STATE_DRAWING;
				break;
			}
			case PA_TOOL_RECTANGLE:
			{
				set_up_tool_variables(canvas_editor);
				pa_tool_rectangle_logic(canvas_editor);
				canvas_editor->tool_state = PA_TOOL_STATE_DRAWING;
				break;
			}
			case PA_TOOL_FILLED_RECTANGLE:
			{
				set_up_tool_variables(canvas_editor);
				pa_tool_filled_rectangle_logic(canvas_editor);
				canvas_editor->tool_state = PA_TOOL_STATE_DRAWING;
				break;
			}
			case PA_TOOL_OVAL:
			{
				set_up_tool_variables(canvas_editor);
				pa_tool_oval_logic(canvas_editor);
				canvas_editor->tool_state = PA_TOOL_STATE_DRAWING;
				break;
			}
			case PA_TOOL_FILLED_OVAL:
			{
				set_up_tool_variables(canvas_editor);
				pa_tool_filled_oval_logic(canvas_editor);
				canvas_editor->tool_state = PA_TOOL_STATE_DRAWING;
				break;
			}
			case PA_TOOL_FLOOD_FILL:
			{
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
							canvas_editor->update_title = true;
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
				set_up_tool_variables(canvas_editor);
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
						pa_initialize_box(&canvas_editor->selection.box, canvas_editor->click_x, canvas_editor->click_y, 1, 1, canvas_editor->peg_bitmap);
						canvas_editor->selection.box.hover_handle = 0;
						canvas_editor->selection.box.state = PA_BOX_STATE_RESIZING;
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
		}
	}
}