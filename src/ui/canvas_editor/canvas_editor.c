#include "t3f/t3f.h"
#include "t3gui/t3gui.h"
#include "canvas_editor.h"
#include "modules/canvas/canvas_helpers.h"
#include "modules/primitives.h"
#include "modules/pixel_shader.h"
#include "modules/color.h"
#include "modules/dynamic_array.h"
#include "undo.h"
#include "undo_selection.h"
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

static ALLEGRO_BITMAP * create_default_brush(void)
{
	ALLEGRO_BITMAP * bp;
	ALLEGRO_STATE old_state;

	al_store_state(&old_state, ALLEGRO_STATE_TARGET_BITMAP | ALLEGRO_STATE_NEW_BITMAP_PARAMETERS);
	al_set_new_bitmap_flags(0);
	bp = al_create_bitmap(1, 1);
	if(bp)
	{
		al_set_target_bitmap(bp);
		al_clear_to_color(t3f_color_white);
	}
	al_restore_state(&old_state);
	return bp;
}

void pa_reset_canvas_editor(PA_CANVAS_EDITOR * cep)
{
	cep->view_x = 0;
	cep->view_y = 0;
	cep->view_fx = 0.0;
	cep->view_fy = 0.0;
	cep->view_zoom = 8;
	cep->current_layer = 0;
	cep->modified = 0;
	strcpy(cep->canvas_path, "");
	cep->backup_tick = PA_BACKUP_INTERVAL;
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
	t3f_debug_message("Use alpha blend shader\n");
	al_use_shader(cep->standard_shader);

	al_store_state(&old_state, ALLEGRO_STATE_NEW_BITMAP_PARAMETERS);
	al_set_new_bitmap_flags(0);
	cep->brush = create_default_brush();
	if(!cep->brush)
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
	if(cep->peg_bitmap)
	{
		al_destroy_bitmap(cep->peg_bitmap);
	}
	if(cep->scratch_bitmap)
	{
		al_destroy_bitmap(cep->scratch_bitmap);
	}
	free(cep);
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
		pa_get_canvas_dimensions(cep->canvas, &x, &y, &w, &h, 0);
		cep->view_x = x + w / 2 - (cep->editor_element->w / cep->view_zoom) / 2;
		cep->view_y = y + h / 2 - (cep->editor_element->h / cep->view_zoom) / 2;
	}
	cep->view_fx = cep->view_x;
	cep->view_fy = cep->view_y;
}

void pa_set_canvas_editor_zoom(PA_CANVAS_EDITOR * cep, int level)
{
	int cx, cy;
	int amount = cep->view_zoom - level;

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
	}
}

void pa_select_canvas_editor_tool(PA_CANVAS_EDITOR * cep, int tool)
{
	if(cep->current_tool == PA_TOOL_SELECTION && tool != PA_TOOL_SELECTION)
	{
		if(cep->selection.bitmap)
		{
			pa_unfloat_canvas_editor_selection(cep, &cep->selection.box);
		}
		pa_clear_canvas_editor_selection(cep);
	}
	cep->current_tool = tool;
}

void pa_select_canvas_editor_layer(PA_CANVAS_EDITOR * cep, int layer)
{
	if(cep->current_layer != layer && layer < cep->canvas->layer_max)
	{
		if(cep->selection.box.width > 0 && cep->selection.box.height > 0)
		{
			if(!cep->selection.bitmap)
			{
				pa_clear_canvas_editor_selection(cep);
			}
		}
		cep->current_layer = layer;
	}
}

bool pa_import_image(PA_CANVAS_EDITOR * cep, const char * fn)
{
	int x, y;

	t3f_debug_message("Enter pa_import_image()\n");
	if(cep->selection.bitmap)
	{
		pa_handle_unfloat_canvas_editor_selection(cep, &cep->selection.box);
	}
	cep->selection.bitmap = (ALLEGRO_BITMAP **)pa_malloc(sizeof(ALLEGRO_BITMAP *), cep->canvas->layer_max);
	if(cep->selection.bitmap)
	{
		cep->selection.bitmap[0] = al_load_bitmap_flags(fn, ALLEGRO_NO_PREMULTIPLIED_ALPHA);
		if(cep->selection.bitmap[0])
		{
			pa_select_canvas_editor_tool(cep, PA_TOOL_SELECTION);
			x = cep->view_x + cep->view_width / 2 - al_get_bitmap_width(cep->selection.bitmap[0]) / 2;
			y = cep->view_y + cep->view_height / 2 - al_get_bitmap_height(cep->selection.bitmap[0]) / 2;
			pa_initialize_box(&cep->selection.box, x, y, al_get_bitmap_width(cep->selection.bitmap[0]), al_get_bitmap_height(cep->selection.bitmap[0]), cep->peg_bitmap);
			pa_update_box_handles(&cep->selection.box, cep->view_x, cep->view_y, cep->view_zoom);
		}
	}
	t3f_debug_message("Exit pa_import_image()\n");
	return true;

}
