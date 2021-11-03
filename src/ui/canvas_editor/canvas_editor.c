#include "t3f/t3f.h"
#include "t3gui/t3gui.h"
#include "canvas_editor.h"
#include "brush.h"
#include "modules/canvas/canvas_helpers.h"
#include "modules/primitives.h"
#include "modules/pixel_shader.h"
#include "modules/color.h"
#include "modules/dynamic_array.h"
#include "undo/undo.h"
#include "selection.h"
#include "clipboard.h"
#include "selection.h"

static int get_config_val(ALLEGRO_CONFIG * cp, const char * section, const char * key, int default_val)
{
	const char * val;

	val = al_get_config_value(cp, section, key);
	if(val)
	{
		return atoi(val);
	}
	return default_val;
}

static void set_config_val(ALLEGRO_CONFIG * cp, const char * section, const char * key, int val)
{
	char buf[256] = {0};

	sprintf(buf, "%d", val);
	al_set_config_value(cp, section, key, buf);
}

void pa_reset_canvas_editor(PA_CANVAS_EDITOR * cep)
{
	cep->want_cursor = ALLEGRO_SYSTEM_MOUSE_CURSOR_PRECISION;
	cep->view_x = 0;
	cep->view_y = 0;
	cep->view_fx = 0.0;
	cep->view_fy = 0.0;
	cep->view_zoom = 8;
	cep->current_layer = 0;
	cep->hover_frame = -1;
	cep->modified = 0;
	strcpy(cep->canvas_path, "");
	cep->backup_tick = PA_BACKUP_INTERVAL;
	pa_clear_canvas_editor_selection(cep);
}

PA_CANVAS_EDITOR * pa_create_canvas_editor(PA_CANVAS * cp)
{
	PA_CANVAS_EDITOR * cep;
	ALLEGRO_STATE old_state;

	cep = malloc(sizeof(PA_CANVAS_EDITOR));
	if(!cep)
	{
		goto fail;
	}
	memset(cep, 0, sizeof(PA_CANVAS_EDITOR));

	t3f_debug_message("Create standard shader\n");
	cep->standard_shader = pa_create_pixel_shader("data/shaders/standard_shader.glsl");
	if(!cep->standard_shader)
	{
		printf("Error initializing standard shader!\n");
		return false;
	}
	t3f_debug_message("Create pre-multiplied alpha shader\n");
	cep->premultiplied_alpha_shader = pa_create_pixel_shader("data/shaders/premultiplied_alpha_shader.glsl");
	if(!cep->premultiplied_alpha_shader)
	{
		printf("Error initializing pre-multiplied alpha shader!\n");
		return false;
	}
	t3f_debug_message("Create conditional copy shader\n");
	cep->conditional_copy_shader = pa_create_pixel_shader("data/shaders/conditional_copy_shader.glsl");
	if(!cep->conditional_copy_shader)
	{
		printf("Error initializing conditional copy shader!\n");
		return false;
	}
	t3f_debug_message("Create solid shader\n");
	cep->solid_shader = pa_create_pixel_shader("data/shaders/solid_shader.glsl");
	if(!cep->solid_shader)
	{
		printf("Error initializing solid shader!\n");
		return false;
	}
	t3f_debug_message("Use alpha blend shader\n");
	al_use_shader(cep->standard_shader);

	cep->view = t3f_create_view(0, 0, al_get_display_width(t3f_display), al_get_display_height(t3f_display), al_get_display_width(t3f_display) / 2, al_get_display_height(t3f_display) / 2, T3F_NO_SCALE);
	if(!cep->view)
	{
		goto fail;
	}
	al_store_state(&old_state, ALLEGRO_STATE_NEW_BITMAP_PARAMETERS);
	al_set_new_bitmap_flags(0);
	cep->color_scratch_bitmap = al_create_bitmap(1, 1);
	if(!cep->color_scratch_bitmap)
	{
		goto fail;
	}
	cep->brush = pa_create_default_brush();
	if(!cep->brush)
	{
		goto fail;
	}
	cep->tool_bitmap = al_create_bitmap(al_get_display_width(t3f_display), al_get_display_height(t3f_display));
	al_restore_state(&old_state);
	if(!cep->tool_bitmap)
	{
		goto fail;
	}
	cep->scratch_bitmap = al_create_bitmap(al_get_display_width(t3f_display), al_get_display_height(t3f_display));
	al_restore_state(&old_state);
	if(!cep->scratch_bitmap)
	{
		goto fail;
	}
	cep->peg_bitmap = al_load_bitmap_flags("data/graphics/peg.png", ALLEGRO_NO_PREMULTIPLIED_ALPHA);
	if(!cep->peg_bitmap)
	{
		goto fail;
	}
	cep->multilayer_bitmap = al_load_bitmap_flags("data/graphics/multilayer.png", ALLEGRO_NO_PREMULTIPLIED_ALPHA);
	if(!cep->multilayer_bitmap)
	{
		goto fail;
	}
	cep->singlelayer_bitmap = al_load_bitmap_flags("data/graphics/singlelayer.png", ALLEGRO_NO_PREMULTIPLIED_ALPHA);
	if(!cep->singlelayer_bitmap)
	{
		goto fail;
	}
	cep->canvas = cp;
	pa_reset_canvas_editor(cep);
	return cep;

	fail:
	{
		if(cep)
		{
			pa_destroy_canvas_editor(cep);
		}
		return NULL;
	}
}

