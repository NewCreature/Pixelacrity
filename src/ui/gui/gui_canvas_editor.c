#include "t3f/t3f.h"
#include "t3f/file_utils.h"
#include "modules/canvas/canvas.h"
#include "modules/canvas/canvas_file.h"
#include "modules/canvas/canvas_helpers.h"
#include "ui/canvas_editor/canvas_editor.h"
#include "ui/canvas_editor/undo.h"
#include "ui/canvas_editor/undo_tool.h"
#include "ui/canvas_editor/undo_flood_fill.h"
#include "modules/primitives.h"
#include "ui/gui/gui_canvas_editor.h"
#include "ui/canvas_editor/tools/tool_pixel.h"
#include "ui/canvas_editor/tools/tool_line.h"
#include "ui/canvas_editor/tools/tool_rectangle.h"
#include "ui/canvas_editor/tools/tool_filled_rectangle.h"
#include "ui/canvas_editor/tools/tool_oval.h"
#include "ui/canvas_editor/tools/tool_filled_oval.h"
#include "ui/canvas_editor/tools/tool_dropper.h"
#include "ui/canvas_editor/tools/tool_selection.h"
#include "ui/ui.h"
#include "modules/date.h"
#include "modules/color.h"
#include "modules/canvas/flood_fill.h"
#include "modules/snap.h"

