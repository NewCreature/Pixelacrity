#include "t3f/t3f.h"
#include "t3gui/t3gui.h"
#include "modules/canvas/canvas_helpers.h"
#include "modules/snap.h"
#include "modules/primitives.h"
#include "ui/canvas_editor/canvas_editor.h"
#include "ui/canvas_editor/undo/undo.h"
#include "ui/canvas_editor/undo/tool.h"
#include "ui/canvas_editor/undo/frame.h"
#include "ui/canvas_editor/tools/pixel.h"
#include "ui/canvas_editor/tools/line.h"
#include "ui/canvas_editor/tools/rectangle.h"
#include "ui/canvas_editor/tools/filled_rectangle.h"
#include "ui/canvas_editor/tools/oval.h"
#include "ui/canvas_editor/tools/filled_oval.h"
#include "ui/canvas_editor/tools/dropper.h"
#include "ui/canvas_editor/tools/selection.h"
#include "ui/canvas_editor/selection.h"
#include "ui/window.h"

static bool create_undo(PA_CANVAS_EDITOR * cep, const char * action, int x, int y, int width, int height)
{
	char undo_path[1024];

	pa_get_undo_path("undo", cep->undo_count, undo_path, 1024);
	if(pa_make_tool_undo(cep, action, cep->current_layer, x, y, width, height, undo_path))
	{
		return true;
	}
	return false;
}

static bool create_primitive_undo(PA_CANVAS_EDITOR * cep)
{
	return create_undo(cep, NULL, cep->tool_left, cep->tool_top, cep->tool_right - cep->tool_left + 1, cep->tool_bottom - cep->tool_top + 1);
}

static bool handle_canvas_expansion(PA_CANVAS_EDITOR * cep)
{
	bool ret;

	ret = pa_handle_canvas_expansion(cep->canvas, cep->tool_left, cep->tool_top, cep->tool_right, cep->tool_bottom, &cep->shift_x, &cep->shift_y);
	pa_shift_canvas_editor_variables(cep, cep->shift_x * cep->canvas->bitmap_size, cep->shift_y * cep->canvas->bitmap_size);

	return ret;
}

static bool make_frame_undo(PA_CANVAS_EDITOR * cep, const char * name)
{
	char undo_path[1024];

	pa_get_undo_path("undo", cep->undo_count, undo_path, 1024);
	if(pa_make_frame_undo(cep, name, undo_path))
	{
		pa_finalize_undo(cep);
		cep->modified++;
		pa_set_window_message(NULL);
		return true;
	}
	return false;
}

static bool box_moved(PA_BOX * updated_box, PA_BOX * original_box)
{
	if(updated_box->start_x != original_box->start_x || updated_box->start_y != original_box->start_y || updated_box->width != original_box->width || updated_box->height != original_box->height)
	{
		return true;
	}
	return false;
}

static bool finalize_frame_edit(PA_CANVAS_EDITOR * cep, const char * name)
{
	PA_BOX temp_box;

	cep->canvas->frame[cep->hover_frame]->box.state = PA_BOX_STATE_IDLE;
	if(box_moved(&cep->canvas->frame[cep->hover_frame]->box, &cep->click_box))
	{
		memcpy(&temp_box, &cep->canvas->frame[cep->hover_frame]->box, sizeof(PA_BOX));
		memcpy(&cep->canvas->frame[cep->hover_frame]->box, &cep->click_box, sizeof(PA_BOX));
		make_frame_undo(cep, name);
		memcpy(&cep->canvas->frame[cep->hover_frame]->box, &temp_box, sizeof(PA_BOX));
	}
	return true;
}

static int find_frame(PA_CANVAS * cp, PA_CANVAS_FRAME * fp)
{
	int i;

	for(i = 0; i < cp->frame_max; i++)
	{
		if(cp->frame[i] == fp)
		{
			return i;
		}
	}
	return 0;
}