void pa_destroy_canvas_editor(PA_CANVAS_EDITOR * cep)
{
	if(cep->export_path)
	{
		free(cep->export_path);
	}
	pa_free_clipboard(cep);
	if(cep->multilayer_bitmap)
	{
		al_destroy_bitmap(cep->multilayer_bitmap);
	}
	if(cep->peg_bitmap)
	{
		al_destroy_bitmap(cep->peg_bitmap);
	}
	if(cep->tool_bitmap)
	{
		al_destroy_bitmap(cep->tool_bitmap);
	}
	if(cep->scratch_bitmap)
	{
		al_destroy_bitmap(cep->scratch_bitmap);
	}
	if(cep->color_scratch_bitmap)
	{
		al_destroy_bitmap(cep->color_scratch_bitmap);
	}
	if(cep->view)
	{
		t3f_destroy_view(cep->view);
	}
	free(cep);
}

static void load_palette(PA_CANVAS_EDITOR * cep)
{
	int i;
	char buf[32];
	const char * val;

	for(i = 0; i < PA_COLOR_PICKER_SHADES * PA_COLOR_PICKER_SHADES; i++)
	{
		sprintf(buf, "color_%d", i);
		val = al_get_config_value(cep->config, "Palette", buf);
		if(val)
		{
			cep->palette[i] = pa_get_color_from_html(val);
		}
	}
}

static void save_palette(PA_CANVAS_EDITOR * cep)
{
	int i;
	char buf[32];
	char val[32];
	unsigned char r, g, b, a;

	for(i = 0; i < PA_COLOR_PICKER_SHADES * PA_COLOR_PICKER_SHADES; i++)
	{
		sprintf(buf, "color_%d", i);
		al_unmap_rgba(cep->palette[i], &r, &g, &b, &a);
		sprintf(val, "%02X%02X%02X%02X", r, g, b, a);
		al_set_config_value(cep->config, "Palette", buf, val);
	}
}

bool pa_load_canvas_editor_state(PA_CANVAS_EDITOR * cep, const char * fn)
{
	char buf[64];
	int i;
	const char * val;

	cep->config = al_load_config_file(fn);
	if(cep->config)
	{
		cep->view_x = get_config_val(cep->config, "State", "view_x", cep->view_x);
		cep->view_y = get_config_val(cep->config, "State", "view_y", cep->view_y);
		cep->view_fx = cep->view_x;
		cep->view_fy = cep->view_y;
		cep->view_zoom = get_config_val(cep->config, "State", "view_zoom", 8);
		pa_set_canvas_editor_zoom(cep, cep->view_zoom);
		cep->current_tool = get_config_val(cep->config, "State", "current_tool", 0);
		cep->current_layer = get_config_val(cep->config, "State", "current_layer", 0);
		val = al_get_config_value(cep->config, "State", "export_path");
		if(val)
		{
			cep->export_path = strdup(val);
		}
		else
		{
			cep->export_path = NULL;
		}
		for(i = 0; i < cep->canvas->frame_max; i++)
		{
			sprintf(buf, "Frame %d", i);
			val = al_get_config_value(cep->config, buf, "export_path");
			if(val)
			{
				cep->canvas->frame[i]->export_path = strdup(val);
			}
			else
			{
				cep->canvas->frame[i]->export_path = NULL;
			}
			load_palette(cep);
		}
		return true;
	}
	return false;
}

