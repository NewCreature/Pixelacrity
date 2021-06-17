#include "t3f/t3f.h"
#include "modules/canvas/canvas.h"
#include "modules/canvas/canvas_helpers.h"
#include "modules/canvas/flood_fill.h"
#include "modules/queue.h"
#include "canvas_editor.h"
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

bool quixel_make_flood_fill_undo(QUIXEL_CANVAS_EDITOR * cep, int layer, ALLEGRO_COLOR color, QUIXEL_QUEUE * qp, const char * fn)
{
	ALLEGRO_FILE * fp = NULL;
	QUIXEL_QUEUE_NODE * current_node;

	fp = al_fopen(fn, "wb");
	if(!fp)
	{
		printf("fail: %s\n", fn);
		goto fail;
	}
	quixel_write_undo_header(fp, QUIXEL_UNDO_TYPE_FLOOD_FILL, "Flood Fill");
	al_fwrite16le(fp, cep->current_tool);
	al_fwrite32le(fp, layer);
	write_color(fp, color);
	al_fwrite32le(fp, quixel_get_queue_size(qp));
	current_node = qp->current;
	while(current_node)
	{
		al_fwrite32le(fp, current_node->x);
		al_fwrite32le(fp, current_node->y);
		current_node = current_node->previous;
	}
	al_fclose(fp);
	return true;

	fail:
	{
		if(fp)
		{
			al_fclose(fp);
		}
		return false;
	}
}

bool quixel_make_flood_fill_redo(QUIXEL_CANVAS_EDITOR * cep, int layer, ALLEGRO_COLOR color, QUIXEL_QUEUE * qp, const char * fn)
{
	return quixel_make_flood_fill_undo(cep, layer, color, qp, fn);
}

static bool read_color(ALLEGRO_FILE * fp, ALLEGRO_COLOR * color)
{
	unsigned char r, g, b, a;

	r = al_fgetc(fp);
	g = al_fgetc(fp);
	b = al_fgetc(fp);
	a = al_fgetc(fp);
	*color = al_map_rgba(r, b, b, a);

	return true;
}

bool quixel_apply_flood_fill_undo(QUIXEL_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp)
{
	QUIXEL_QUEUE * qp;
	ALLEGRO_COLOR color;
	ALLEGRO_COLOR old_color;
	int size;
	int i, x, y;
	char undo_path[1024];
	int layer;
	int tool;

	tool = al_fread16le(fp);
	layer = al_fread32le(fp);
	read_color(fp, &color);
	size = al_fread32le(fp);
	if(size)
	{
		qp = quixel_create_queue();
		if(!qp)
		{
			goto fail;
		}
		for(i = 0; i < size; i++)
		{
			x = al_fread32le(fp);
			y = al_fread32le(fp);
			quixel_queue_push(qp, x, y);
		}
		old_color = quixel_get_canvas_pixel(cep->canvas, layer, x, y);
		quixel_make_flood_fill_redo(cep, layer, old_color, qp,  quixel_get_undo_path("redo", cep->redo_count, undo_path, 1024));
		cep->redo_count++;
		quixel_flood_fill_canvas_from_queue(cep->canvas, layer, color, qp);
		quixel_destroy_queue(qp);
	}
	quixel_select_canvas_editor_tool(cep, QUIXEL_TOOL_FLOOD_FILL);
	return true;

	fail:
	{
		if(qp)
		{
			quixel_destroy_queue(qp);
		}
		return false;
	}
}

bool quixel_apply_flood_fill_redo(QUIXEL_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp)
{
	QUIXEL_QUEUE * qp;
	ALLEGRO_COLOR color;
	ALLEGRO_COLOR old_color;
	int size;
	int i, x, y;
	char undo_path[1024];
	int layer;
	int tool;

	tool = al_fread16le(fp);
	layer = al_fread32le(fp);
	read_color(fp, &color);
	size = al_fread32le(fp);
	if(size)
	{
		qp = quixel_create_queue();
		if(!qp)
		{
			goto fail;
		}
		for(i = 0; i < size; i++)
		{
			x = al_fread32le(fp);
			y = al_fread32le(fp);
			quixel_queue_push(qp, x, y);
		}
		old_color = quixel_get_canvas_pixel(cep->canvas, layer, x, y);
		quixel_make_flood_fill_undo(cep, layer, old_color, qp,  quixel_get_undo_path("undo", cep->undo_count, undo_path, 1024));
		cep->undo_count++;
		quixel_flood_fill_canvas_from_queue(cep->canvas, layer, color, qp);
		quixel_destroy_queue(qp);
	}
	quixel_select_canvas_editor_tool(cep, QUIXEL_TOOL_FLOOD_FILL);
	return true;

	fail:
	{
		if(qp)
		{
			quixel_destroy_queue(qp);
		}
		return false;
	}
}
