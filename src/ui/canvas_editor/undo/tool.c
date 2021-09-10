#include "t3f/t3f.h"
#include "modules/canvas/canvas.h"
#include "modules/canvas/canvas_helpers.h"
#include "ui/canvas_editor/canvas_editor.h"
#include "undo.h"

bool pa_make_tool_undo(PA_CANVAS_EDITOR * cep, const char * action, int layer, int x, int y, int width, int height, const char * fn)
{
	ALLEGRO_STATE old_state;
	ALLEGRO_BITMAP * bp = NULL;
	ALLEGRO_FILE * fp = NULL;
	const char * action_name;

	t3f_debug_message("Enter pa_make_tool_undo()\n");
	if(width <= 0 || height <= 0)
	{
		pa_get_canvas_dimensions(cep->canvas, &x, &y, &width, &height, 0, false);
	}
	if(width > 0 && height > 0)
	{
		al_store_state(&old_state, ALLEGRO_STATE_NEW_BITMAP_PARAMETERS);
		al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
		bp = al_create_bitmap(width, height);
		al_restore_state(&old_state);
		if(!bp)
		{
			goto fail;
		}
	}
	if(width > 0 && height > 0)
	{
		pa_render_canvas_to_bitmap(cep->canvas, layer, layer + 1, x, y, width, height, 0, bp, NULL);
	}
	fp = al_fopen(fn, "wb");
	if(!fp)
	{
		printf("fail: %s\n", fn);
		goto fail;
	}
	switch(cep->current_tool)
	{
		case PA_TOOL_PIXEL:
		{
			action_name = "Draw Pixel";
			break;
		}
		case PA_TOOL_LINE:
		{
			action_name = "Draw Line";
			break;
		}
		case PA_TOOL_RECTANGLE:
		{
			action_name = "Draw Rectangle";
			break;
		}
		case PA_TOOL_FILLED_RECTANGLE:
		{
			action_name = "Draw Filled Rectangle";
			break;
		}
		case PA_TOOL_OVAL:
		{
			action_name = "Draw Oval";
			break;
		}
		case PA_TOOL_FILLED_OVAL:
		{
			action_name = "Draw Filled Oval";
			break;
		}
		case PA_TOOL_FLOOD_FILL:
		{
			action_name = "Flood Fill";
			break;
		}
		case PA_TOOL_ERASER:
		{
			action_name = "Erase";
			break;
		}
		default:
		{
			action_name = "Draw";
			break;
		}
	}
	pa_write_undo_header(fp, cep, PA_UNDO_TYPE_TOOL, action ? action : action_name);
	al_fwrite16le(fp, cep->current_tool);
	al_fwrite32le(fp, layer);
	if(width < 0 || height < 0)
	{
		al_fputc(fp, 0);
	}
	else
	{
		al_fputc(fp, 1);
		al_fwrite32le(fp, x);
		al_fwrite32le(fp, y);
		if(!al_save_bitmap_f(fp, ".png", bp))
		{
			printf("fail: %s\n", fn);
			goto fail;
		}
	}
	if(bp)
	{
		al_destroy_bitmap(bp);
	}
	al_fclose(fp);
	t3f_debug_message("Exit pa_make_tool_undo()\n");
	return true;

	fail:
	{
		t3f_debug_message("Fail pa_make_tool_undo()\n");
		if(fp)
		{
			al_fclose(fp);
		}
		if(bp)
		{
			al_destroy_bitmap(bp);
		}
		return false;
	}
}

bool pa_make_tool_redo(PA_CANVAS_EDITOR * cep, const char * action, int tool, int layer, int x, int y, int width, int height, const char * fn)
{
	ALLEGRO_STATE old_state;
	ALLEGRO_BITMAP * bp = NULL;
	ALLEGRO_FILE * fp = NULL;
	const char * action_name;
	int shift_x, shift_y;

	t3f_debug_message("Enter pa_make_tool_redo()\n");
	pa_get_canvas_shift(cep->canvas, x, y, &shift_x, &shift_y);
	al_store_state(&old_state, ALLEGRO_STATE_NEW_BITMAP_PARAMETERS);
	al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
	bp = al_create_bitmap(width, height);
	al_restore_state(&old_state);
	if(!bp)
	{
		printf("bitmap fail\n");
		goto fail;
	}
	pa_render_canvas_to_bitmap(cep->canvas, layer, layer + 1, x + shift_x * cep->canvas->bitmap_size, y + shift_y * cep->canvas->bitmap_size, width, height, 0, bp, NULL);
	fp = al_fopen(fn, "wb");
	if(!fp)
	{
		printf("fail: %s\n", fn);
		goto fail;
	}
	switch(cep->current_tool)
	{
		case PA_TOOL_PIXEL:
		{
			action_name = "Draw Pixel";
			break;
		}
		case PA_TOOL_LINE:
		{
			action_name = "Draw Line";
			break;
		}
		case PA_TOOL_RECTANGLE:
		{
			action_name = "Draw Rectangle";
			break;
		}
		case PA_TOOL_FILLED_RECTANGLE:
		{
			action_name = "Draw Filled Rectangle";
			break;
		}
		case PA_TOOL_OVAL:
		{
			action_name = "Draw Oval";
			break;
		}
		case PA_TOOL_FILLED_OVAL:
		{
			action_name = "Draw Filled Oval";
			break;
		}
		case PA_TOOL_FLOOD_FILL:
		{
			action_name = "Flood Fill";
			break;
		}
		case PA_TOOL_ERASER:
		{
			action_name = "Erase";
			break;
		}
		default:
		{
			action_name = "Draw";
			break;
		}
	}
	pa_write_undo_header(fp, cep, PA_UNDO_TYPE_TOOL, action ? action : action_name);
	al_fwrite16le(fp, tool);
	al_fwrite32le(fp, layer);
	al_fwrite32le(fp, x);
	al_fwrite32le(fp, y);
	if(!al_save_bitmap_f(fp, ".png", bp))
	{
		printf("fail: %s\n", fn);
		goto fail;
	}
	al_fclose(fp);
	t3f_debug_message("Exit pa_make_tool_redo()\n");
	return true;

	fail:
	{
		t3f_debug_message("Fail pa_make_tool_redo()\n");
		if(fp)
		{
			al_fclose(fp);
		}
		if(bp)
		{
			al_destroy_bitmap(bp);
		}
		return false;
	}
}