bool pa_save_canvas_editor_state(PA_CANVAS_EDITOR * cep, const char * fn)
{
	int i;
	char buf[64];

	if(!cep->config)
	{
		cep->config = al_create_config();
		if(!cep->config)
		{
			return false;
		}
	}
	set_config_val(cep->config, "State", "view_x", cep->view_x);
	set_config_val(cep->config, "State", "view_y", cep->view_y);
	set_config_val(cep->config, "State", "view_zoom", cep->view_zoom);
	set_config_val(cep->config, "State", "current_tool", cep->current_tool);
	set_config_val(cep->config, "State", "current_layer", cep->current_layer);
	if(cep->export_path)
	{
		al_set_config_value(cep->config, "State", "export_path", cep->export_path);
	}
	for(i = 0; i < cep->canvas->frame_max; i++)
	{
		if(cep->canvas->frame[i]->export_path)
		{
			sprintf(buf, "Frame %d", i);
			al_set_config_value(cep->config, buf, "export_path", cep->canvas->frame[i]->export_path);
		}
	}
	save_palette(cep);
	return al_save_config_file(fn, cep->config);
}

void pa_set_color(PA_COLOR_INFO * cip, ALLEGRO_COLOR color)
{
	cip->base_color = color;
	cip->last_base_color = color;
	cip->color = color;
	cip->last_color = color;
	cip->shade_color = color;
	cip->last_shade_color = color;
	cip->alpha_color = color;
	cip->last_alpha_color = color;

	cip->shade_slider_element->d2 = pa_get_color_shade(color) * 1000.0;
	cip->alpha_slider_element->d2 = pa_get_color_alpha(color) * 1000.0;
	cip->old_shade_slider_d2 = cip->shade_slider_element->d2;
	cip->old_alpha_slider_d2 = cip->alpha_slider_element->d2;
}

void pa_center_canvas_editor(PA_CANVAS_EDITOR * cep, int frame)
{
	int x, y, w, h;

	if(frame < cep->canvas->frame_max)
	{
		cep->view_x = cep->canvas->frame[frame]->box.start_x + cep->canvas->frame[frame]->box.width / 2 - (cep->editor_element->w / cep->view_zoom) / 2;
		cep->view_y = cep->canvas->frame[frame]->box.start_y + cep->canvas->frame[frame]->box.height / 2 - (cep->editor_element->h / cep->view_zoom) / 2;
	}
	else
	{
		pa_get_canvas_dimensions(cep->canvas, &x, &y, &w, &h, 0, false);
		cep->view_x = x + w / 2 - (cep->editor_element->w / cep->view_zoom) / 2;
		cep->view_y = y + h / 2 - (cep->editor_element->h / cep->view_zoom) / 2;
	}
	cep->view_fx = cep->view_x;
	cep->view_fy = cep->view_y;
}

void pa_set_canvas_editor_zoom(PA_CANVAS_EDITOR * cep, int level)
{
	int cx, cy;

	if(level > 0)
	{
		if(cep->editor_element)
		{
			cx = cep->view_x + (cep->editor_element->w / cep->view_zoom) / 2;
			cy = cep->view_y + (cep->editor_element->h / cep->view_zoom) / 2;
		}
		cep->view_zoom = level;
		if(cep->editor_element)
		{
			cep->view_x = cx - (cep->editor_element->w / cep->view_zoom) / 2;
			cep->view_y = cy - (cep->editor_element->h / cep->view_zoom) / 2;
			cep->view_fx = cep->view_x;
			cep->view_fy = cep->view_y;
			cep->view_width = cep->editor_element->w / cep->view_zoom;
			cep->view_height = cep->editor_element->h / cep->view_zoom;
		}
	}
}

void pa_shift_canvas_editor_variables(PA_CANVAS_EDITOR * cep, int ox, int oy)
{
	int i;

	cep->view_x += ox;
	cep->view_y += oy;
	cep->view_fx = cep->view_x;
	cep->view_fy = cep->view_y;
	cep->click_x += ox;
	cep->click_y += oy;
	cep->release_x += ox;
	cep->release_y += oy;
	cep->hover_x += ox;
	cep->hover_y += oy;
	cep->snap_end_x += ox;
	cep->snap_end_y += oy;
	cep->scratch_offset_x += ox;
	cep->scratch_offset_y += oy;
	cep->selection.box.start_x += ox;
	cep->selection.box.start_y += oy;
	cep->selection.box.middle_x += ox;
	cep->selection.box.middle_y += oy;
	cep->selection.box.end_x += ox;
	cep->selection.box.end_y += oy;
	for(i = 0; i < cep->canvas->frame_max; i++)
	{
		cep->canvas->frame[i]->box.start_x += ox;
		cep->canvas->frame[i]->box.start_y += oy;
		cep->canvas->frame[i]->box.end_x += ox;
		cep->canvas->frame[i]->box.end_y += oy;
	}
	for(i = 0; i < cep->canvas->layer_max; i++)
	{
		cep->canvas->layer[i]->offset_x += ox;
		cep->canvas->layer[i]->offset_y += oy;
	}
}

