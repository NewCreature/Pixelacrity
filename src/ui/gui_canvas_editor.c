#include "t3f/t3f.h"
#include "t3f/file_utils.h"
#include "canvas.h"
#include "canvas_helpers.h"
#include "canvas_editor.h"
#include "primitives.h"
#include "gui_canvas_editor.h"
#include "tool_pixel.h"
#include "tool_line.h"
#include "tool_rectangle.h"
#include "tool_filled_rectangle.h"
#include "tool_oval.h"
#include "tool_filled_oval.h"
#include "tool_dropper.h"
#include "tool_selection.h"
#include "ui.h"

static ALLEGRO_COLOR shade_color(ALLEGRO_COLOR color, float l)
{
	float h, s, old_l;
	float r, g, b;

	al_unmap_rgb_f(color, &r, &g, &b);
	al_color_rgb_to_hsl(r, g, b, &h, &s, &old_l);

	return al_color_hsl(h, s, l);
}

static ALLEGRO_COLOR alpha_color(ALLEGRO_COLOR color, float alpha)
{
	float r, g, b;

	al_unmap_rgb_f(color, &r, &g, &b);

	return al_map_rgba_f(r, g, b, alpha);
}

static float get_shade(ALLEGRO_COLOR color)
{
	float r, g, b, h, s, l;

	al_unmap_rgb_f(color, &r, &g, &b);
	al_color_rgb_to_hsl(r, g, b, &h, &s, &l);

	return l;
}

static float get_alpha(ALLEGRO_COLOR color)
{
	float r, a;

	al_unmap_rgba_f(color, &r, &r, &r, &a);

	return a;
}

void quixel_canvas_editor_update_pick_colors(QUIXEL_CANVAS_EDITOR * cep)
{
	float new_l, step;
	int i;

	step = 1.0 / (float)(QUIXEL_COLOR_PICKER_SHADES - 1);
	for(i = 0; i < QUIXEL_COLOR_PICKER_SHADES; i++)
	{
		new_l = step * (float)i;
		cep->pick_color[i] = shade_color(cep->left_base_color, new_l);
	}
}

static bool color_equal(ALLEGRO_COLOR c1, ALLEGRO_COLOR c2)
{
	unsigned char r[2], g[2], b[2], a[2];

	al_unmap_rgba(c1, &r[0], &g[0], &b[0], &a[0]);
	al_unmap_rgba(c2, &r[1], &g[1], &b[1], &a[1]);
	if(r[0] != r[1] || g[0] != g[1] || b[0] != b[1] || a[0] != a[1])
	{
		return false;
	}
	return true;
}

static void update_window_title(QUIXEL_CANVAS_EDITOR * cep)
{
	char buf[1024];
	const char * fn;

	if(cep->update_title)
	{
		fn = t3f_get_path_filename(cep->canvas_path);
		if(fn)
		{
			sprintf(buf, "Quixel - %s%s", fn, cep->modified ? "*" : "");
			al_set_window_title(t3f_display, buf);
		}
		else
		{
			sprintf(buf, "Quixel - Untitled%s", cep->modified ? "*" : "");
		}
		cep->update_title = false;
	}
}

static void float_selection(QUIXEL_CANVAS_EDITOR * cep, QUIXEL_BOX * bp)
{
	ALLEGRO_STATE old_state;
	ALLEGRO_TRANSFORM identity;

	al_store_state(&old_state, ALLEGRO_STATE_BLENDER | ALLEGRO_STATE_TRANSFORM | ALLEGRO_STATE_TARGET_BITMAP);
	al_set_target_bitmap(cep->scratch_bitmap);
	al_identity_transform(&identity);
	al_use_transform(&identity);
	al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);
	al_clear_to_color(al_map_rgba_f(0.0, 0.0, 0.0, 0.0));
	quixel_render_canvas_layer(cep->canvas, cep->current_layer, bp->start_x, bp->start_y, 1, 0, 0, bp->width, bp->height);
	al_restore_state(&old_state);
	quixel_draw_primitive_to_canvas(cep->canvas, cep->current_layer, bp->start_x, bp->start_y, bp->start_x + bp->width - 1, bp->start_y + bp->height - 1, NULL, al_map_rgba_f(0, 0, 0, 0), QUIXEL_RENDER_COPY, quixel_draw_filled_rectangle);
	cep->selection.floating = true;
}

