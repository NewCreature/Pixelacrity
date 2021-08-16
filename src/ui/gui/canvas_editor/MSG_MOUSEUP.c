#include "t3f/t3f.h"
#include "t3gui/t3gui.h"
#include "modules/canvas/canvas_helpers.h"
#include "modules/snap.h"
#include "modules/primitives.h"
#include "ui/canvas_editor/canvas_editor.h"
#include "ui/canvas_editor/undo.h"
#include "ui/canvas_editor/undo_tool.h"
#include "ui/canvas_editor/tools/tool_pixel.h"
#include "ui/canvas_editor/tools/tool_line.h"
#include "ui/canvas_editor/tools/tool_rectangle.h"
#include "ui/canvas_editor/tools/tool_filled_rectangle.h"
#include "ui/canvas_editor/tools/tool_oval.h"
#include "ui/canvas_editor/tools/tool_filled_oval.h"
#include "ui/canvas_editor/tools/tool_dropper.h"
#include "ui/canvas_editor/tools/tool_selection.h"
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

static void revert_undo(PA_CANVAS_EDITOR * cep)
{
	char undo_path[1024];

	pa_get_undo_path("undo", cep->undo_count, undo_path, 1024);
	pa_apply_undo(cep, undo_path, true);
}

static bool handle_canvas_expansion(PA_CANVAS_EDITOR * cep)
{
	bool ret;

	ret = pa_handle_canvas_expansion(cep->canvas, cep->tool_left, cep->tool_top, cep->tool_right, cep->tool_bottom, &cep->shift_x, &cep->shift_y);
	pa_shift_canvas_editor_variables(cep, cep->shift_x * cep->canvas->bitmap_size, cep->shift_y * cep->canvas->bitmap_size);

	return ret;
}