void pa_canvas_editor_MSG_MOUSEUP(T3GUI_ELEMENT * d, int c)
{
	PA_CANVAS_EDITOR * canvas_editor = (PA_CANVAS_EDITOR *)d->dp;
	bool made_undo = false;
	PA_CANVAS_FRAME * old_frame = NULL;

	d->flags = d->flags & ~D_TRACKMOUSE;

	pa_update_mouse_variables(canvas_editor);
	pa_update_tool_variables(canvas_editor);
	canvas_editor->release_x = canvas_editor->hover_x;
	canvas_editor->release_y = canvas_editor->hover_y;
	if(canvas_editor->tool_state == PA_TOOL_STATE_DRAWING)
	{
		t3f_debug_message("Finish tool %d\n", canvas_editor->current_tool);
	}
	switch(canvas_editor->current_tool)
	{
		case PA_TOOL_PIXEL:
		case PA_TOOL_ERASER:
		{
			if(canvas_editor->tool_state == PA_TOOL_STATE_DRAWING && c == canvas_editor->click_button)
			{
				made_undo = create_primitive_undo(canvas_editor);
				handle_canvas_expansion(canvas_editor);
				pa_tool_pixel_finish(canvas_editor);
				if(made_undo)
				{
					pa_finalize_undo(canvas_editor);
				}
				canvas_editor->modified++;
				canvas_editor->preview->update = true;
				pa_set_window_message(NULL);
				canvas_editor->tool_state = PA_TOOL_STATE_OFF;
			}
			break;
		}
		case PA_TOOL_LINE:
		{
			canvas_editor->snap_end_x = canvas_editor->release_x;
			canvas_editor->snap_end_y = canvas_editor->release_y;

			if(t3f_key[ALLEGRO_KEY_LCTRL] || t3f_key[ALLEGRO_KEY_RCTRL])
			{
				pa_snap_coordinates(canvas_editor->click_x, canvas_editor->click_y, &canvas_editor->snap_end_x, &canvas_editor->snap_end_y, 0, ALLEGRO_PI / 16.0);
			}
			if(canvas_editor->tool_state == PA_TOOL_STATE_DRAWING && c == canvas_editor->click_button)
			{
				made_undo = create_primitive_undo(canvas_editor);
				handle_canvas_expansion(canvas_editor);
				pa_draw_primitive_to_canvas(canvas_editor->canvas, canvas_editor->current_layer, canvas_editor->click_x, canvas_editor->click_y, canvas_editor->snap_end_x, canvas_editor->snap_end_y, canvas_editor->brush, canvas_editor->click_color, NULL, PA_RENDER_COPY, canvas_editor->conditional_copy_shader, pa_draw_line);
				al_use_shader(canvas_editor->standard_shader);
				if(made_undo)
				{
					pa_finalize_undo(canvas_editor);
				}
				canvas_editor->modified++;
				canvas_editor->preview->update = true;
				pa_set_window_message(NULL);
				canvas_editor->tool_state = PA_TOOL_STATE_OFF;
			}
			break;
		}
		case PA_TOOL_RECTANGLE:
		{
			canvas_editor->snap_end_x = canvas_editor->release_x;
			canvas_editor->snap_end_y = canvas_editor->release_y;

			if(t3f_key[ALLEGRO_KEY_LCTRL] || t3f_key[ALLEGRO_KEY_RCTRL])
			{
				pa_snap_coordinates(canvas_editor->click_x, canvas_editor->click_y, &canvas_editor->snap_end_x, &canvas_editor->snap_end_y, 1, 0.0);
			}
			if(canvas_editor->tool_state == PA_TOOL_STATE_DRAWING && c == canvas_editor->click_button)
			{
				made_undo = create_primitive_undo(canvas_editor);
				handle_canvas_expansion(canvas_editor);
				pa_draw_primitive_to_canvas(canvas_editor->canvas, canvas_editor->current_layer, canvas_editor->click_x, canvas_editor->click_y, canvas_editor->snap_end_x, canvas_editor->snap_end_y, canvas_editor->brush, canvas_editor->click_color, NULL, PA_RENDER_COPY, canvas_editor->conditional_copy_shader, pa_draw_rectangle);
				al_use_shader(canvas_editor->standard_shader);
				if(made_undo)
				{
					pa_finalize_undo(canvas_editor);
				}
				canvas_editor->modified++;
				canvas_editor->preview->update = true;
				pa_set_window_message(NULL);
				canvas_editor->tool_state = PA_TOOL_STATE_OFF;
			}
			break;
		}
		case PA_TOOL_FILLED_RECTANGLE:
		{
			canvas_editor->snap_end_x = canvas_editor->release_x;
			canvas_editor->snap_end_y = canvas_editor->release_y;

			if(t3f_key[ALLEGRO_KEY_LCTRL] || t3f_key[ALLEGRO_KEY_RCTRL])
			{
				pa_snap_coordinates(canvas_editor->click_x, canvas_editor->click_y, &canvas_editor->snap_end_x, &canvas_editor->snap_end_y, 1, 0.0);
			}
			if(canvas_editor->tool_state == PA_TOOL_STATE_DRAWING && c == canvas_editor->click_button)
			{
				made_undo = create_primitive_undo(canvas_editor);
				handle_canvas_expansion(canvas_editor);
				pa_draw_primitive_to_canvas(canvas_editor->canvas, canvas_editor->current_layer, canvas_editor->click_x, canvas_editor->click_y, canvas_editor->snap_end_x, canvas_editor->snap_end_y, NULL, canvas_editor->click_color, NULL, PA_RENDER_COPY, canvas_editor->conditional_copy_shader, pa_draw_filled_rectangle);
				if(canvas_editor->tool_outline)
				{
					pa_draw_primitive_to_canvas(canvas_editor->canvas, canvas_editor->current_layer, canvas_editor->click_x, canvas_editor->click_y, canvas_editor->snap_end_x, canvas_editor->snap_end_y, canvas_editor->brush, canvas_editor->click_outline_color, NULL, PA_RENDER_COPY, canvas_editor->conditional_copy_shader, pa_draw_rectangle);
				}
				al_use_shader(canvas_editor->standard_shader);
				if(made_undo)
				{
					pa_finalize_undo(canvas_editor);
				}
				canvas_editor->modified++;
				canvas_editor->preview->update = true;
				pa_set_window_message(NULL);
				canvas_editor->tool_state = PA_TOOL_STATE_OFF;
			}
			break;
		}
		case PA_TOOL_OVAL:
		{
			canvas_editor->snap_end_x = canvas_editor->release_x;
			canvas_editor->snap_end_y = canvas_editor->release_y;

			if(t3f_key[ALLEGRO_KEY_LCTRL] || t3f_key[ALLEGRO_KEY_RCTRL])
			{
				pa_snap_coordinates(canvas_editor->click_x, canvas_editor->click_y, &canvas_editor->snap_end_x, &canvas_editor->snap_end_y, 1, 0.0);
			}
			if(canvas_editor->tool_state == PA_TOOL_STATE_DRAWING && c == canvas_editor->click_button)
			{
				made_undo = create_primitive_undo(canvas_editor);
				handle_canvas_expansion(canvas_editor);
				pa_draw_primitive_to_canvas(canvas_editor->canvas, canvas_editor->current_layer, canvas_editor->click_x, canvas_editor->click_y, canvas_editor->snap_end_x, canvas_editor->snap_end_y, canvas_editor->brush, canvas_editor->click_color, NULL, PA_RENDER_COPY, canvas_editor->conditional_copy_shader, pa_draw_oval);
				al_use_shader(canvas_editor->standard_shader);
				if(made_undo)
				{
					pa_finalize_undo(canvas_editor);
				}
				canvas_editor->modified++;
				canvas_editor->preview->update = true;
				pa_set_window_message(NULL);
				canvas_editor->tool_state = PA_TOOL_STATE_OFF;
			}
			break;
		}
		case PA_TOOL_FILLED_OVAL:
		{
			canvas_editor->snap_end_x = canvas_editor->release_x;
			canvas_editor->snap_end_y = canvas_editor->release_y;

			if(t3f_key[ALLEGRO_KEY_LCTRL] || t3f_key[ALLEGRO_KEY_RCTRL])
			{
				pa_snap_coordinates(canvas_editor->click_x, canvas_editor->click_y, &canvas_editor->snap_end_x, &canvas_editor->snap_end_y, 1, 0.0);
			}
			if(canvas_editor->tool_state == PA_TOOL_STATE_DRAWING && c == canvas_editor->click_button)
			{
				made_undo = create_primitive_undo(canvas_editor);
				handle_canvas_expansion(canvas_editor);
				pa_draw_primitive_to_canvas(canvas_editor->canvas, canvas_editor->current_layer, canvas_editor->click_x, canvas_editor->click_y, canvas_editor->snap_end_x, canvas_editor->snap_end_y, NULL, canvas_editor->click_color, NULL, PA_RENDER_COPY, canvas_editor->conditional_copy_shader, pa_draw_filled_oval);
				if(canvas_editor->tool_outline)
				{
					pa_draw_primitive_to_canvas(canvas_editor->canvas, canvas_editor->current_layer, canvas_editor->click_x, canvas_editor->click_y, canvas_editor->snap_end_x, canvas_editor->snap_end_y, canvas_editor->brush, canvas_editor->click_outline_color, NULL, PA_RENDER_COPY, canvas_editor->conditional_copy_shader, pa_draw_oval);
				}
				al_use_shader(canvas_editor->standard_shader);
				if(made_undo)
				{
					pa_finalize_undo(canvas_editor);
				}
				canvas_editor->modified++;
				canvas_editor->preview->update = true;
				pa_set_window_message(NULL);
				canvas_editor->tool_state = PA_TOOL_STATE_OFF;
			}
			break;
		}
		case PA_TOOL_SELECTION:
		{
			switch(canvas_editor->selection.box.state)
			{
				case PA_BOX_STATE_MOVING:
				case PA_BOX_STATE_DRAWING:
				case PA_BOX_STATE_RESIZING:
				case PA_BOX_STATE_ROTATING:
				{
					canvas_editor->selection.box.state = PA_BOX_STATE_IDLE;
					break;
				}
				default:
				{
					canvas_editor->selection.layer = canvas_editor->current_layer;
				}
			}
			canvas_editor->tool_state = PA_TOOL_STATE_OFF;
			if(canvas_editor->selection.box.width < 2 && canvas_editor->selection.box.height < 2)
			{
				pa_clear_canvas_editor_selection(canvas_editor);
			}
			t3f_refresh_menus();
			break;
		}
		case PA_TOOL_FRAME:
		{
			if(c == 1)
			{
				switch(canvas_editor->canvas->frame[canvas_editor->hover_frame]->box.state)
				{
					case PA_BOX_STATE_DRAWING:
					{
						canvas_editor->canvas->frame[canvas_editor->hover_frame]->box.state = PA_BOX_STATE_IDLE;
						break;
					}
					case PA_BOX_STATE_MOVING:
					{
						finalize_frame_edit(canvas_editor, "Move Frame");
						break;
					}
					case PA_BOX_STATE_RESIZING:
					{
						finalize_frame_edit(canvas_editor, "Resize Frame");
						break;
					}
					default:
					{
						canvas_editor->selection.layer = canvas_editor->current_layer;
					}
				}
				canvas_editor->tool_state = PA_TOOL_STATE_OFF;
				if(canvas_editor->canvas->frame[canvas_editor->hover_frame]->box.width < 2 && canvas_editor->canvas->frame[canvas_editor->hover_frame]->box.height < 2)
				{
	//				pa_clear_canvas_editor_selection(canvas_editor);
				}
				if(canvas_editor->current_frame < canvas_editor->canvas->frame_max)
				{
					old_frame = canvas_editor->canvas->frame[canvas_editor->current_frame];
				}
				pa_sort_canvas_frames(canvas_editor->canvas);
				if(canvas_editor->hover_frame == canvas_editor->current_frame)
				{
					canvas_editor->hover_frame = find_frame(canvas_editor->canvas, old_frame);
				}
				canvas_editor->current_frame = find_frame(canvas_editor->canvas, old_frame);
				t3f_refresh_menus();
			}
			break;
		}
	}
}
