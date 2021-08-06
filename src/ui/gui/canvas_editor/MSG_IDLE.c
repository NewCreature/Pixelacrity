#include "t3f/t3f.h"
#include "t3f/file_utils.h"
#include "t3gui/t3gui.h"
#include "modules/canvas/canvas_helpers.h"
#include "ui/canvas_editor/canvas_editor.h"
#include "modules/date.h"
#include "modules/canvas/canvas_file.h"
#include "modules/color.h"
#include "ui/canvas_editor/selection.h"

static bool save_backup(PA_CANVAS * cp)
{
	char backup_path[1024];
	char backup_fn[256];

	pa_get_date_string(backup_fn, 256);
	strcat(backup_fn, ".qcanvas");
	t3f_get_filename(t3f_data_path, backup_fn, backup_path, 1024);
	if(pa_save_canvas(cp, backup_path, ".png", PA_CANVAS_SAVE_AUTO))
	{
		return true;
	}
	return false;
}
static void update_window_title(PA_CANVAS_EDITOR * cep)
{
	char buf[1024];
	const char * fn;

	if(cep->update_title)
	{
		fn = t3f_get_path_filename(cep->canvas_path);
		if(fn)
		{
			sprintf(buf, T3F_APP_TITLE " - %s%s", fn, cep->modified ? "*" : "");
		}
		else
		{
			sprintf(buf, T3F_APP_TITLE " - Untitled%s", cep->modified ? "*" : "");
		}
		al_set_window_title(t3f_display, buf);
		cep->update_title = false;
	}
}

static void update_color_selections(PA_CANVAS_EDITOR * canvas_editor)
{
	/* handle left shade slider */
	if(canvas_editor->left_color.old_shade_slider_d2 != canvas_editor->left_color.shade_slider_element->d2)
	{
		canvas_editor->left_color.shade_color = pa_shade_color(canvas_editor->left_color.base_color, (float)canvas_editor->left_color.shade_slider_element->d2 / 1000.0);
		canvas_editor->left_color.color = canvas_editor->left_color.shade_color;
		canvas_editor->left_color.last_shade_color = canvas_editor->left_color.shade_color;
	}

	/* handle left alpha slider */
	if(canvas_editor->left_color.old_alpha_slider_d2 != canvas_editor->left_color.alpha_slider_element->d2)
	{
		canvas_editor->left_color.alpha_color = pa_alpha_color(canvas_editor->left_color.shade_color, (float)canvas_editor->left_color.alpha_slider_element->d2 / 1000.0);
		canvas_editor->left_color.color = canvas_editor->left_color.alpha_color;
		canvas_editor->left_color.last_alpha_color = canvas_editor->left_color.alpha_color;
	}

	/* handle changing the left base color */
	if(!pa_color_equal(canvas_editor->left_color.base_color, canvas_editor->left_color.last_base_color))
	{
		pa_set_color(&canvas_editor->left_color, canvas_editor->left_color.base_color);
		pa_canvas_editor_update_pick_colors(canvas_editor);
	}

	/* handle changing the left color */
	if(!pa_color_equal(canvas_editor->left_color.color, canvas_editor->left_color.last_color))
	{
		canvas_editor->left_color.last_color = canvas_editor->left_color.color;
		canvas_editor->left_color.shade_slider_element->d2 = pa_get_color_shade(canvas_editor->left_color.color) * 1000.0;
		canvas_editor->left_color.alpha_slider_element->d2 = pa_get_color_alpha(canvas_editor->left_color.color) * 1000.0;
		canvas_editor->left_color.shade_color = pa_shade_color(canvas_editor->left_color.color, pa_get_color_shade(canvas_editor->left_color.color));
		canvas_editor->left_color.alpha_color = pa_alpha_color(canvas_editor->left_color.shade_color, pa_get_color_alpha(canvas_editor->left_color.color));
	}

	/* handle the right shade slider */
	if(canvas_editor->right_color.old_shade_slider_d2 != canvas_editor->right_color.shade_slider_element->d2)
	{
		canvas_editor->right_color.shade_color = pa_shade_color(canvas_editor->right_color.base_color, (float)canvas_editor->right_color.shade_slider_element->d2 / 1000.0);
		canvas_editor->right_color.color = canvas_editor->right_color.shade_color;
		canvas_editor->right_color.last_shade_color = canvas_editor->right_color.shade_color;
	}

	/* handle the right alpha slider */
	if(canvas_editor->right_color.old_alpha_slider_d2 != canvas_editor->right_color.alpha_slider_element->d2)
	{
		canvas_editor->right_color.alpha_color = pa_alpha_color(canvas_editor->right_color.shade_color, (float)canvas_editor->right_color.alpha_slider_element->d2 / 1000.0);
		canvas_editor->right_color.color = canvas_editor->right_color.alpha_color;
		canvas_editor->right_color.last_alpha_color = canvas_editor->right_color.alpha_color;
	}

	/* handle changing the right base color */
	if(!pa_color_equal(canvas_editor->right_color.base_color, canvas_editor->right_color.last_base_color))
	{
		pa_set_color(&canvas_editor->left_color, canvas_editor->left_color.base_color);
		pa_canvas_editor_update_pick_colors(canvas_editor);
	}

	/* handle changing the right color */
	if(!pa_color_equal(canvas_editor->right_color.color, canvas_editor->right_color.last_color))
	{
		canvas_editor->right_color.last_color = canvas_editor->right_color.color;
		canvas_editor->right_color.shade_slider_element->d2 = pa_get_color_shade(canvas_editor->right_color.color) * 1000.0;
		canvas_editor->right_color.alpha_slider_element->d2 = pa_get_color_alpha(canvas_editor->right_color.color) * 1000.0;
		canvas_editor->right_color.shade_color = pa_shade_color(canvas_editor->right_color.color, pa_get_color_shade(canvas_editor->right_color.color));
		canvas_editor->right_color.alpha_color = pa_alpha_color(canvas_editor->right_color.shade_color, pa_get_color_alpha(canvas_editor->right_color.color));
	}
	canvas_editor->left_color.old_shade_slider_d2 = canvas_editor->left_color.shade_slider_element->d2;
	canvas_editor->left_color.old_alpha_slider_d2 = canvas_editor->left_color.alpha_slider_element->d2;
	canvas_editor->right_color.old_shade_slider_d2 = canvas_editor->right_color.shade_slider_element->d2;
	canvas_editor->right_color.old_alpha_slider_d2 = canvas_editor->right_color.alpha_slider_element->d2;
}