bool pa_apply_tool_undo(PA_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp, const char * action, bool revert)
{
	ALLEGRO_STATE old_state;
	int i, j, l;
	char undo_path[1024];
	ALLEGRO_BITMAP * bp = NULL;
	int layer, x, y, width, height;
	int shift_x = 0, shift_y = 0;
	int tool;

	t3f_debug_message("Enter pa_apply_tool_undo()\n");
	tool = al_fread16le(fp);
	layer = al_fread32le(fp);
	l = al_fgetc(fp);
	if(l)
	{
		x = al_fread32le(fp);
		y = al_fread32le(fp);
		pa_get_canvas_shift(cep->canvas, x, y, &shift_x, &shift_y);
		bp = al_load_bitmap_flags_f(fp, ".png", ALLEGRO_NO_PREMULTIPLIED_ALPHA);
		if(!bp)
		{
			goto fail;
		}
		if(!revert)
		{
			pa_make_tool_redo(cep, action, tool, layer, x, y, al_get_bitmap_width(bp), al_get_bitmap_height(bp), pa_get_undo_path("redo", cep->redo_count, undo_path, 1024));
			cep->redo_count++;
		}
		pa_import_bitmap_to_canvas(cep->canvas, bp, layer, x + shift_x * cep->canvas->bitmap_size, y + shift_y * cep->canvas->bitmap_size);
		al_destroy_bitmap(bp);
	}
	else
	{
		pa_get_canvas_dimensions(cep->canvas, &x, &y, &width, &height, 0, false);
		al_store_state(&old_state, ALLEGRO_STATE_NEW_BITMAP_PARAMETERS);
		al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
		bp = al_create_bitmap(width, height);
		al_restore_state(&old_state);
		if(!bp)
		{
			goto fail;
		}
		pa_render_canvas_to_bitmap(cep->canvas, layer, layer + 1, x, y, width, height, 0, bp, NULL);
		pa_make_tool_redo(cep, action, tool, layer, x, y, width, height, pa_get_undo_path("redo", cep->redo_count, undo_path, 1024));
		for(i = 0; i < cep->canvas->layer_height; i++)
		{
			for(j = 0; j < cep->canvas->layer_width; j++)
			{
				if(cep->canvas->layer[layer]->bitmap[i][j])
				{
					al_destroy_bitmap(cep->canvas->layer[layer]->bitmap[i][j]);
					cep->canvas->layer[layer]->bitmap[i][j] = NULL;
				}
			}
		}
	}
	if(shift_x || shift_y)
	{
		pa_shift_canvas_bitmap_array(cep->canvas, -shift_x, -shift_y);
		pa_shift_canvas_editor_variables(cep, -shift_x * cep->canvas->bitmap_size, -shift_y * cep->canvas->bitmap_size);
	}
	pa_select_canvas_editor_tool(cep, tool);
	t3f_debug_message("Exit pa_apply_tool_undo()\n");
	return true;

	fail:
	{
		t3f_debug_message("Fail pa_apply_tool_undo()\n");
		if(bp)
		{
			al_destroy_bitmap(bp);
		}
		return false;
	}
}

bool pa_apply_tool_redo(PA_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp, const char * action)
{
	char undo_path[1024];
	ALLEGRO_BITMAP * bp = NULL;
	int layer, x, y;
	int shift_x, shift_y;
	int tool;

	t3f_debug_message("Enter pa_apply_tool_redo()\n");
	tool = al_fread16le(fp);
	layer = al_fread32le(fp);
	x = al_fread32le(fp);
	y = al_fread32le(fp);
	pa_get_canvas_shift(cep->canvas, x, y, &shift_x, &shift_y);
	bp = al_load_bitmap_flags_f(fp, ".png", ALLEGRO_NO_PREMULTIPLIED_ALPHA);
	if(!bp)
	{
		goto fail;
	}
	pa_make_tool_undo(cep, action, layer, x, y, al_get_bitmap_width(bp), al_get_bitmap_height(bp), pa_get_undo_path("undo", cep->undo_count, undo_path, 1024));
	cep->undo_count++;
	pa_import_bitmap_to_canvas(cep->canvas, bp, layer, x, y);
	pa_shift_canvas_editor_variables(cep, shift_x * cep->canvas->bitmap_size, shift_y * cep->canvas->bitmap_size);
	al_destroy_bitmap(bp);
	pa_select_canvas_editor_tool(cep, tool);
	t3f_debug_message("Exit pa_apply_tool_redo()\n");

	return true;

	fail:
	{
		t3f_debug_message("Fail pa_apply_tool_redo()\n");
		if(bp)
		{
			al_destroy_bitmap(bp);
		}
		return false;
	}
}