void pa_select_canvas_editor_tool(PA_CANVAS_EDITOR * cep, int tool)
{
	if(cep->current_tool == PA_TOOL_SELECTION && tool != PA_TOOL_SELECTION)
	{
		if(cep->selection.bitmap_stack)
		{
			pa_unfloat_canvas_editor_selection(cep, &cep->selection.box);
		}
		pa_clear_canvas_editor_selection(cep);
	}
	if(cep->tool_state == PA_TOOL_STATE_OFF)
	{
		cep->current_tool = tool;
		t3f_refresh_menus();
	}
}

void pa_select_canvas_editor_layer(PA_CANVAS_EDITOR * cep, int layer)
{
	if(cep->current_layer != layer && layer < cep->canvas->layer_max)
	{
		if(cep->selection.box.width > 0 && cep->selection.box.height > 0)
		{
			if(!cep->selection.bitmap_stack)
			{
				pa_clear_canvas_editor_selection(cep);
			}
		}
		cep->current_layer = layer;
		t3f_refresh_menus();
	}
}

bool pa_import_image(PA_CANVAS_EDITOR * cep, const char * fn)
{
	int i, x, y;
	ALLEGRO_BITMAP * bp;

	t3f_debug_message("Enter pa_import_image()\n");
	if(cep->selection.bitmap_stack)
	{
		pa_unfloat_canvas_editor_selection(cep, &cep->selection.box);
	}
	bp = al_load_bitmap_flags(fn, ALLEGRO_NO_PREMULTIPLIED_ALPHA);
	if(bp)
	{
		cep->selection.bitmap_stack = pa_create_bitmap_stack(cep->canvas->layer_max, al_get_bitmap_width(bp), al_get_bitmap_height(bp));
		if(cep->selection.bitmap_stack)
		{
			pa_select_canvas_editor_tool(cep, PA_TOOL_SELECTION);
			x = cep->view_x + cep->view_width / 2 - al_get_bitmap_width(bp) / 2;
			y = cep->view_y + cep->view_height / 2 - al_get_bitmap_height(bp) / 2;
			pa_initialize_box(&cep->selection.box, x, y, al_get_bitmap_width(bp), al_get_bitmap_height(bp));
			for(i = 0; i < cep->canvas->layer_max; i++)
			{
				pa_add_layer_to_bitmap_stack(cep->selection.bitmap_stack, i == cep->current_layer ? bp : NULL, i);
			}
			pa_update_box_handles(&cep->selection.box, cep->view_x, cep->view_y, cep->view_zoom);
			cep->selection.layer_max = cep->canvas->layer_max;
			cep->selection.layer = cep->current_layer;
		}
	}
	t3f_debug_message("Exit pa_import_image()\n");
	return true;

}

void pa_update_mouse_variables(PA_CANVAS_EDITOR * cep)
{
	cep->last_hover_x = cep->hover_x;
	cep->hover_x = cep->view_x + (t3gui_get_mouse_x() - cep->editor_element->x) / cep->view_zoom;
	cep->last_hover_y = cep->hover_y;
	cep->hover_y = cep->view_y + (t3gui_get_mouse_y() - cep->editor_element->y) / cep->view_zoom;
}

void pa_initialize_tool_variables(PA_CANVAS_EDITOR * cep)
{
	int w, h;

	cep->scratch_offset_x = cep->view_x;
	cep->scratch_offset_y = cep->view_y;
	w = al_get_bitmap_width(cep->brush);
	h = al_get_bitmap_height(cep->brush);
	cep->tool_left = cep->hover_x - w / 2;
	cep->tool_right = cep->hover_x + w / 2 + w % 2;
	cep->tool_top = cep->hover_y - h / 2;
	cep->tool_bottom = cep->hover_y + h / 2 + h % 2;
}

void pa_update_tool_variables(PA_CANVAS_EDITOR * cep)
{
	int w, h;

	cep->scratch_offset_x = cep->view_x;
	cep->scratch_offset_y = cep->view_y;
	w = al_get_bitmap_width(cep->brush);
	h = al_get_bitmap_height(cep->brush);
	if(cep->hover_x - w / 2 < cep->tool_left)
	{
		cep->tool_left = cep->hover_x - w / 2;
	}
	if(cep->hover_x + w / 2 + w % 2 > cep->tool_right)
	{
		cep->tool_right = cep->hover_x + w / 2 + w % 2;
	}
	if(cep->hover_y - h / 2 < cep->tool_top)
	{
		cep->tool_top = cep->hover_y - h / 2;
	}
	if(cep->hover_y + h / 2 + h % 2 > cep->tool_bottom)
	{
		cep->tool_bottom = cep->hover_y + h / 2 + h % 2;
	}
}