void pa_canvas_editor_MSG_IDLE(T3GUI_ELEMENT * d, int c)
{
	PA_CANVAS_EDITOR * canvas_editor = (PA_CANVAS_EDITOR *)d->dp;
	bool simulate_mouse_move = false;
	int frame_x, frame_y, frame_width, frame_height;

	if(canvas_editor->backup_tick > 0)
	{
		canvas_editor->backup_tick--;
	}
	if(canvas_editor->backup_tick <= 0 && canvas_editor->tool_state == PA_TOOL_STATE_OFF)
	{
		save_backup(canvas_editor->canvas);
		canvas_editor->backup_tick = PA_BACKUP_INTERVAL;
	}
	update_window_title(canvas_editor);
//			update_cursor(canvas_editor);
	update_color_selections(canvas_editor);
	canvas_editor->view_scroll_speed = 8.0 / (float)canvas_editor->view_zoom;
	if(t3f_key[ALLEGRO_KEY_LEFT] || t3f_key[ALLEGRO_KEY_A])
	{
		canvas_editor->view_fx -= canvas_editor->view_scroll_speed;
		simulate_mouse_move = true;
	}
	if(t3f_key[ALLEGRO_KEY_RIGHT] || t3f_key[ALLEGRO_KEY_D])
	{
		canvas_editor->view_fx += canvas_editor->view_scroll_speed;
		simulate_mouse_move = true;
	}
	if(t3f_key[ALLEGRO_KEY_UP] || t3f_key[ALLEGRO_KEY_W])
	{
		canvas_editor->view_fy -= canvas_editor->view_scroll_speed;
		simulate_mouse_move = true;
	}
	if(t3f_key[ALLEGRO_KEY_DOWN] || t3f_key[ALLEGRO_KEY_S])
	{
		canvas_editor->view_fy += canvas_editor->view_scroll_speed;
		simulate_mouse_move = true;
	}
	if(t3f_key[ALLEGRO_KEY_MINUS])
	{
		if(canvas_editor->view_zoom > 1)
		{
			pa_set_canvas_editor_zoom(canvas_editor, canvas_editor->view_zoom - 1);
			simulate_mouse_move = true;
		}
		t3f_key[ALLEGRO_KEY_MINUS] = 0;
	}
	if(t3f_key[ALLEGRO_KEY_EQUALS])
	{
		pa_set_canvas_editor_zoom(canvas_editor, canvas_editor->view_zoom + 1);
		simulate_mouse_move = true;
		t3f_key[ALLEGRO_KEY_EQUALS] = 0;
	}
	if(t3f_key[ALLEGRO_KEY_H])
	{
		canvas_editor->canvas->layer[canvas_editor->current_layer]->flags ^= PA_CANVAS_FLAG_HIDDEN;
		t3f_key[ALLEGRO_KEY_H] = 0;
	}
	canvas_editor->view_x = canvas_editor->view_fx;
	canvas_editor->view_y = canvas_editor->view_fy;
	canvas_editor->last_hover_x = canvas_editor->hover_x;
	canvas_editor->hover_x = canvas_editor->view_x + (t3f_mouse_x - d->x) / canvas_editor->view_zoom;
	canvas_editor->last_hover_y = canvas_editor->hover_y;
	canvas_editor->hover_y = canvas_editor->view_y + (t3f_mouse_y - d->y) / canvas_editor->view_zoom;
	if(simulate_mouse_move)
	{
		t3gui_object_message(d, MSG_MOUSEMOVE, 0);
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
		pa_get_canvas_dimensions(canvas_editor->canvas, &frame_x, &frame_y, &frame_width, &frame_height, 0);
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
}