static void update_cursor(QUIXEL_CANVAS_EDITOR * cep)
{
	if(cep->current_cursor != cep->old_cursor)
	{
		al_set_system_mouse_cursor(t3f_display, cep->current_cursor);
	}
	cep->old_cursor = cep->current_cursor;
}

static void click_on_canvas(QUIXEL_CANVAS_EDITOR * cep, int x, int y)
{
	cep->click_x = x;
	cep->click_y = y;
	if(t3f_mouse_button[0])
	{
		cep->click_color = cep->left_color;
	}
	else
	{
		cep->click_color = cep->right_color;
	}
}

int quixel_gui_canvas_editor_proc(int msg, T3GUI_ELEMENT * d, int c)
{
	QUIXEL_CANVAS_EDITOR * canvas_editor = (QUIXEL_CANVAS_EDITOR *)d->dp;
	char frame_name[256];
	int frame_x, frame_y, frame_width, frame_height;
	int cx, cy, w, h;
	QUIXEL_BOX old_box;
	T3GUI_THEME * theme;
	ALLEGRO_COLOR color = t3f_color_black;

	switch(msg)
	{
		case MSG_GOTMOUSE:
		{
			canvas_editor->hover_color = quixel_get_canvas_pixel(canvas_editor->canvas, canvas_editor->current_layer, canvas_editor->hover_x, canvas_editor->hover_y);
			d->flags |= D_TRACKMOUSE;
			break;
		}
		case MSG_LOSTMOUSE:
		{
			if(canvas_editor->tool_state == QUIXEL_TOOL_STATE_OFF)
			{
				canvas_editor->hover_color = al_map_rgba_f(0.0, 0.0, 0.0, 0.0);
				d->flags &= ~D_TRACKMOUSE;
			}
			break;
		}
		case MSG_MOUSEDOWN:
		{
			d->flags |= D_TRACKMOUSE;
			switch(canvas_editor->current_tool)
			{
				case QUIXEL_TOOL_PIXEL:
				{
					click_on_canvas(canvas_editor, canvas_editor->hover_x, canvas_editor->hover_y);
					quixel_tool_pixel_logic(canvas_editor);
					canvas_editor->modified = true;
					canvas_editor->update_title = true;
					canvas_editor->tool_state = QUIXEL_TOOL_STATE_EDITING;
					break;
				}
				case QUIXEL_TOOL_LINE:
				{
					click_on_canvas(canvas_editor, canvas_editor->hover_x, canvas_editor->hover_y);
					quixel_tool_line_logic(canvas_editor);
					canvas_editor->tool_state = QUIXEL_TOOL_STATE_DRAWING;
					break;
				}
				case QUIXEL_TOOL_RECTANGLE:
				{
					click_on_canvas(canvas_editor, canvas_editor->hover_x, canvas_editor->hover_y);
					quixel_tool_rectangle_logic(canvas_editor);
					canvas_editor->tool_state = QUIXEL_TOOL_STATE_DRAWING;
					break;
				}
				case QUIXEL_TOOL_FILLED_RECTANGLE:
				{
					click_on_canvas(canvas_editor, canvas_editor->hover_x, canvas_editor->hover_y);
					quixel_tool_filled_rectangle_logic(canvas_editor);
					canvas_editor->tool_state = QUIXEL_TOOL_STATE_DRAWING;
					break;
				}
				case QUIXEL_TOOL_OVAL:
				{
					click_on_canvas(canvas_editor, canvas_editor->hover_x, canvas_editor->hover_y);
					quixel_tool_oval_logic(canvas_editor);
					canvas_editor->tool_state = QUIXEL_TOOL_STATE_DRAWING;
					break;
				}
				case QUIXEL_TOOL_FILLED_OVAL:
				{
					click_on_canvas(canvas_editor, canvas_editor->hover_x, canvas_editor->hover_y);
					quixel_tool_filled_oval_logic(canvas_editor);
					canvas_editor->tool_state = QUIXEL_TOOL_STATE_DRAWING;
					break;
				}
				case QUIXEL_TOOL_DROPPER:
				{
					quixel_tool_dropper_logic(canvas_editor);
					break;
				}
				case QUIXEL_TOOL_SELECTION:
				{
					click_on_canvas(canvas_editor, canvas_editor->hover_x, canvas_editor->hover_y);
					switch(canvas_editor->selection.box.state)
					{
						/* start creating a new selection if we are not currently
						   interacting with an existing selection */
						case QUIXEL_BOX_STATE_IDLE:
						{
							if(canvas_editor->selection.floating)
							{
								quixel_unfloat_canvas_editor_selection(canvas_editor, &canvas_editor->selection.box);
							}
							quixel_initialize_box(&canvas_editor->selection.box, canvas_editor->click_x, canvas_editor->click_y, 1, 1, canvas_editor->peg_bitmap);
							canvas_editor->selection.box.hover_handle = 0;
							canvas_editor->selection.box.state = QUIXEL_BOX_STATE_RESIZING;
							break;
						}
						/* set box moving logic in motion */
						case QUIXEL_BOX_STATE_HOVER:
						{
							canvas_editor->selection.box.click_start_x = canvas_editor->selection.box.start_x;
							canvas_editor->selection.box.click_start_y = canvas_editor->selection.box.start_y;
							canvas_editor->selection.box.click_x = canvas_editor->selection.box.hover_x;
							canvas_editor->selection.box.click_y = canvas_editor->selection.box.hover_y;
							canvas_editor->selection.box.state = QUIXEL_BOX_STATE_MOVING;
							break;
						}
						/* set box resizing logic in motion */
						case QUIXEL_BOX_STATE_HOVER_HANDLE:
						{
							if(canvas_editor->selection.floating)
							{
								quixel_unfloat_canvas_editor_selection(canvas_editor, &canvas_editor->selection.box);
							}
							canvas_editor->selection.box.state = QUIXEL_BOX_STATE_RESIZING;
							break;
						}
					}
					canvas_editor->tool_state = QUIXEL_TOOL_STATE_EDITING;
					break;
				}
			}
			break;
		}
		case MSG_MOUSEUP:
		{
			canvas_editor->release_x = canvas_editor->hover_x;
			canvas_editor->release_y = canvas_editor->hover_y;
			switch(canvas_editor->current_tool)
			{
				case QUIXEL_TOOL_PIXEL:
				{
					canvas_editor->tool_state = QUIXEL_TOOL_STATE_OFF;
					break;
				}
				case QUIXEL_TOOL_LINE:
				{
					quixel_draw_primitive_to_canvas(canvas_editor->canvas, canvas_editor->current_layer, canvas_editor->click_x, canvas_editor->click_y, canvas_editor->release_x, canvas_editor->release_y, NULL, canvas_editor->click_color, QUIXEL_RENDER_COPY, quixel_draw_line);
					canvas_editor->modified = true;
					canvas_editor->update_title = true;
					canvas_editor->tool_state = QUIXEL_TOOL_STATE_OFF;
					break;
				}
				case QUIXEL_TOOL_RECTANGLE:
				{
					quixel_draw_primitive_to_canvas(canvas_editor->canvas, canvas_editor->current_layer, canvas_editor->click_x, canvas_editor->click_y, canvas_editor->release_x, canvas_editor->release_y, NULL, canvas_editor->click_color, QUIXEL_RENDER_COPY, quixel_draw_rectangle);
					canvas_editor->modified = true;
					canvas_editor->update_title = true;
					canvas_editor->tool_state = QUIXEL_TOOL_STATE_OFF;
					break;
				}
				case QUIXEL_TOOL_FILLED_RECTANGLE:
				{
					quixel_draw_primitive_to_canvas(canvas_editor->canvas, canvas_editor->current_layer, canvas_editor->click_x, canvas_editor->click_y, canvas_editor->release_x, canvas_editor->release_y, NULL, canvas_editor->click_color, QUIXEL_RENDER_COPY, quixel_draw_filled_rectangle);
					canvas_editor->modified = true;
					canvas_editor->update_title = true;
					canvas_editor->tool_state = QUIXEL_TOOL_STATE_OFF;
					break;
				}
				case QUIXEL_TOOL_OVAL:
				{
					quixel_draw_primitive_to_canvas(canvas_editor->canvas, canvas_editor->current_layer, canvas_editor->click_x, canvas_editor->click_y, canvas_editor->release_x, canvas_editor->release_y, NULL, canvas_editor->click_color, QUIXEL_RENDER_COPY, quixel_draw_oval);
					canvas_editor->modified = true;
					canvas_editor->update_title = true;
					canvas_editor->tool_state = QUIXEL_TOOL_STATE_OFF;
					break;
				}
				case QUIXEL_TOOL_FILLED_OVAL:
				{
					quixel_draw_primitive_to_canvas(canvas_editor->canvas, canvas_editor->current_layer, canvas_editor->click_x, canvas_editor->click_y, canvas_editor->release_x, canvas_editor->release_y, NULL, canvas_editor->click_color, QUIXEL_RENDER_COPY, quixel_draw_filled_oval);
					canvas_editor->modified = true;
					canvas_editor->update_title = true;
					canvas_editor->tool_state = QUIXEL_TOOL_STATE_OFF;
					break;
				}
				case QUIXEL_TOOL_SELECTION:
				{
					switch(canvas_editor->selection.box.state)
					{
						case QUIXEL_BOX_STATE_MOVING:
						case QUIXEL_BOX_STATE_RESIZING:
						{
							canvas_editor->selection.box.state = QUIXEL_BOX_STATE_IDLE;
							break;
						}
					}
					canvas_editor->tool_state = QUIXEL_TOOL_STATE_OFF;
					break;
				}
			}
			break;
		}
		case MSG_MOUSEMOVE:
		{
			switch(canvas_editor->current_tool)
			{
				case QUIXEL_TOOL_PIXEL:
				{
					if(canvas_editor->tool_state == QUIXEL_TOOL_STATE_DRAWING || canvas_editor->tool_state == QUIXEL_TOOL_STATE_EDITING)
					{
						quixel_tool_pixel_logic(canvas_editor);
						canvas_editor->modified = true;
						canvas_editor->update_title = true;
					}
					break;
				}
				case QUIXEL_TOOL_DROPPER:
				{
					if(canvas_editor->tool_state == QUIXEL_TOOL_STATE_DRAWING || canvas_editor->tool_state == QUIXEL_TOOL_STATE_EDITING)
					{
						quixel_tool_dropper_logic(canvas_editor);
					}
					break;
				}
				case QUIXEL_TOOL_LINE:
				{
					if(canvas_editor->tool_state == QUIXEL_TOOL_STATE_DRAWING || canvas_editor->tool_state == QUIXEL_TOOL_STATE_EDITING)
					{
						quixel_tool_line_logic(canvas_editor);
					}
					break;
				}
				case QUIXEL_TOOL_RECTANGLE:
				{
					if(canvas_editor->tool_state == QUIXEL_TOOL_STATE_DRAWING || canvas_editor->tool_state == QUIXEL_TOOL_STATE_EDITING)
					{
						quixel_tool_rectangle_logic(canvas_editor);
					}
					break;
				}
				case QUIXEL_TOOL_FILLED_RECTANGLE:
				{
					if(canvas_editor->tool_state == QUIXEL_TOOL_STATE_DRAWING || canvas_editor->tool_state == QUIXEL_TOOL_STATE_EDITING)
					{
						quixel_tool_filled_rectangle_logic(canvas_editor);
					}
					break;
				}
				case QUIXEL_TOOL_OVAL:
				{
					if(canvas_editor->tool_state == QUIXEL_TOOL_STATE_DRAWING || canvas_editor->tool_state == QUIXEL_TOOL_STATE_EDITING)
					{
						quixel_tool_oval_logic(canvas_editor);
					}
					break;
				}
				case QUIXEL_TOOL_FILLED_OVAL:
				{
					if(canvas_editor->tool_state == QUIXEL_TOOL_STATE_DRAWING || canvas_editor->tool_state == QUIXEL_TOOL_STATE_EDITING)
					{
						quixel_tool_filled_oval_logic(canvas_editor);
					}
					break;
				}
				case QUIXEL_TOOL_SELECTION:
				{
					if(canvas_editor->selection.box.width > 0 && canvas_editor->selection.box.height > 0)
					{
						memcpy(&old_box, &canvas_editor->selection.box, sizeof(QUIXEL_BOX));
						quixel_update_box_handles(&canvas_editor->selection.box, canvas_editor->view_x, canvas_editor->view_y, canvas_editor->view_zoom);
						quixel_box_logic(&canvas_editor->selection.box, canvas_editor->view_x, canvas_editor->view_y, canvas_editor->view_zoom, d->x, d->y);
						if(!canvas_editor->selection.floating && (canvas_editor->selection.box.start_x != old_box.start_x || canvas_editor->selection.box.start_y != old_box.start_y))
						{
							if(canvas_editor->selection.box.state == QUIXEL_BOX_STATE_MOVING)
							{
								float_selection(canvas_editor, &old_box);
							}
						}
					}
					break;
				}
			}
			break;
		}
		case MSG_IDLE:
		{
			update_window_title(canvas_editor);
//			update_cursor(canvas_editor);
			if(!color_equal(canvas_editor->left_base_color, canvas_editor->last_left_base_color))
			{
				canvas_editor->left_color = canvas_editor->left_base_color;
				canvas_editor->last_left_base_color = canvas_editor->left_base_color;
				quixel_canvas_editor_update_pick_colors(canvas_editor);
			}
			if(!color_equal(canvas_editor->left_color, canvas_editor->last_left_color))
			{
				canvas_editor->last_left_color = canvas_editor->left_color;
				canvas_editor->left_shade_slider_element->d2 = get_shade(canvas_editor->left_color) * 1000.0;
				canvas_editor->left_alpha_slider_element->d2 = get_alpha(canvas_editor->left_color) * 1000.0;
			}
			canvas_editor->left_shade_color = shade_color(canvas_editor->left_base_color, (float)canvas_editor->left_shade_slider_element->d2 / 1000.0);
			if(!color_equal(canvas_editor->left_shade_color, canvas_editor->last_left_shade_color))
			{
				canvas_editor->left_color = canvas_editor->left_shade_color;
				canvas_editor->last_left_shade_color = canvas_editor->left_shade_color;
			}
			canvas_editor->left_alpha_color = alpha_color(canvas_editor->left_shade_color, (float)canvas_editor->left_alpha_slider_element->d2 / 1000.0);
			if(!color_equal(canvas_editor->left_alpha_color, canvas_editor->last_left_alpha_color))
			{
				canvas_editor->left_color = canvas_editor->left_alpha_color;
				canvas_editor->last_left_alpha_color = canvas_editor->left_alpha_color;
			}
			if(!color_equal(canvas_editor->right_base_color, canvas_editor->last_right_base_color))
			{
				canvas_editor->right_color = canvas_editor->right_base_color;
				canvas_editor->last_right_base_color = canvas_editor->right_base_color;
				quixel_canvas_editor_update_pick_colors(canvas_editor);
			}
			if(!color_equal(canvas_editor->right_color, canvas_editor->last_right_color))
			{
				canvas_editor->last_right_color = canvas_editor->right_color;
				canvas_editor->right_shade_slider_element->d2 = get_shade(canvas_editor->right_color) * 1000.0;
				canvas_editor->right_alpha_slider_element->d2 = get_alpha(canvas_editor->right_color) * 1000.0;
			}
			canvas_editor->right_shade_color = shade_color(canvas_editor->right_base_color, (float)canvas_editor->right_shade_slider_element->d2 / 1000.0);
			if(!color_equal(canvas_editor->right_shade_color, canvas_editor->last_right_shade_color))
			{
				canvas_editor->right_color = canvas_editor->right_shade_color;
				canvas_editor->last_right_shade_color = canvas_editor->right_shade_color;
			}
			canvas_editor->right_alpha_color = alpha_color(canvas_editor->right_shade_color, (float)canvas_editor->right_alpha_slider_element->d2 / 1000.0);
			if(!color_equal(canvas_editor->right_alpha_color, canvas_editor->last_right_alpha_color))
			{
				canvas_editor->right_color = canvas_editor->right_alpha_color;
				canvas_editor->last_right_alpha_color = canvas_editor->right_alpha_color;
			}
			if(t3f_key[ALLEGRO_KEY_LEFT])
			{
				canvas_editor->view_x--;
			}
			if(t3f_key[ALLEGRO_KEY_RIGHT])
			{
				canvas_editor->view_x++;
			}
			if(t3f_key[ALLEGRO_KEY_UP])
			{
				canvas_editor->view_y--;
			}
			if(t3f_key[ALLEGRO_KEY_DOWN])
			{
				canvas_editor->view_y++;
			}
			if(t3f_key[ALLEGRO_KEY_MINUS])
			{
				if(canvas_editor->view_zoom > 1)
				{
					cx = canvas_editor->view_x + (d->w / canvas_editor->view_zoom) / 2;
					cy = canvas_editor->view_y + (d->h / canvas_editor->view_zoom) / 2;
					canvas_editor->view_zoom--;
					canvas_editor->view_x = cx - (d->w / canvas_editor->view_zoom) / 2;
					canvas_editor->view_y = cy - (d->h / canvas_editor->view_zoom) / 2;
				}
				t3f_key[ALLEGRO_KEY_MINUS] = 0;
			}
			if(t3f_key[ALLEGRO_KEY_EQUALS])
			{
				cx = canvas_editor->view_x + (d->w / canvas_editor->view_zoom) / 2;
				cy = canvas_editor->view_y + (d->h / canvas_editor->view_zoom) / 2;
				canvas_editor->view_zoom++;
				canvas_editor->view_x = cx - (d->w / canvas_editor->view_zoom) / 2;
				canvas_editor->view_y = cy - (d->h / canvas_editor->view_zoom) / 2;
				t3f_key[ALLEGRO_KEY_EQUALS] = 0;
			}
			if(t3f_key[ALLEGRO_KEY_H])
			{
				canvas_editor->canvas->layer[canvas_editor->current_layer]->flags ^= QUIXEL_CANVAS_FLAG_HIDDEN;
				t3f_key[ALLEGRO_KEY_H] = 0;
			}
			canvas_editor->hover_x = canvas_editor->view_x + (t3f_mouse_x - d->x) / canvas_editor->view_zoom;
			canvas_editor->hover_y = canvas_editor->view_y + (t3f_mouse_y - d->y) / canvas_editor->view_zoom;

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
			if(t3f_key[ALLEGRO_KEY_F])
			{
				sprintf(frame_name, "Frame %d", canvas_editor->canvas->frame_max + 1);
				quixel_get_canvas_dimensions(canvas_editor->canvas, &frame_x, &frame_y, &frame_width, &frame_height, 0);
				if(quixel_add_canvas_frame(canvas_editor->canvas, frame_name, frame_x, frame_y, frame_width, frame_height))
				{
					canvas_editor->current_frame = canvas_editor->canvas->frame_max - 1;
				}
				t3f_key[ALLEGRO_KEY_F] = 0;
			}
			if(t3f_key[ALLEGRO_KEY_G])
			{
				quixel_remove_canvas_frame(canvas_editor->canvas, canvas_editor->current_frame);
				if(canvas_editor->current_frame >= canvas_editor->canvas->frame_max)
				{
					canvas_editor->current_frame = canvas_editor->canvas->frame_max - 1;
				}
				if(canvas_editor->current_frame < 0)
				{
					canvas_editor->current_frame = 0;
				}
				t3f_key[ALLEGRO_KEY_G] = 0;
			}
			if(t3f_key[ALLEGRO_KEY_C])
			{
				quixel_get_canvas_dimensions(canvas_editor->canvas, &frame_x, &frame_y, &frame_width, &frame_height, 0);
				canvas_editor->view_x = frame_x + frame_width / 2 - (d->w / 2) / canvas_editor->view_zoom;
				canvas_editor->view_y = frame_y + frame_height / 2 - (d->h / 2) / canvas_editor->view_zoom;
				t3f_key[ALLEGRO_KEY_C] = 0;
			}
			if(t3f_key[ALLEGRO_KEY_DELETE])
			{
				canvas_editor->signal = QUIXEL_CANVAS_EDITOR_SIGNAL_DELETE_LAYER;
				t3f_key[ALLEGRO_KEY_DELETE] = 0;
			}
			if(canvas_editor->tool_state == QUIXEL_TOOL_STATE_OFF)
			{
				canvas_editor->hover_color = quixel_get_canvas_pixel(canvas_editor->canvas, canvas_editor->current_layer, canvas_editor->hover_x, canvas_editor->hover_y);
			}
			if(canvas_editor->selection.box.width > 0 && canvas_editor->selection.box.height > 0 && canvas_editor->selection.box.state == QUIXEL_BOX_STATE_IDLE)
			{
//				quixel_box_idle_logic(&canvas_editor->selection.box, canvas_editor->view_x, canvas_editor->view_y, canvas_editor->view_zoom, d->x, d->y);
			}
			break;
		}
		case MSG_DRAW:
		{
			ALLEGRO_STATE old_state;
			ALLEGRO_TRANSFORM identity;
			int i;

			al_store_state(&old_state, ALLEGRO_STATE_TRANSFORM);
			al_identity_transform(&identity);
			al_use_transform(&identity);
			if(canvas_editor->tool_state != QUIXEL_TOOL_STATE_DRAWING)
			{
				quixel_render_canvas(canvas_editor->canvas, canvas_editor->view_x, canvas_editor->view_y, canvas_editor->view_zoom, d->x, d->y, d->w, d->h);
			}
			else
			{
				for(i = 0; i < canvas_editor->current_layer; i++)
				{
					quixel_render_canvas_layer(canvas_editor->canvas, i, canvas_editor->view_x, canvas_editor->view_y, canvas_editor->view_zoom, d->x, d->y, d->w, d->h);
				}
				al_draw_scaled_bitmap(canvas_editor->scratch_bitmap, 0, 0,  al_get_bitmap_width(canvas_editor->scratch_bitmap), al_get_bitmap_height(canvas_editor->scratch_bitmap), d->x, d->y, al_get_bitmap_width(canvas_editor->scratch_bitmap) * canvas_editor->view_zoom, al_get_bitmap_height(canvas_editor->scratch_bitmap) * canvas_editor->view_zoom, 0);
			}
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
					al_draw_text(theme->state[0].font[0], color, d->x + (canvas_editor->canvas->frame[i]->box.start_x - canvas_editor->view_x) * canvas_editor->view_zoom, d->y + (canvas_editor->canvas->frame[i]->box.start_y - canvas_editor->view_y) * canvas_editor->view_zoom - al_get_font_line_height(theme->state[0].font[0]) - QUIXEL_UI_ELEMENT_SPACE, 0, canvas_editor->canvas->frame[i]->name);
				}
				al_draw_rectangle(d->x + (canvas_editor->canvas->frame[i]->box.start_x - canvas_editor->view_x) * canvas_editor->view_zoom - 1.0 + 0.5, d->y + (canvas_editor->canvas->frame[i]->box.start_y - canvas_editor->view_y) * canvas_editor->view_zoom - 1.0 + 0.5, d->x + (canvas_editor->canvas->frame[i]->box.start_x + canvas_editor->canvas->frame[i]->box.width - canvas_editor->view_x) * canvas_editor->view_zoom + 0.5, d->y + (canvas_editor->canvas->frame[i]->box.start_y + canvas_editor->canvas->frame[i]->box.height - canvas_editor->view_y) * canvas_editor->view_zoom + 0.5, color, 1.0);
			}
			if(canvas_editor->selection.box.width > 0 && canvas_editor->selection.box.height > 0)
			{
				if(canvas_editor->selection.floating)
				{
					al_draw_scaled_bitmap(canvas_editor->scratch_bitmap, 0, 0,  canvas_editor->selection.box.width, canvas_editor->selection.box.height, d->x + (canvas_editor->selection.box.start_x - canvas_editor->view_x) * canvas_editor->view_zoom, d->y + (canvas_editor->selection.box.start_y - canvas_editor->view_y) * canvas_editor->view_zoom, canvas_editor->selection.box.width * canvas_editor->view_zoom, canvas_editor->selection.box.height * canvas_editor->view_zoom, 0);
				}
				quixel_box_render(&canvas_editor->selection.box, 0, canvas_editor->view_x, canvas_editor->view_y, canvas_editor->view_zoom, d->x, d->y);
			}
			al_restore_state(&old_state);
			break;
		}
	}
	return D_O_K;
}