void pa_set_tool_boundaries(PA_CANVAS_EDITOR * cep, int start_x, int start_y, int end_x, int end_y)
{
	cep->tool_left = start_x;
	cep->tool_right = end_x;
	pa_sort_coordinates(&cep->tool_left, &cep->tool_right);
	cep->tool_top = start_y;
	cep->tool_bottom = end_y;
	pa_sort_coordinates(&cep->tool_top, &cep->tool_bottom);
}

void pa_canvas_editor_update_pick_colors(PA_CANVAS_EDITOR * cep)
{
	float new_l, step;
	int i;

	step = 1.0 / (float)(PA_COLOR_PICKER_SHADES - 1);
	for(i = 0; i < PA_COLOR_PICKER_SHADES; i++)
	{
		new_l = step * (float)i;
		cep->left_shade_color[i] = pa_get_real_color(pa_shade_color(cep->left_color.base_color, new_l), cep->color_scratch_bitmap);
	}
	for(i = 0; i < PA_COLOR_PICKER_SHADES; i++)
	{
		new_l = step * (float)i;
		cep->left_alpha_color[i] = pa_get_real_color(pa_alpha_color(cep->left_color.color, new_l), cep->color_scratch_bitmap);
	}
	for(i = 0; i < PA_COLOR_PICKER_SHADES; i++)
	{
		new_l = step * (float)i;
		cep->right_shade_color[i] = pa_get_real_color(pa_shade_color(cep->right_color.base_color, new_l), cep->color_scratch_bitmap);
	}
	for(i = 0; i < PA_COLOR_PICKER_SHADES; i++)
	{
		new_l = step * (float)i;
		cep->right_alpha_color[i] = pa_get_real_color(pa_alpha_color(cep->right_color.color, new_l), cep->color_scratch_bitmap);
	}
}

static bool blank_bitmap(ALLEGRO_BITMAP * bp)
{
	int i, j;
	ALLEGRO_COLOR color;
	unsigned char r, g, b, a;

	if(bp)
	{
		al_lock_bitmap(bp, ALLEGRO_LOCK_READONLY, ALLEGRO_PIXEL_FORMAT_ANY);
		for(i = 0; i < al_get_bitmap_height(bp); i++)
		{
			for(j = 0; j < al_get_bitmap_width(bp); j++)
			{
				color = al_get_pixel(bp, j, i);
				al_unmap_rgba(color, &r, &g, &b, &a);
				if(a > 0)
				{
					al_unlock_bitmap(bp);
					return false;
				}
			}
		}
		al_unlock_bitmap(bp);
	}
	return true;
}

void pa_optimize_canvas(PA_CANVAS_EDITOR * cep, int skip_x, int skip_y)
{
	int i, j, k;
	int left = cep->canvas->layer_width;
	int right = 0;
	int top = cep->canvas->layer_height;
	int bottom = 0;
	int width, height;

	for(i = 0; i < cep->canvas->layer_max; i++)
	{
		for(j = skip_y; j < cep->canvas->layer_height; j++)
		{
			for(k = skip_x; k < cep->canvas->layer_width; k++)
			{
				if(!blank_bitmap(cep->canvas->layer[i]->bitmap[j][k]))
				{
					if(k < left)
					{
						left = k;
					}
					if(k > right)
					{
						right = k;
					}
					if(j < top)
					{
						top = j;
					}
					if(j > bottom)
					{
						bottom = j;
					}
				}
			}
		}
	}
	width = right - left + 1;
	height = bottom - top + 1;
	for(i = 0; i < cep->canvas->layer_max; i++)
	{
		for(j = 0; j < height; j++)
		{
			for(k = 0; k < width; k++)
			{
				cep->canvas->layer[i]->bitmap[j][k] = cep->canvas->layer[i]->bitmap[j + top][k + left];
			}
		}
	}
	cep->canvas->layer_width = width;
	cep->canvas->layer_height = height;
	pa_shift_canvas_editor_variables(cep, -left * cep->canvas->bitmap_size, -top * cep->canvas->bitmap_size);
	for(i = 0; i < cep->canvas->layer_max; i++)
	{
		pa_calculate_canvas_layer_dimensions(cep->canvas, i, &cep->canvas->layer[i]->offset_x, &cep->canvas->layer[i]->offset_y, &cep->canvas->layer[i]->width, &cep->canvas->layer[i]->height);
	}
}