void pa_canvas_editor_update_pick_colors(PA_CANVAS_EDITOR * cep)
{
	float new_l, step;
	int i;

	step = 1.0 / (float)(PA_COLOR_PICKER_SHADES - 1);
	for(i = 0; i < PA_COLOR_PICKER_SHADES; i++)
	{
		new_l = step * (float)i;
		cep->pick_color[i] = pa_shade_color(cep->left_base_color, new_l);
	}
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

static void update_cursor(PA_CANVAS_EDITOR * cep)
{
	if(cep->current_cursor != cep->old_cursor)
	{
		al_set_system_mouse_cursor(t3f_display, cep->current_cursor);
	}
	cep->old_cursor = cep->current_cursor;
}

static int mouse_button = 0;

static void click_on_canvas(PA_CANVAS_EDITOR * cep, int button, int x, int y)
{
	mouse_button = button;
	cep->click_x = x;
	cep->click_y = y;
	cep->click_button = button;
	if(button == 1)
	{
		cep->click_color = cep->left_color;
	}
	else
	{
		cep->click_color = cep->right_color;
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

static void update_tool_variables(PA_CANVAS_EDITOR * cep)
{
	cep->scratch_offset_x = cep->view_x;
	cep->scratch_offset_y = cep->view_y;
	if(cep->hover_x < cep->tool_left)
	{
		cep->tool_left = cep->hover_x;
	}
	if(cep->hover_x > cep->tool_right)
	{
		cep->tool_right = cep->hover_x;
	}
	if(cep->hover_y < cep->tool_top)
	{
		cep->tool_top = cep->hover_y;
	}
	if(cep->hover_y > cep->tool_bottom)
	{
		cep->tool_bottom = cep->hover_y;
	}
}

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

static void clear_bitmap(ALLEGRO_BITMAP * bp)
{
	ALLEGRO_STATE old_state;

	al_store_state(&old_state, ALLEGRO_STATE_TARGET_BITMAP);
	al_set_target_bitmap(bp);
	al_clear_to_color(al_map_rgba_f(0.0, 0.0, 0.0, 0.0));
	al_restore_state(&old_state);
}

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

static bool handle_canvas_expansion(PA_CANVAS_EDITOR * cep)
{
	bool ret;

	ret = pa_handle_canvas_expansion(cep->canvas, cep->tool_left, cep->tool_top, cep->tool_right, cep->tool_bottom, &cep->shift_x, &cep->shift_y);
	pa_shift_canvas_editor_variables(cep, cep->shift_x * cep->canvas->bitmap_size, cep->shift_y * cep->canvas->bitmap_size);

	return ret;
}

static void adjust_zoom(T3GUI_ELEMENT * d, PA_CANVAS_EDITOR * cep, int amount)
{
	int cx, cy;

	cx = cep->view_x + (d->w / cep->view_zoom) / 2;
	cy = cep->view_y + (d->h / cep->view_zoom) / 2;
	cep->view_zoom += amount;
	cep->view_x = cx - (d->w / cep->view_zoom) / 2;
	cep->view_y = cy - (d->h / cep->view_zoom) / 2;
	cep->view_fx = cep->view_x;
	cep->view_fy = cep->view_y;
}

static void update_color_selections(PA_CANVAS_EDITOR * canvas_editor)
{
	/* handle left shade slider */
	if(canvas_editor->old_left_shade_slider_d2 != canvas_editor->left_shade_slider_element->d2)
	{
		canvas_editor->left_shade_color = pa_shade_color(canvas_editor->left_base_color, (float)canvas_editor->left_shade_slider_element->d2 / 1000.0);
		canvas_editor->left_color = canvas_editor->left_shade_color;
		canvas_editor->last_left_shade_color = canvas_editor->left_shade_color;
	}

	/* handle left alpha slider */
	if(canvas_editor->old_left_alpha_slider_d2 != canvas_editor->left_alpha_slider_element->d2)
	{
		canvas_editor->left_alpha_color = pa_alpha_color(canvas_editor->left_shade_color, (float)canvas_editor->left_alpha_slider_element->d2 / 1000.0);
		canvas_editor->left_color = canvas_editor->left_alpha_color;
		canvas_editor->last_left_alpha_color = canvas_editor->left_alpha_color;
	}

	/* handle changing the left base color */
	if(!pa_color_equal(canvas_editor->left_base_color, canvas_editor->last_left_base_color))
	{
		canvas_editor->left_color = canvas_editor->left_base_color;
		canvas_editor->last_left_base_color = canvas_editor->left_base_color;
		pa_canvas_editor_update_pick_colors(canvas_editor);
	}

	/* handle changing the left color */
	if(!pa_color_equal(canvas_editor->left_color, canvas_editor->last_left_color))
	{
		canvas_editor->last_left_color = canvas_editor->left_color;
		canvas_editor->left_shade_slider_element->d2 = pa_get_color_shade(canvas_editor->left_color) * 1000.0;
		canvas_editor->left_alpha_slider_element->d2 = pa_get_color_alpha(canvas_editor->left_color) * 1000.0;
	}

	/* handle the right shade slider */
	if(canvas_editor->old_right_shade_slider_d2 != canvas_editor->right_shade_slider_element->d2)
	{
		canvas_editor->right_shade_color = pa_shade_color(canvas_editor->right_base_color, (float)canvas_editor->right_shade_slider_element->d2 / 1000.0);
		canvas_editor->right_color = canvas_editor->right_shade_color;
		canvas_editor->last_right_shade_color = canvas_editor->right_shade_color;
	}

	/* handle the right alpha slider */
	if(canvas_editor->old_right_alpha_slider_d2 != canvas_editor->right_alpha_slider_element->d2)
	{
		canvas_editor->right_alpha_color = pa_alpha_color(canvas_editor->right_shade_color, (float)canvas_editor->right_alpha_slider_element->d2 / 1000.0);
		canvas_editor->right_color = canvas_editor->right_alpha_color;
		canvas_editor->last_right_alpha_color = canvas_editor->right_alpha_color;
	}

	/* handle changing the right base color */
	if(!pa_color_equal(canvas_editor->right_base_color, canvas_editor->last_right_base_color))
	{
		canvas_editor->right_color = canvas_editor->right_base_color;
		canvas_editor->last_right_base_color = canvas_editor->right_base_color;
		pa_canvas_editor_update_pick_colors(canvas_editor);
	}

	/* handle changing the right color */
	if(!pa_color_equal(canvas_editor->right_color, canvas_editor->last_right_color))
	{
		canvas_editor->last_right_color = canvas_editor->right_color;
		canvas_editor->right_shade_slider_element->d2 = pa_get_color_shade(canvas_editor->right_color) * 1000.0;
		canvas_editor->right_alpha_slider_element->d2 = pa_get_color_alpha(canvas_editor->right_color) * 1000.0;
	}
	canvas_editor->old_left_shade_slider_d2 = canvas_editor->left_shade_slider_element->d2;
	canvas_editor->old_left_alpha_slider_d2 = canvas_editor->left_alpha_slider_element->d2;
	canvas_editor->old_right_shade_slider_d2 = canvas_editor->right_shade_slider_element->d2;
	canvas_editor->old_right_alpha_slider_d2 = canvas_editor->right_alpha_slider_element->d2;
}

static void pa_update_selection(PA_CANVAS_EDITOR * canvas_editor, T3GUI_ELEMENT * d)
{
	PA_BOX old_box;

	if(canvas_editor->selection.box.width > 0 && canvas_editor->selection.box.height > 0)
	{
		memcpy(&old_box, &canvas_editor->selection.box, sizeof(PA_BOX));
		pa_update_box_handles(&canvas_editor->selection.box, canvas_editor->view_x, canvas_editor->view_y, canvas_editor->view_zoom);
		pa_box_logic(&canvas_editor->selection.box, canvas_editor->view_x, canvas_editor->view_y, canvas_editor->view_zoom, d->x, d->y, false);
		if(!canvas_editor->selection.bitmap && (canvas_editor->selection.box.start_x != old_box.start_x || canvas_editor->selection.box.start_y != old_box.start_y))
		{
			if(canvas_editor->selection.box.state == PA_BOX_STATE_MOVING)
			{
				pa_float_canvas_editor_selection(canvas_editor, &old_box);
			}
		}
	}
}

int pa_gui_canvas_editor_proc(int msg, T3GUI_ELEMENT * d, int c)
{
	PA_CANVAS_EDITOR * canvas_editor = (PA_CANVAS_EDITOR *)d->dp;
	char frame_name[256];
	int frame_x, frame_y, frame_width, frame_height;
	int cx, cy, tx, ty;
	T3GUI_THEME * theme;
	ALLEGRO_COLOR color = t3f_color_black;
	bool made_undo = false;
	bool simulate_mouse_move = false;
	PA_QUEUE * flood_fill_queue = NULL;

	switch(msg)
	{
		case MSG_GOTMOUSE:
		{
			canvas_editor->hover_color = pa_get_canvas_pixel(canvas_editor->canvas, canvas_editor->current_layer, canvas_editor->hover_x, canvas_editor->hover_y);
			d->flags |= D_TRACKMOUSE;
			break;
		}
		case MSG_LOSTMOUSE:
		{
			if(canvas_editor->tool_state == PA_TOOL_STATE_OFF)
			{
				canvas_editor->hover_color = al_map_rgba_f(0.0, 0.0, 0.0, 0.0);
				d->flags &= ~D_TRACKMOUSE;
			}
			break;
		}
		case MSG_MOUSEDOWN:
		{
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
								if(pa_flood_fill_canvas(canvas_editor->canvas, canvas_editor->current_layer, canvas_editor->hover_x, canvas_editor->hover_y, c == 1 ? canvas_editor->left_color : canvas_editor->right_color, flood_fill_queue))
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
			break;
		}
		case MSG_MOUSEUP:
		{
			update_tool_variables(canvas_editor);
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
						canvas_editor->update_title = true;
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
						canvas_editor->update_title = true;
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
						canvas_editor->update_title = true;
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
						al_use_shader(canvas_editor->standard_shader);
						if(made_undo)
						{
							pa_finalize_undo(canvas_editor);
						}
						canvas_editor->modified++;
						canvas_editor->update_title = true;
						canvas_editor->tool_state = PA_TOOL_STATE_OFF;
					}
					break;
				}
				case PA_TOOL_OVAL:
				{
					if(canvas_editor->tool_state == PA_TOOL_STATE_DRAWING && c == canvas_editor->click_button)
					{
						made_undo = create_primitive_undo(canvas_editor);
						handle_canvas_expansion(canvas_editor);
						pa_draw_primitive_to_canvas(canvas_editor->canvas, canvas_editor->current_layer, canvas_editor->click_x, canvas_editor->click_y, canvas_editor->release_x, canvas_editor->release_y, canvas_editor->brush, canvas_editor->click_color, PA_RENDER_COPY, canvas_editor->conditional_copy_shader, pa_draw_oval);
						al_use_shader(canvas_editor->standard_shader);
						if(made_undo)
						{
							pa_finalize_undo(canvas_editor);
						}
						canvas_editor->modified++;
						canvas_editor->update_title = true;
						canvas_editor->tool_state = PA_TOOL_STATE_OFF;
					}
					break;
				}
				case PA_TOOL_FILLED_OVAL:
				{
					if(canvas_editor->tool_state == PA_TOOL_STATE_DRAWING && c == canvas_editor->click_button)
					{
						made_undo = create_primitive_undo(canvas_editor);
						handle_canvas_expansion(canvas_editor);
						pa_draw_primitive_to_canvas(canvas_editor->canvas, canvas_editor->current_layer, canvas_editor->click_x, canvas_editor->click_y, canvas_editor->release_x, canvas_editor->release_y, NULL, canvas_editor->click_color, PA_RENDER_COPY, canvas_editor->conditional_copy_shader, pa_draw_filled_oval);
						al_use_shader(canvas_editor->standard_shader);
						if(made_undo)
						{
							pa_finalize_undo(canvas_editor);
						}
						canvas_editor->modified++;
						canvas_editor->update_title = true;
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
			break;
		}
		case MSG_MOUSEMOVE:
		{
			update_tool_variables(canvas_editor);
			switch(canvas_editor->current_tool)
			{
				case PA_TOOL_PIXEL:
				case PA_TOOL_ERASER:
				{
					if(canvas_editor->tool_state == PA_TOOL_STATE_DRAWING || canvas_editor->tool_state == PA_TOOL_STATE_EDITING)
					{
						pa_tool_pixel_logic(canvas_editor);
					}
					break;
				}
				case PA_TOOL_DROPPER:
				{
					if(canvas_editor->tool_state == PA_TOOL_STATE_DRAWING || canvas_editor->tool_state == PA_TOOL_STATE_EDITING)
					{
						pa_tool_dropper_logic(canvas_editor, mouse_button);
					}
					break;
				}
				case PA_TOOL_LINE:
				{
					if(canvas_editor->tool_state == PA_TOOL_STATE_DRAWING || canvas_editor->tool_state == PA_TOOL_STATE_EDITING)
					{
						pa_tool_line_logic(canvas_editor);
					}
					break;
				}
				case PA_TOOL_RECTANGLE:
				{
					if(canvas_editor->tool_state == PA_TOOL_STATE_DRAWING || canvas_editor->tool_state == PA_TOOL_STATE_EDITING)
					{
						pa_tool_rectangle_logic(canvas_editor);
					}
					break;
				}
				case PA_TOOL_FILLED_RECTANGLE:
				{
					if(canvas_editor->tool_state == PA_TOOL_STATE_DRAWING || canvas_editor->tool_state == PA_TOOL_STATE_EDITING)
					{
						pa_tool_filled_rectangle_logic(canvas_editor);
					}
					break;
				}
				case PA_TOOL_OVAL:
				{
					if(canvas_editor->tool_state == PA_TOOL_STATE_DRAWING || canvas_editor->tool_state == PA_TOOL_STATE_EDITING)
					{
						pa_tool_oval_logic(canvas_editor);
					}
					break;
				}
				case PA_TOOL_FILLED_OVAL:
				{
					if(canvas_editor->tool_state == PA_TOOL_STATE_DRAWING || canvas_editor->tool_state == PA_TOOL_STATE_EDITING)
					{
						pa_tool_filled_oval_logic(canvas_editor);
					}
					break;
				}
				case PA_TOOL_SELECTION:
				{
					pa_update_selection(canvas_editor, d);
					break;
				}
			}
			break;
		}
		case MSG_WHEEL:
		{
			if(c < 0)
			{
				if(canvas_editor->view_zoom > 1)
				{
					adjust_zoom(d, canvas_editor, -1);
				}
			}
			else if(c > 0)
			{
				adjust_zoom(d, canvas_editor, 1);
			}
			break;
		}
		case MSG_IDLE:
		{
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
					adjust_zoom(d, canvas_editor, -1);
					simulate_mouse_move = true;
				}
				t3f_key[ALLEGRO_KEY_MINUS] = 0;
			}
			if(t3f_key[ALLEGRO_KEY_EQUALS])
			{
				adjust_zoom(d, canvas_editor, 1);
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
			break;
		}
		case MSG_DRAW:
		{
			ALLEGRO_STATE old_state;
			ALLEGRO_TRANSFORM identity;
			int i;

			al_store_state(&old_state, ALLEGRO_STATE_TRANSFORM | ALLEGRO_STATE_BLENDER);
			al_identity_transform(&identity);
			al_use_transform(&identity);

			/* render background layers */
			if(!t3f_key[ALLEGRO_KEY_I])
			{
				for(i = 0; i < canvas_editor->current_layer; i++)
				{
					pa_render_canvas_layer(canvas_editor->canvas, i, canvas_editor->view_x, canvas_editor->view_y, canvas_editor->view_zoom, d->x, d->y, d->w, d->h);
				}
			}

			if(canvas_editor->selection.bitmap)
			{
				pa_tool_selection_logic(canvas_editor);
			}
			if(canvas_editor->tool_state == PA_TOOL_STATE_DRAWING || canvas_editor->selection.bitmap)
			{
				al_draw_scaled_bitmap(canvas_editor->scratch_bitmap, 0, 0,  al_get_bitmap_width(canvas_editor->scratch_bitmap), al_get_bitmap_height(canvas_editor->scratch_bitmap), d->x - (canvas_editor->view_x - canvas_editor->scratch_offset_x) * canvas_editor->view_zoom, d->y - (canvas_editor->view_y - canvas_editor->scratch_offset_y) * canvas_editor->view_zoom, al_get_bitmap_width(canvas_editor->scratch_bitmap) * canvas_editor->view_zoom, al_get_bitmap_height(canvas_editor->scratch_bitmap) * canvas_editor->view_zoom, 0);
			}
			else
			{
				pa_render_canvas_layer(canvas_editor->canvas, canvas_editor->current_layer, canvas_editor->view_x, canvas_editor->view_y, canvas_editor->view_zoom, d->x, d->y, d->w, d->h);
			}

			/* draw foreground layers */
			for(i = canvas_editor->current_layer + 1; i < canvas_editor->canvas->layer_max; i++)
			{
				if(!t3f_key[ALLEGRO_KEY_I])
				{
					pa_render_canvas_layer(canvas_editor->canvas, i, canvas_editor->view_x, canvas_editor->view_y, canvas_editor->view_zoom, d->x, d->y, d->w, d->h);
				}
			}

			/* render frames */
			theme = t3gui_get_default_theme();
			for(i = 0; i < canvas_editor->canvas->frame_max; i++)
			{
				if(i == canvas_editor->current_frame)
				{
					color = t3f_color_black;
				}
				else
				{
					color = al_map_rgba_f(0.0, 0.0, 0.0, 0.25);
				}
				if(theme)
				{
					al_draw_text(theme->state[0].font[0], color, d->x + (canvas_editor->canvas->frame[i]->box.start_x - canvas_editor->view_x) * canvas_editor->view_zoom, d->y + (canvas_editor->canvas->frame[i]->box.start_y - canvas_editor->view_y) * canvas_editor->view_zoom - al_get_font_line_height(theme->state[0].font[0]) - PA_UI_ELEMENT_SPACE, 0, canvas_editor->canvas->frame[i]->name);
				}
				al_draw_rectangle(d->x + (canvas_editor->canvas->frame[i]->box.start_x - canvas_editor->view_x) * canvas_editor->view_zoom - 1.0 + 0.5, d->y + (canvas_editor->canvas->frame[i]->box.start_y - canvas_editor->view_y) * canvas_editor->view_zoom - 1.0 + 0.5, d->x + (canvas_editor->canvas->frame[i]->box.start_x + canvas_editor->canvas->frame[i]->box.width - canvas_editor->view_x) * canvas_editor->view_zoom + 0.5, d->y + (canvas_editor->canvas->frame[i]->box.start_y + canvas_editor->canvas->frame[i]->box.height - canvas_editor->view_y) * canvas_editor->view_zoom + 0.5, color, 1.0);
			}

			/* render selection box */
			if(canvas_editor->selection.box.width > 0 && canvas_editor->selection.box.height > 0)
			{
				pa_box_render(&canvas_editor->selection.box, 0, canvas_editor->view_x, canvas_editor->view_y, canvas_editor->view_zoom, d->x, d->y);
			}

			if(t3f_key[ALLEGRO_KEY_G])
			{
				tx = ((int)(t3f_mouse_x - d->x) / canvas_editor->view_zoom) * canvas_editor->view_zoom + canvas_editor->view_zoom / 2 + d->x;
				ty = ((int)(t3f_mouse_y - d->y) / canvas_editor->view_zoom) * canvas_editor->view_zoom + canvas_editor->view_zoom / 2 + d->y;
				al_draw_line(d->x, ty + 0.5, d->x + d->w, ty + 0.5, t3f_color_black, 0.0);
				al_draw_line(tx + 0.5, d->y, tx + 0/5, d->y + d->h, t3f_color_black, 0.0);
			}
			al_restore_state(&old_state);
			break;
		}
	}
	return D_O_K;
}
