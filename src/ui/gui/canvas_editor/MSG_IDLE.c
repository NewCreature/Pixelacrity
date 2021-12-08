#include "t3f/t3f.h"
#include "t3f/file_utils.h"
#include "t3gui/t3gui.h"
#include "modules/canvas/canvas_helpers.h"
#include "ui/canvas_editor/canvas_editor.h"
#include "modules/date.h"
#include "modules/canvas/canvas_file.h"
#include "modules/color.h"
#include "modules/box.h"
#include "ui/canvas_editor/selection.h"
#include "ui/canvas_editor/frame.h"
#include "ui/window.h"
#include "defines.h"

static bool save_backup(PA_CANVAS * cp)
{
	char backup_path[1024];
	char backup_fn[256];

	pa_set_window_message("Saving backup...");
	pa_get_date_string(backup_fn, 256);
	strcat(backup_fn, PA_CANVAS_FILE_EXTENSION);
	t3f_get_filename(t3f_data_path, backup_fn, backup_path, 1024);
	if(pa_save_canvas(cp, backup_path, ".png", PA_CANVAS_SAVE_AUTO))
	{
		return true;
	}
	return false;
}

static void update_color_selections(PA_CANVAS_EDITOR * canvas_editor)
{
	bool update_pick_colors = false;

	/* handle left shade slider */
	if(canvas_editor->left_color.old_shade_slider_d2 != canvas_editor->left_color.shade_slider_element->d2)
	{
		canvas_editor->left_color.shade_color = pa_get_real_color(pa_shade_color(canvas_editor->left_color.base_color, (float)canvas_editor->left_color.shade_slider_element->d2 / 1000.0), canvas_editor->color_scratch_bitmap);
		canvas_editor->left_color.color = pa_alpha_color(canvas_editor->left_color.shade_color, pa_get_color_alpha(canvas_editor->left_color.color));
		canvas_editor->left_color.last_shade_color = canvas_editor->left_color.shade_color;
	}

	/* handle left alpha slider */
	if(canvas_editor->left_color.old_alpha_slider_d2 != canvas_editor->left_color.alpha_slider_element->d2)
	{
		canvas_editor->left_color.alpha_color = pa_get_real_color(pa_alpha_color(canvas_editor->left_color.shade_color, (float)canvas_editor->left_color.alpha_slider_element->d2 / 1000.0), canvas_editor->color_scratch_bitmap);
		canvas_editor->left_color.color = canvas_editor->left_color.alpha_color;
		canvas_editor->left_color.last_alpha_color = canvas_editor->left_color.alpha_color;
	}

	/* handle changing the left base color */
	if(!pa_color_equal(canvas_editor->left_color.base_color, canvas_editor->left_color.last_base_color))
	{
		pa_set_color(&canvas_editor->left_color, canvas_editor->left_color.base_color);
		update_pick_colors = true;
	}
	else if(canvas_editor->left_color.clicked)
	{
		pa_set_color(&canvas_editor->left_color, canvas_editor->left_color.base_color);
		update_pick_colors = true;
		canvas_editor->left_color.clicked = 0;
	}

	/* handle changing the left color */
	if(!pa_color_equal(canvas_editor->left_color.color, canvas_editor->left_color.last_color))
	{
		canvas_editor->left_color.last_color = canvas_editor->left_color.color;
		canvas_editor->left_color.shade_slider_element->d2 = pa_get_color_shade(canvas_editor->left_color.color) * 1000.0;
		canvas_editor->left_color.alpha_slider_element->d2 = pa_get_color_alpha(canvas_editor->left_color.color) * 1000.0;
		canvas_editor->left_color.shade_color = pa_shade_color(canvas_editor->left_color.color, pa_get_color_shade(canvas_editor->left_color.color));
		canvas_editor->left_color.alpha_color = pa_alpha_color(canvas_editor->left_color.shade_color, pa_get_color_alpha(canvas_editor->left_color.color));
		update_pick_colors = true;
	}

	/* handle the right shade slider */
	if(canvas_editor->right_color.old_shade_slider_d2 != canvas_editor->right_color.shade_slider_element->d2)
	{
		canvas_editor->right_color.shade_color = pa_get_real_color(pa_shade_color(canvas_editor->right_color.base_color, (float)canvas_editor->right_color.shade_slider_element->d2 / 1000.0), canvas_editor->color_scratch_bitmap);
		canvas_editor->right_color.color = pa_alpha_color(canvas_editor->right_color.shade_color, pa_get_color_alpha(canvas_editor->right_color.color));
		canvas_editor->right_color.last_shade_color = canvas_editor->right_color.shade_color;
	}

	/* handle the right alpha slider */
	if(canvas_editor->right_color.old_alpha_slider_d2 != canvas_editor->right_color.alpha_slider_element->d2)
	{
		canvas_editor->right_color.alpha_color = pa_get_real_color(pa_alpha_color(canvas_editor->right_color.shade_color, (float)canvas_editor->right_color.alpha_slider_element->d2 / 1000.0), canvas_editor->color_scratch_bitmap);
		canvas_editor->right_color.color = canvas_editor->right_color.alpha_color;
		canvas_editor->right_color.last_alpha_color = canvas_editor->right_color.alpha_color;
	}

	/* handle changing the right base color */
	if(!pa_color_equal(canvas_editor->right_color.base_color, canvas_editor->right_color.last_base_color))
	{
		pa_set_color(&canvas_editor->right_color, canvas_editor->right_color.base_color);
		update_pick_colors = true;
	}
	else if(canvas_editor->right_color.clicked)
	{
		pa_set_color(&canvas_editor->right_color, canvas_editor->right_color.base_color);
		update_pick_colors = true;
		canvas_editor->right_color.clicked = 0;
	}

	/* handle changing the right color */
	if(!pa_color_equal(canvas_editor->right_color.color, canvas_editor->right_color.last_color))
	{
		canvas_editor->right_color.last_color = canvas_editor->right_color.color;
		canvas_editor->right_color.shade_slider_element->d2 = pa_get_color_shade(canvas_editor->right_color.color) * 1000.0;
		canvas_editor->right_color.alpha_slider_element->d2 = pa_get_color_alpha(canvas_editor->right_color.color) * 1000.0;
		canvas_editor->right_color.shade_color = pa_shade_color(canvas_editor->right_color.color, pa_get_color_shade(canvas_editor->right_color.color));
		canvas_editor->right_color.alpha_color = pa_alpha_color(canvas_editor->right_color.shade_color, pa_get_color_alpha(canvas_editor->right_color.color));
		update_pick_colors = true;
	}
	canvas_editor->left_color.old_shade_slider_d2 = canvas_editor->left_color.shade_slider_element->d2;
	canvas_editor->left_color.old_alpha_slider_d2 = canvas_editor->left_color.alpha_slider_element->d2;
	canvas_editor->right_color.old_shade_slider_d2 = canvas_editor->right_color.shade_slider_element->d2;
	canvas_editor->right_color.old_alpha_slider_d2 = canvas_editor->right_color.alpha_slider_element->d2;
	if(update_pick_colors)
	{
		pa_canvas_editor_update_pick_colors(canvas_editor);
	}
}

