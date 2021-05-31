#include "t3f/t3f.h"
#include "modules/canvas/canvas.h"
#include "modules/canvas/canvas_helpers.h"
#include "canvas_editor.h"
#include "undo.h"

bool quixel_make_tool_undo(QUIXEL_CANVAS_EDITOR * cep, const char * action, int layer, int x, int y, int width, int height, const char * fn)
{
	ALLEGRO_STATE old_state;
	ALLEGRO_BITMAP * bp = NULL;
	ALLEGRO_FILE * fp = NULL;
	const char * action_name;

	if(width <= 0 || height <= 0)
	{
		quixel_get_canvas_dimensions(cep->canvas, &x, &y, &width, &height, 0);
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
		quixel_render_canvas_to_bitmap(cep->canvas, layer, layer + 1, x, y, width, height, 0, bp);
	}
	fp = al_fopen(fn, "wb");
	if(!fp)
	{
		printf("fail: %s\n", fn);
		goto fail;
	}
	switch(cep->current_tool)
	{
		case QUIXEL_TOOL_PIXEL:
		{
			action_name = "Draw Pixel";
			break;
		}
		case QUIXEL_TOOL_LINE:
		{
			action_name = "Draw Line";
			break;
		}
		case QUIXEL_TOOL_RECTANGLE:
		{
			action_name = "Draw Rectangle";
			break;
		}
		case QUIXEL_TOOL_FILLED_RECTANGLE:
		{
			action_name = "Draw Filled Rectangle";
			break;
		}
		case QUIXEL_TOOL_OVAL:
		{
			action_name = "Draw Oval";
			break;
		}
		case QUIXEL_TOOL_FILLED_OVAL:
		{
			action_name = "Draw Filled Oval";
			break;
		}
		case QUIXEL_TOOL_FLOOD_FILL:
		{
			action_name = "Flood Fill";
			break;
		}
		default:
		{
			action_name = "Draw";
			break;
		}
	}
	quixel_write_undo_header(fp, QUIXEL_UNDO_TYPE_TOOL, action ? action : action_name);
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
	return true;

	fail:
	{
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

bool quixel_make_tool_redo(QUIXEL_CANVAS_EDITOR * cep, const char * action, int layer, int x, int y, int width, int height, const char * fn)
{
	return quixel_make_tool_undo(cep, action, layer, x, y, width, height, fn);
}

bool quixel_apply_tool_undo(QUIXEL_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp, const char * action, bool revert)
{
	int i, j, l;
	char undo_path[1024];
	ALLEGRO_BITMAP * bp = NULL;
	int layer, x, y;

	layer = al_fread32le(fp);
	l = al_fgetc(fp);
	if(l)
	{
		x = al_fread32le(fp);
		y = al_fread32le(fp);
		bp = al_load_bitmap_flags_f(fp, ".png", ALLEGRO_NO_PREMULTIPLIED_ALPHA);
		if(!bp)
		{
			goto fail;
		}
		if(!revert)
		{
			quixel_make_tool_redo(cep, action, layer, x, y, al_get_bitmap_width(bp), al_get_bitmap_height(bp), quixel_get_undo_path("redo", cep->redo_count, undo_path, 1024));
			cep->redo_count++;
		}
		quixel_import_bitmap_to_canvas(cep->canvas, bp, layer, x, y);
		al_destroy_bitmap(bp);
	}
	else
	{
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
	return true;

	fail:
	{
		if(bp)
		{
			al_destroy_bitmap(bp);
		}
		return false;
	}
}

bool quixel_apply_tool_redo(QUIXEL_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp, const char * action)
{
	int i, j, l;
	char undo_path[1024];
	ALLEGRO_BITMAP * bp = NULL;
	int layer, x, y;

	layer = al_fread32le(fp);
	l = al_fgetc(fp);
	if(l)
	{
		x = al_fread32le(fp);
		y = al_fread32le(fp);
		bp = al_load_bitmap_flags_f(fp, ".png", ALLEGRO_NO_PREMULTIPLIED_ALPHA);
		if(!bp)
		{
			goto fail;
		}
		quixel_make_tool_undo(cep, action, layer, x, y, al_get_bitmap_width(bp), al_get_bitmap_height(bp), quixel_get_undo_path("undo", cep->undo_count, undo_path, 1024));
		cep->undo_count++;
		quixel_import_bitmap_to_canvas(cep->canvas, bp, layer, x, y);
		al_destroy_bitmap(bp);
	}
	else
	{
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
	return true;

	fail:
	{
		if(bp)
		{
			al_destroy_bitmap(bp);
		}
		return false;
	}
}
