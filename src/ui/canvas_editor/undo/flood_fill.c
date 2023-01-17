#include "t3f/t3f.h"
#include "t3f/file.h"
#include "modules/canvas/canvas.h"
#include "modules/canvas/canvas_helpers.h"
#include "modules/canvas/flood_fill.h"
#include "modules/queue.h"
#include "ui/canvas_editor/canvas_editor.h"
#include "undo.h"

static bool write_color(ALLEGRO_FILE * fp, ALLEGRO_COLOR color)
{
	unsigned char r, g, b, a;

	al_unmap_rgba(color, &r, &g, &b, &a);
	al_fputc(fp, r);
	al_fputc(fp, g);
	al_fputc(fp, b);
	al_fputc(fp, a);
	return true;
}

bool pa_make_flood_fill_undo(PA_CANVAS_EDITOR * cep, int layer, ALLEGRO_COLOR color, PA_QUEUE * qp, int shift_x, int shift_y, const char * fn)
{
	ALLEGRO_FILE * fp = NULL;
	PA_QUEUE_NODE * current_node;

	t3f_debug_message("Enter pa_make_flood_fill_undo()\n");
	fp = al_fopen(fn, "wb");
	if(!fp)
	{
		printf("fail: %s\n", fn);
		goto fail;
	}
	pa_write_undo_header(fp, cep, PA_UNDO_TYPE_FLOOD_FILL, qp->name);
	al_fwrite16le(fp, cep->current_tool);
	al_fwrite32le(fp, layer);
	write_color(fp, color);
	al_fwrite32le(fp, shift_x);
	al_fwrite32le(fp, shift_y);
	t3f_save_string_f(fp, qp->name);
	al_fwrite32le(fp, pa_get_queue_size(qp));
	current_node = qp->current;
	while(current_node)
	{
		al_fwrite32le(fp, current_node->x);
		al_fwrite32le(fp, current_node->y);
		current_node = current_node->previous;
	}
	al_fclose(fp);
	t3f_debug_message("Exit pa_make_flood_fill_undo()\n");
	return true;

	fail:
	{
		t3f_debug_message("Fail pa_make_flood_fill_undo()\n");
		if(fp)
		{
			al_fclose(fp);
		}
		return false;
	}
}

bool pa_make_flood_fill_redo(PA_CANVAS_EDITOR * cep, int layer, ALLEGRO_COLOR color, PA_QUEUE * qp, int shift_x, int shift_y, const char * fn)
{
	return pa_make_flood_fill_undo(cep, layer, color, qp, shift_x, shift_y, fn);
}

static bool read_color(ALLEGRO_FILE * fp, ALLEGRO_COLOR * color)
{
	unsigned char r, g, b, a;

	r = al_fgetc(fp);
	g = al_fgetc(fp);
	b = al_fgetc(fp);
	a = al_fgetc(fp);
	*color = al_map_rgba(r, g, b, a);

	return true;
}

bool pa_apply_flood_fill_undo(PA_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp)
{
	PA_QUEUE * qp;
	ALLEGRO_COLOR color;
	ALLEGRO_COLOR old_color;
	int size;
	int i, x, y;
	char undo_path[1024];
	int layer;
	int tool;
	int shift_x, shift_y;
	char * name;

	t3f_debug_message("Enter pa_apply_flood_fill_undo()\n");
	tool = al_fread16le(fp);
	layer = al_fread32le(fp);
	read_color(fp, &color);
	shift_x = al_fread32le(fp);
	shift_y = al_fread32le(fp);
	name = t3f_load_string_f(fp);
	size = al_fread32le(fp);
	if(size)
	{
		qp = pa_create_queue(name);
		if(!qp)
		{
			goto fail;
		}
		for(i = 0; i < size; i++)
		{
			x = al_fread32le(fp);
			y = al_fread32le(fp);
			pa_queue_push(qp, x, y);
		}
		old_color = pa_get_canvas_pixel(cep->canvas, layer, x, y, NULL, NULL);
		pa_make_flood_fill_redo(cep, layer, old_color, qp, shift_x, shift_y,  pa_get_undo_path("redo", cep->redo_count, undo_path, 1024));
		cep->redo_count++;
		pa_flood_fill_canvas_from_queue(cep->canvas, layer, color, qp);
		pa_destroy_queue(qp);
		if(shift_x || shift_y)
		{
			pa_shift_canvas_bitmap_array(cep->canvas, -shift_x, -shift_y);
			pa_shift_canvas_editor_variables(cep, -shift_x * cep->canvas->bitmap_size, -shift_y * cep->canvas->bitmap_size);
		}
	}
	if(name)
	{
		free(name);
	}
	pa_select_canvas_editor_tool(cep, PA_TOOL_FLOOD_FILL);
	t3f_debug_message("Exit pa_apply_flood_fill_undo()\n");
	return true;

	fail:
	{
		t3f_debug_message("Fail pa_apply_flood_fill_undo()\n");
		if(qp)
		{
			pa_destroy_queue(qp);
		}
		if(name)
		{
			free(name);
		}
		return false;
	}
}

