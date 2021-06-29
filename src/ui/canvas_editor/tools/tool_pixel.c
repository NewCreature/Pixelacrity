#include "t3f/t3f.h"
#include "modules/queue.h"
#include "modules/primitives.h"
#include "modules/canvas/canvas.h"
#include "modules/canvas/canvas_helpers.h"
#include "ui/canvas_editor/canvas_editor.h"
#include "tool_pixel.h"

static PA_QUEUE * pixel_queue = NULL;

static void render_queued_lines(PA_CANVAS_EDITOR * cep)
{
	ALLEGRO_TRANSFORM identity;
	ALLEGRO_STATE old_state;
	PA_QUEUE_NODE * current_node;
	int current_x, current_y;
	int old_x, old_y;
	float start_x, start_y, end_x, end_y;
	int count = 0;

	al_store_state(&old_state, ALLEGRO_STATE_TARGET_BITMAP | ALLEGRO_STATE_TRANSFORM | ALLEGRO_STATE_BLENDER);
	al_set_target_bitmap(cep->scratch_bitmap);
	al_identity_transform(&identity);
	al_use_transform(&identity);
	al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);
	al_clear_to_color(al_map_rgba_f(0.0, 0.0, 0.0, 0.0));
	pa_render_canvas_layer(cep->canvas, cep->current_layer, cep->view_x, cep->view_y, 1, 0, 0, cep->editor_element->w, cep->editor_element->h);

	current_node = pixel_queue->current;
	current_x = pixel_queue->current->x;
	current_y = pixel_queue->current->y;
	old_x = current_x;
	old_y = current_y;
	start_x = current_x - cep->view_x;
	start_y = current_y - cep->view_y;
	while(1)
	{
		count++;
		current_node = current_node->previous;
		if(current_node)
		{
			old_x = current_x;
			old_y = current_y;
			current_x = current_node->x;
			current_y = current_node->y;
			start_x = current_x - cep->view_x;
			start_y = current_y - cep->view_y;
			end_x = old_x - cep->view_x;
			end_y = old_y - cep->view_y;
			pa_draw_line(start_x, start_y, end_x, end_y, cep->brush, cep->click_color);
		}
		else
		{
			break;
		}
	}
	al_restore_state(&old_state);
}

bool pa_tool_pixel_start(PA_CANVAS_EDITOR * cep)
{
	pixel_queue = pa_create_queue();
	if(!pixel_queue)
	{
		goto fail;
	}
	if(!pa_queue_push(pixel_queue, cep->click_x, cep->click_y))
	{
		goto fail;
	}
	if(!pa_queue_push(pixel_queue, cep->click_x, cep->click_y))
	{
		goto fail;
	}
	render_queued_lines(cep);
	return true;

	fail:
	{
		if(pixel_queue)
		{
			pa_destroy_queue(pixel_queue);
		}
		return false;
	}
}

void pa_tool_pixel_logic(PA_CANVAS_EDITOR * cep)
{
	if(cep->hover_x != pixel_queue->current->x || cep->hover_y != pixel_queue->current->y)
	{
		pa_queue_push(pixel_queue, cep->hover_x, cep->hover_y);
		render_queued_lines(cep);
	}
}

void pa_tool_pixel_finish(PA_CANVAS_EDITOR * cep)
{
	PA_QUEUE_NODE * current_node;
	int current_x, current_y;
	int old_x, old_y;

	current_node = pixel_queue->current;
	current_x = pixel_queue->current->x + cep->shift_x * cep->canvas->bitmap_size;
	current_y = pixel_queue->current->y + cep->shift_y * cep->canvas->bitmap_size;
	old_x = current_x;
	old_y = current_y;
	while(1)
	{
		current_node = current_node->previous;
		if(current_node)
		{
			old_x = current_x;
			old_y = current_y;
			current_x = current_node->x + cep->shift_x * cep->canvas->bitmap_size;
			current_y = current_node->y + cep->shift_y * cep->canvas->bitmap_size;
			pa_draw_primitive_to_canvas(cep->canvas, cep->current_layer, old_x, old_y, current_x, current_y, cep->brush, cep->click_color, PA_RENDER_COPY, cep->conditional_copy_shader, pa_draw_line);
			al_use_shader(cep->conditional_copy_shader);
		}
		else
		{
			break;
		}
	}
	pa_destroy_queue(pixel_queue);
}
