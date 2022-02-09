#include "t3f/t3f.h"
#include "t3gui/t3gui.h"
#include "modules/canvas/canvas_helpers.h"
#include "modules/queue.h"
#include "modules/canvas/flood_fill.h"
#include "ui/canvas_editor/canvas_editor.h"
#include "ui/canvas_editor/tools/pixel.h"
#include "ui/canvas_editor/tools/line.h"
#include "ui/canvas_editor/tools/rectangle.h"
#include "ui/canvas_editor/tools/filled_rectangle.h"
#include "ui/canvas_editor/tools/oval.h"
#include "ui/canvas_editor/tools/filled_oval.h"
#include "ui/canvas_editor/tools/dropper.h"
#include "ui/canvas_editor/tools/selection.h"
#include "ui/canvas_editor/undo/undo.h"
#include "ui/canvas_editor/undo/flood_fill.h"
#include "ui/canvas_editor/undo/frame.h"
#include "ui/canvas_editor/selection.h"
#include "ui/canvas_editor/frame.h"
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

static bool create_flood_fill_undo(PA_CANVAS_EDITOR * cep, ALLEGRO_COLOR color, PA_QUEUE * qp, int shift_x, int shift_y)
{
	char undo_path[1024];

	pa_get_undo_path("undo", cep->undo_count, undo_path, 1024);
	if(pa_make_flood_fill_undo(cep, cep->current_layer, color, qp, shift_x, shift_y, undo_path))
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
	char undo_path[1024];
	int shift_x, shift_y;
	int left, right, top, bottom;
	int hover_frame;

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
				if(t3f_key[ALLEGRO_KEY_LSHIFT] || t3f_key[ALLEGRO_KEY_RSHIFT])
				{
					tx = canvas_editor->hover_x / canvas_editor->canvas->bitmap_size;
					ty = canvas_editor->hover_y / canvas_editor->canvas->bitmap_size;
					if(tx >= 0 && tx < canvas_editor->canvas->layer_width && ty >= 0 && ty < canvas_editor->canvas->layer_height && canvas_editor->canvas->layer[canvas_editor->current_layer]->bitmap[ty][tx])
					{
						flood_fill_queue = pa_create_queue("Flood Fill");
						if(flood_fill_queue)
						{
							color = pa_get_canvas_pixel(canvas_editor->canvas, canvas_editor->current_layer, canvas_editor->hover_x, canvas_editor->hover_y);
							if(pa_flood_fill_canvas(canvas_editor->canvas, canvas_editor->current_layer, canvas_editor->hover_x, canvas_editor->hover_y, c == 1 ? canvas_editor->left_color.color : canvas_editor->right_color.color, flood_fill_queue))
							{
								made_undo = create_flood_fill_undo(canvas_editor, color, flood_fill_queue, 0, 0);
								if(made_undo)
								{
									pa_finalize_undo(canvas_editor);
								}
								canvas_editor->modified++;
							}
							pa_destroy_queue(flood_fill_queue);
						}
					}
				}
				else if(t3f_key[ALLEGRO_KEY_LCTRL] || t3f_key[ALLEGRO_KEY_RCTRL] || t3f_key[ALLEGRO_KEY_COMMAND])
				{
					flood_fill_queue = pa_create_queue("Flood Fill");
					if(flood_fill_queue)
					{
						hover_frame = pa_get_hover_frame(canvas_editor);
						if(hover_frame >= 0)
						{
							left = canvas_editor->canvas->frame[hover_frame]->box.start_x;
							top = canvas_editor->canvas->frame[hover_frame]->box.start_y;
							right = left + canvas_editor->canvas->frame[hover_frame]->box.width;
							bottom = top + canvas_editor->canvas->frame[hover_frame]->box.height;
							color = pa_get_canvas_pixel(canvas_editor->canvas, canvas_editor->current_layer, canvas_editor->hover_x, canvas_editor->hover_y);
							pa_handle_canvas_expansion(canvas_editor->canvas, canvas_editor->view_x, canvas_editor->view_y, canvas_editor->view_x + canvas_editor->view_width, canvas_editor->view_y + canvas_editor->view_height, &shift_x, &shift_y);
							pa_shift_canvas_editor_variables(canvas_editor, shift_x * canvas_editor->canvas->bitmap_size, shift_y * canvas_editor->canvas->bitmap_size);
							left += shift_x * canvas_editor->canvas->bitmap_size;
							top += shift_y * canvas_editor->canvas->bitmap_size;
							right += shift_x * canvas_editor->canvas->bitmap_size;
							bottom += shift_y * canvas_editor->canvas->bitmap_size;
							if(pa_flood_fill_canvas_area(canvas_editor->canvas, canvas_editor->current_layer, canvas_editor->canvas->frame[hover_frame]->box.start_x, canvas_editor->canvas->frame[hover_frame]->box.start_y, canvas_editor->canvas->frame[hover_frame]->box.start_x + canvas_editor->canvas->frame[hover_frame]->box.width - 1, canvas_editor->canvas->frame[hover_frame]->box.start_y + canvas_editor->canvas->frame[hover_frame]->box.height - 1, canvas_editor->hover_x, canvas_editor->hover_y, c == 1 ? canvas_editor->left_color.color : canvas_editor->right_color.color, flood_fill_queue))
							{
								made_undo = create_flood_fill_undo(canvas_editor, color, flood_fill_queue, shift_x, shift_y);
								if(made_undo)
								{
									pa_finalize_undo(canvas_editor);
								}
								canvas_editor->modified++;
							}
							pa_destroy_queue(flood_fill_queue);
							pa_update_canvas_dimensions(canvas_editor->canvas, canvas_editor->current_layer, left, top, right, bottom);
						}
					}
				}
				else
				{
					flood_fill_queue = pa_create_queue("Flood Fill");
					if(flood_fill_queue)
					{
						left = canvas_editor->view_x;
						top = canvas_editor->view_y;
						right = canvas_editor->view_x + canvas_editor->view_width;
						bottom = canvas_editor->view_y + canvas_editor->view_height;
						color = pa_get_canvas_pixel(canvas_editor->canvas, canvas_editor->current_layer, canvas_editor->hover_x, canvas_editor->hover_y);
						pa_handle_canvas_expansion(canvas_editor->canvas, canvas_editor->view_x, canvas_editor->view_y, canvas_editor->view_x + canvas_editor->view_width, canvas_editor->view_y + canvas_editor->view_height, &shift_x, &shift_y);
						pa_shift_canvas_editor_variables(canvas_editor, shift_x * canvas_editor->canvas->bitmap_size, shift_y * canvas_editor->canvas->bitmap_size);
						left += shift_x * canvas_editor->canvas->bitmap_size;
						top += shift_y * canvas_editor->canvas->bitmap_size;
						right += shift_x * canvas_editor->canvas->bitmap_size;
						bottom += shift_y * canvas_editor->canvas->bitmap_size;
						if(pa_flood_fill_canvas_area(canvas_editor->canvas, canvas_editor->current_layer, canvas_editor->view_x, canvas_editor->view_y, canvas_editor->view_x + canvas_editor->view_width - 1, canvas_editor->view_y + canvas_editor->view_height - 1, canvas_editor->hover_x, canvas_editor->hover_y, c == 1 ? canvas_editor->left_color.color : canvas_editor->right_color.color, flood_fill_queue))
						{
							made_undo = create_flood_fill_undo(canvas_editor, color, flood_fill_queue, shift_x, shift_y);
							if(made_undo)
							{
								pa_finalize_undo(canvas_editor);
							}
							canvas_editor->modified++;
						}
						pa_destroy_queue(flood_fill_queue);
						pa_update_canvas_dimensions(canvas_editor->canvas, canvas_editor->current_layer, left, top, right, bottom);
					}
				}
				break;
			}
			case PA_TOOL_REPLACE:
			{
				pa_set_window_message("Processing color replace...");
				if(t3f_key[ALLEGRO_KEY_LCTRL] || t3f_key[ALLEGRO_KEY_RCTRL] || t3f_key[ALLEGRO_KEY_COMMAND])
				{
					flood_fill_queue = pa_create_queue("Replace Color");
					if(flood_fill_queue)
					{
						hover_frame = pa_get_hover_frame(canvas_editor);
						if(hover_frame >= 0)
						{
							left = canvas_editor->canvas->frame[hover_frame]->box.start_x;
							top = canvas_editor->canvas->frame[hover_frame]->box.start_y;
							right = left + canvas_editor->canvas->frame[hover_frame]->box.width;
							bottom = top + canvas_editor->canvas->frame[hover_frame]->box.height;
							color = pa_get_canvas_pixel(canvas_editor->canvas, canvas_editor->current_layer, canvas_editor->hover_x, canvas_editor->hover_y);
							if(pa_replace_canvas_color_area(canvas_editor->canvas, canvas_editor->current_layer, color, c == 1 ? canvas_editor->left_color.color : canvas_editor->right_color.color, left, top, right, bottom, flood_fill_queue))
							{
								made_undo = create_flood_fill_undo(canvas_editor, color, flood_fill_queue, 0, 0);
								if(made_undo)
								{
									pa_finalize_undo(canvas_editor);
								}
								canvas_editor->modified++;
							}
						}
						pa_destroy_queue(flood_fill_queue);
					}
				}
				else
				{
					flood_fill_queue = pa_create_queue("Replace Color");
					if(flood_fill_queue)
					{
						color = pa_get_canvas_pixel(canvas_editor->canvas, canvas_editor->current_layer, canvas_editor->hover_x, canvas_editor->hover_y);
						if(pa_replace_canvas_color(canvas_editor->canvas, canvas_editor->current_layer, color, c == 1 ? canvas_editor->left_color.color : canvas_editor->right_color.color, flood_fill_queue))
						{
							made_undo = create_flood_fill_undo(canvas_editor, color, flood_fill_queue, 0, 0);
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
						if(canvas_editor->selection.bitmap_stack)
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
						if(canvas_editor->selection.bitmap_stack)
						{
							//pa_unfloat_canvas_editor_selection(canvas_editor, &canvas_editor->selection.box);
						}
						if(canvas_editor->selection.box.handle[canvas_editor->selection.box.hover_handle].type == PA_BOX_HANDLE_TYPE_ANGLE)
						{
							canvas_editor->selection.box.state = PA_BOX_STATE_ROTATING;
						}
						else
						{
							canvas_editor->selection.box.state = PA_BOX_STATE_RESIZING;
						}
						break;
					}
				}
				canvas_editor->tool_state = PA_TOOL_STATE_EDITING;
				break;
			}
			case PA_TOOL_FRAME:
			{
				if(c == 1)
				{
					if(canvas_editor->hover_frame < 0)
					{
						pa_get_undo_path("undo", canvas_editor->undo_count, undo_path, 1024);
						if(pa_make_frame_undo(canvas_editor, "Create Frame", undo_path))
						{
							pa_finalize_undo(canvas_editor);
						}
						sprintf(buf, "Frame %d", canvas_editor->frame_id);
						canvas_editor->frame_id++;
						if(pa_add_canvas_frame(canvas_editor->canvas, buf, canvas_editor->hover_x, canvas_editor->hover_y, 1, 1))
						{
							canvas_editor->hover_frame = canvas_editor->canvas->frame_max - 1;
							canvas_editor->current_frame = canvas_editor->hover_frame;
							canvas_editor->modified++;
							pa_set_window_message(NULL);
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
								memcpy(&canvas_editor->click_box, &canvas_editor->canvas->frame[canvas_editor->hover_frame]->box, sizeof(PA_BOX));
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
								memcpy(&canvas_editor->click_box, &canvas_editor->canvas->frame[canvas_editor->hover_frame]->box, sizeof(PA_BOX));
								canvas_editor->canvas->frame[canvas_editor->hover_frame]->box.state = PA_BOX_STATE_RESIZING;
								canvas_editor->current_frame = canvas_editor->hover_frame;
								break;
							}
						}
						canvas_editor->tool_state = PA_TOOL_STATE_EDITING;
					}
				}
				else if(canvas_editor->current_frame < canvas_editor->canvas->frame_max)
				{
					int fx, fy, fw, fh;

					pa_get_undo_path("undo", canvas_editor->undo_count, undo_path, 1024);
					if(pa_make_frame_undo(canvas_editor, "Create Frame", undo_path))
					{
						pa_finalize_undo(canvas_editor);
					}
					sprintf(buf, "Frame %d", canvas_editor->frame_id);
					canvas_editor->frame_id++;
					fw = canvas_editor->canvas->frame[canvas_editor->current_frame]->box.width;
					fh = canvas_editor->canvas->frame[canvas_editor->current_frame]->box.height;
					fx = canvas_editor->hover_x - fw / 2;
					fy = canvas_editor->hover_y - fh / 2;
					if(pa_add_canvas_frame(canvas_editor->canvas, buf, fx, fy, fw, fh))
					{
						canvas_editor->hover_frame = canvas_editor->canvas->frame_max - 1;
						canvas_editor->current_frame = canvas_editor->hover_frame;
						canvas_editor->modified++;
						pa_set_window_message(NULL);
					}
				}
				break;
			}
		}
	}
}