void pa_canvas_editor_MSG_MOUSEUP(T3GUI_ELEMENT * d, int c)
{
	PA_CANVAS_EDITOR * canvas_editor = (PA_CANVAS_EDITOR *)d->dp;
	bool made_undo = false;

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
				pa_set_window_message(NULL);
				canvas_editor->tool_state = PA_TOOL_STATE_OFF;
			}
			break;
		}
		case PA_TOOL_LINE:
		{
			float start_x = canvas_editor->click_x;
			float start_y = canvas_editor->click_y;
			float end_x = canvas_editor->release_x;
			float end_y = canvas_editor->release_y;

			if(t3f_key[ALLEGRO_KEY_LCTRL] || t3f_key[ALLEGRO_KEY_RCTRL])
			{
				pa_snap_coordinates(start_x, start_y, &end_x, &end_y, 0, ALLEGRO_PI / 16.0);
			}
			if(canvas_editor->tool_state == PA_TOOL_STATE_DRAWING && c == canvas_editor->click_button)
			{
				made_undo = create_primitive_undo(canvas_editor);
				handle_canvas_expansion(canvas_editor);
				pa_draw_primitive_to_canvas(canvas_editor->canvas, canvas_editor->current_layer, start_x, start_y, end_x, end_y, canvas_editor->brush, canvas_editor->click_color, PA_RENDER_COPY, canvas_editor->conditional_copy_shader, pa_draw_line);
				al_use_shader(canvas_editor->standard_shader);
				if(made_undo)
				{
					pa_finalize_undo(canvas_editor);
				}
				canvas_editor->modified++;
				pa_set_window_message(NULL);
				canvas_editor->tool_state = PA_TOOL_STATE_OFF;
			}
			break;
		}
		case PA_TOOL_RECTANGLE:
		{
			float start_x = canvas_editor->click_x;
			float start_y = canvas_editor->click_y;
			float end_x = canvas_editor->release_x;
			float end_y = canvas_editor->release_y;

			if(t3f_key[ALLEGRO_KEY_LCTRL] || t3f_key[ALLEGRO_KEY_RCTRL])
			{
				pa_snap_coordinates(start_x, start_y, &end_x, &end_y, 1, 0.0);
			}
			if(canvas_editor->tool_state == PA_TOOL_STATE_DRAWING && c == canvas_editor->click_button)
			{
				made_undo = create_primitive_undo(canvas_editor);
				handle_canvas_expansion(canvas_editor);
				pa_draw_primitive_to_canvas(canvas_editor->canvas, canvas_editor->current_layer, start_x, start_y, end_x, end_y, NULL, canvas_editor->click_color, PA_RENDER_COPY, canvas_editor->conditional_copy_shader, pa_draw_rectangle);
				al_use_shader(canvas_editor->standard_shader);
				if(made_undo)
				{
					pa_finalize_undo(canvas_editor);
				}
				canvas_editor->modified++;
				pa_set_window_message(NULL);
				canvas_editor->tool_state = PA_TOOL_STATE_OFF;
			}
			break;
		}
		case PA_TOOL_FILLED_RECTANGLE:
		{
			float start_x = canvas_editor->click_x;
			float start_y = canvas_editor->click_y;
			float end_x = canvas_editor->release_x;
			float end_y = canvas_editor->release_y;

			if(t3f_key[ALLEGRO_KEY_LCTRL] || t3f_key[ALLEGRO_KEY_RCTRL])
			{
				pa_snap_coordinates(start_x, start_y, &end_x, &end_y, 1, 0.0);
			}
			if(canvas_editor->tool_state == PA_TOOL_STATE_DRAWING && c == canvas_editor->click_button)
			{
				made_undo = create_primitive_undo(canvas_editor);
				handle_canvas_expansion(canvas_editor);
				pa_draw_primitive_to_canvas(canvas_editor->canvas, canvas_editor->current_layer, start_x, start_y, end_x, end_y, NULL, canvas_editor->click_color, PA_RENDER_COPY, canvas_editor->conditional_copy_shader, pa_draw_filled_rectangle);
				pa_draw_primitive_to_canvas(canvas_editor->canvas, canvas_editor->current_layer, start_x, start_y, end_x, end_y, NULL, canvas_editor->click_outline_color, PA_RENDER_COPY, canvas_editor->conditional_copy_shader, pa_draw_rectangle);
				al_use_shader(canvas_editor->standard_shader);
				if(made_undo)
				{
					pa_finalize_undo(canvas_editor);
				}
				canvas_editor->modified++;
				pa_set_window_message(NULL);
				canvas_editor->tool_state = PA_TOOL_STATE_OFF;
			}
			break;
		}
		case PA_TOOL_OVAL:
		{
			float start_x = canvas_editor->click_x;
			float start_y = canvas_editor->click_y;
			float end_x = canvas_editor->release_x;
			float end_y = canvas_editor->release_y;

			if(t3f_key[ALLEGRO_KEY_LCTRL] || t3f_key[ALLEGRO_KEY_RCTRL])
			{
				pa_snap_coordinates(start_x, start_y, &end_x, &end_y, 1, 0.0);
			}
			if(canvas_editor->tool_state == PA_TOOL_STATE_DRAWING && c == canvas_editor->click_button)
			{
				made_undo = create_primitive_undo(canvas_editor);
				handle_canvas_expansion(canvas_editor);
				pa_draw_primitive_to_canvas(canvas_editor->canvas, canvas_editor->current_layer, start_x, start_y, end_x, end_y, canvas_editor->brush, canvas_editor->click_color, PA_RENDER_COPY, canvas_editor->conditional_copy_shader, pa_draw_oval);
				al_use_shader(canvas_editor->standard_shader);
				if(made_undo)
				{
					pa_finalize_undo(canvas_editor);
				}
				canvas_editor->modified++;
				pa_set_window_message(NULL);
				canvas_editor->tool_state = PA_TOOL_STATE_OFF;
			}
			break;
		}
		case PA_TOOL_FILLED_OVAL:
		{
			float start_x = canvas_editor->click_x;
			float start_y = canvas_editor->click_y;
			float end_x = canvas_editor->release_x;
			float end_y = canvas_editor->release_y;

			if(t3f_key[ALLEGRO_KEY_LCTRL] || t3f_key[ALLEGRO_KEY_RCTRL])
			{
				pa_snap_coordinates(start_x, start_y, &end_x, &end_y, 1, 0.0);
			}
			if(canvas_editor->tool_state == PA_TOOL_STATE_DRAWING && c == canvas_editor->click_button)
			{
				made_undo = create_primitive_undo(canvas_editor);
				handle_canvas_expansion(canvas_editor);
				pa_draw_primitive_to_canvas(canvas_editor->canvas, canvas_editor->current_layer, start_x, start_y, end_x, end_y, NULL, canvas_editor->click_color, PA_RENDER_COPY, canvas_editor->conditional_copy_shader, pa_draw_filled_oval);
				pa_draw_primitive_to_canvas(canvas_editor->canvas, canvas_editor->current_layer, start_x, start_y, end_x, end_y, NULL, canvas_editor->click_outline_color, PA_RENDER_COPY, canvas_editor->conditional_copy_shader, pa_draw_oval);
				al_use_shader(canvas_editor->standard_shader);
				if(made_undo)
				{
					pa_finalize_undo(canvas_editor);
				}
				canvas_editor->modified++;
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
				case PA_BOX_STATE_RESIZING:
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
	}
}