void pa_canvas_editor_MSG_IDLE(T3GUI_ELEMENT * d, int c)
{
	PA_CANVAS_EDITOR * canvas_editor = (PA_CANVAS_EDITOR *)d->dp;
	int frame_x, frame_y, frame_width, frame_height;

	if(canvas_editor->backup_tick > 0)
	{
		canvas_editor->backup_tick--;
	}
	if(canvas_editor->view_break_out_frame < PA_CANVAS_EDITOR_BREAKOUT_DISTANCE)
	{
		canvas_editor->view_break_out_frame++;
	}
	if(canvas_editor->backup_tick <= 0 && canvas_editor->tool_state == PA_TOOL_STATE_OFF)
	{
		save_backup(canvas_editor->canvas);
		canvas_editor->backup_tick = PA_BACKUP_INTERVAL;
	}
	update_color_selections(canvas_editor);
	canvas_editor->view_x = canvas_editor->view_fx;
	canvas_editor->view_y = canvas_editor->view_fy;
	pa_update_mouse_variables(canvas_editor);
	if(canvas_editor->simulate_mouse_move)
	{
		t3gui_object_message(d, MSG_MOUSEMOVE, 0);
		canvas_editor->simulate_mouse_move = false;
	}

	if(t3f_key[ALLEGRO_KEY_PGUP])
	{
		canvas_editor->current_layer++;
		t3f_key[ALLEGRO_KEY_PGUP] = 0;
	}
	if(t3f_key[ALLEGRO_KEY_PGDN])
	{
		if(canvas_editor->current_layer > 0)
		{
			canvas_editor->current_layer--;
		}
		t3f_key[ALLEGRO_KEY_PGDN] = 0;
	}
	if(t3f_key[ALLEGRO_KEY_C] && !(t3f_key[ALLEGRO_KEY_LCTRL] || t3f_key[ALLEGRO_KEY_RCTRL] || t3f_key[ALLEGRO_KEY_COMMAND]))
	{
		pa_get_canvas_dimensions(canvas_editor->canvas, &frame_x, &frame_y, &frame_width, &frame_height, 0, false);
		canvas_editor->view_x = frame_x + frame_width / 2 - (d->w / 2) / canvas_editor->view_zoom;
		canvas_editor->view_y = frame_y + frame_height / 2 - (d->h / 2) / canvas_editor->view_zoom;
		canvas_editor->view_fx = canvas_editor->view_x;
		canvas_editor->view_fy = canvas_editor->view_y;
		t3f_key[ALLEGRO_KEY_C] = 0;
	}
	if(canvas_editor->tool_state == PA_TOOL_STATE_OFF)
	{
		canvas_editor->hover_color = pa_get_canvas_pixel(canvas_editor->canvas, canvas_editor->current_layer, canvas_editor->hover_x, canvas_editor->hover_y);
	}
	if(canvas_editor->current_tool == PA_TOOL_SELECTION)
	{
		pa_update_selection(canvas_editor, d);
	}
	else
	{
		pa_update_hover_frame(canvas_editor, d);
	}
	pa_update_canvas_editor_box_handles(canvas_editor);
}