static void ensure_bitmaps_exist(PA_CANVAS * cp, int layer, PA_QUEUE * qp)
{
	PA_QUEUE_NODE * current_node;

	current_node = qp->current;
	while(current_node)
	{
		pa_expand_canvas(cp, layer, current_node->x, current_node->y);
		current_node = current_node->previous;
	}
}

bool pa_apply_flood_fill_redo(PA_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp)
{
	PA_QUEUE * qp;
	ALLEGRO_COLOR color;
	ALLEGRO_COLOR old_color;
	int size;
	int i, x, y;
	char undo_path[1024];
	int layer;
	int tool;
	int shift_x, shift_y;
	int left, right, top, bottom;
	char * name;

	t3f_debug_message("Enter pa_apply_flood_fill_redo()\n");
	tool = al_fread16le(fp);
	layer = al_fread32le(fp);
	left = cep->canvas->layer[layer]->width;
	right = 0;
	top = cep->canvas->layer[layer]->height;
	bottom = 0;
	read_color(fp, &color);
	shift_x = al_fread32le(fp);
	shift_y = al_fread32le(fp);
	name = t3f_load_string_f(fp);
	size = al_fread32le(fp);
	if(size)
	{
		qp = pa_create_queue(name);
		if(!qp)
		{
			goto fail;
		}
		for(i = 0; i < size; i++)
		{
			x = al_fread32le(fp);
			y = al_fread32le(fp);
			pa_queue_push(qp, x, y);
			if(x < left)
			{
				left = x;
			}
			if(x > right)
			{
				right = x;
			}
			if(y < top)
			{
				top = y;
			}
			if(y > bottom)
			{
				bottom = y;
			}
		}
		left -= shift_x * cep->canvas->bitmap_size;
		top -= shift_y * cep->canvas->bitmap_size;
		right -= shift_x * cep->canvas->bitmap_size;
		bottom -= shift_y * cep->canvas->bitmap_size;
		pa_handle_canvas_expansion(cep->canvas, left, top, right, bottom, &shift_x, &shift_y);
		ensure_bitmaps_exist(cep->canvas, layer, qp);
		pa_shift_canvas_editor_variables(cep, shift_x * cep->canvas->bitmap_size, shift_y * cep->canvas->bitmap_size);
		old_color = pa_get_canvas_pixel(cep->canvas, layer, x, y, NULL, NULL);
		pa_make_flood_fill_undo(cep, layer, old_color, qp, shift_x, shift_y,  pa_get_undo_path("undo", cep->undo_count, undo_path, 1024));
		cep->undo_count++;
		pa_flood_fill_canvas_from_queue(cep->canvas, layer, color, qp);
		pa_destroy_queue(qp);
	}
	if(name)
	{
		free(name);
	}
	pa_select_canvas_editor_tool(cep, PA_TOOL_FLOOD_FILL);
	t3f_debug_message("Exit pa_apply_flood_fill_redo()\n");
	return true;

	fail:
	{
		t3f_debug_message("Fail pa_apply_flood_fill_redo()\n");
		if(qp)
		{
			pa_destroy_queue(qp);
		}
		if(name)
		{
			free(name);
		}
		return false;
	}
}
