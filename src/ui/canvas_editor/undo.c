#include "t3f/t3f.h"
#include "modules/canvas/canvas.h"
#include "modules/canvas/canvas_helpers.h"
#include "canvas_editor.h"
#include "undo.h"

static bool write_undo_header(ALLEGRO_FILE * fp, int type, const char * name)
{
	al_fputc(fp, type);
	al_fwrite16le(fp, strlen(name));
	al_fwrite(fp, name, strlen(name));

	return true;
}

bool quixel_make_unfloat_selection_undo(QUIXEL_CANVAS_EDITOR * cep, const char * fn)
{
	ALLEGRO_STATE old_state;
	ALLEGRO_BITMAP * bp = NULL;
	ALLEGRO_BITMAP * float_bp = NULL;
	ALLEGRO_FILE * fp = NULL;

	al_store_state(&old_state, ALLEGRO_STATE_NEW_BITMAP_PARAMETERS);
	al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
	bp = al_create_bitmap(cep->selection.box.width, cep->selection.box.height);
	al_restore_state(&old_state);
	if(!bp)
	{
		goto fail;
	}
	float_bp = al_create_sub_bitmap(cep->scratch_bitmap, 0, 0, cep->selection.box.width, cep->selection.box.height);
	if(!float_bp)
	{
		goto fail;
	}
	quixel_render_canvas_to_bitmap(cep->canvas, cep->current_layer, cep->current_layer + 1, cep->selection.box.start_x, cep->selection.box.start_y, cep->selection.box.width, cep->selection.box.height, 0, bp);
	fp = al_fopen(fn, "wb");
	if(!fp)
	{
		printf("fail: %s\n", fn);
		goto fail;
	}
	write_undo_header(fp, QUIXEL_UNDO_TYPE_UNFLOAT_SELECTION, "Unfloat Selection");
	al_fwrite32le(fp, cep->current_layer);
	al_fwrite32le(fp, cep->selection.box.start_x);
	al_fwrite32le(fp, cep->selection.box.start_y);
	al_fwrite32le(fp, cep->selection.box.width);
	al_fwrite32le(fp, cep->selection.box.height);
	if(!al_save_bitmap_f(fp, ".png", bp))
	{
		printf("fail: %s\n", fn);
		goto fail;
	}

	if(!al_save_bitmap_f(fp, ".png", float_bp))
	{
		printf("fail: %s\n", fn);
		goto fail;
	}
	al_destroy_bitmap(bp);
	al_destroy_bitmap(float_bp);
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
		if(float_bp)
		{
			al_destroy_bitmap(bp);
		}
		return false;
	}
}

bool quixel_make_unfloat_selection_redo(QUIXEL_CANVAS_EDITOR * cep, const char * fn)
{
	return quixel_make_unfloat_selection_undo(cep, fn);
}

bool quixel_make_float_selection_undo(QUIXEL_CANVAS_EDITOR * cep, const char * fn)
{
	ALLEGRO_STATE old_state;
	ALLEGRO_BITMAP * bp = NULL;
	ALLEGRO_FILE * fp = NULL;

	al_store_state(&old_state, ALLEGRO_STATE_NEW_BITMAP_PARAMETERS);
	al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
	bp = al_create_bitmap(cep->selection.box.width, cep->selection.box.height);
	al_restore_state(&old_state);
	if(!bp)
	{
		goto fail;
	}
	quixel_render_canvas_to_bitmap(cep->canvas, cep->current_layer, cep->current_layer + 1, cep->selection.box.start_x, cep->selection.box.start_y, cep->selection.box.width, cep->selection.box.height, 0, bp);
	fp = al_fopen(fn, "wb");
	if(!fp)
	{
		printf("fail: %s\n", fn);
		goto fail;
	}
	write_undo_header(fp, QUIXEL_UNDO_TYPE_FLOAT_SELECTION, "Float Selection");
	al_fwrite32le(fp, cep->current_layer);
	al_fwrite32le(fp, cep->selection.box.start_x);
	al_fwrite32le(fp, cep->selection.box.start_y);
	al_fwrite32le(fp, cep->selection.box.width);
	al_fwrite32le(fp, cep->selection.box.height);
	if(!al_save_bitmap_f(fp, ".png", bp))
	{
		printf("fail: %s\n", fn);
		goto fail;
	}
	al_destroy_bitmap(bp);
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

bool quixel_make_float_selection_redo(QUIXEL_CANVAS_EDITOR * cep, const char * fn)
{
	return quixel_make_float_selection_undo(cep, fn);
}

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
	write_undo_header(fp, QUIXEL_UNDO_TYPE_TOOL, action ? action : action_name);
	if(width < 0 || height < 0)
	{
		al_fputc(fp, 0);
	}
	else
	{
		al_fputc(fp, 1);
		al_fwrite32le(fp, layer);
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

bool quixel_make_frame_undo(QUIXEL_CANVAS_EDITOR * cep, const char * action, const char * fn)
{
	ALLEGRO_FILE * fp = NULL;
	const char * action_name = "Add Frame";
	int i, l;

	fp = al_fopen(fn, "wb");
	if(!fp)
	{
		printf("fail: %s\n", fn);
		goto fail;
	}
	write_undo_header(fp, QUIXEL_UNDO_TYPE_FRAME, action ? action : action_name);
	al_fwrite32le(fp, cep->canvas->frame_max);
	for(i = 0; i < cep->canvas->frame_max; i++)
	{
		l = strlen(cep->canvas->frame[i]->name);
		al_fwrite16le(fp, l);
		al_fwrite(fp, cep->canvas->frame[i]->name, l);
		al_fwrite32le(fp, cep->canvas->frame[i]->box.start_x);
		al_fwrite32le(fp, cep->canvas->frame[i]->box.start_y);
		al_fwrite32le(fp, cep->canvas->frame[i]->box.width);
		al_fwrite32le(fp, cep->canvas->frame[i]->box.height);
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

bool quixel_make_frame_redo(QUIXEL_CANVAS_EDITOR * cep, const char * action, const char * fn)
{
	return quixel_make_frame_undo(cep, action, fn);
}

const char * quixel_get_undo_name(const char * fn, char * out, int out_size)
{
	ALLEGRO_FILE * fp = NULL;
	int l;

	fp = al_fopen(fn, "rb");
	if(!fp)
	{
		goto fail;
	}
	al_fgetc(fp);
	l = al_fread16le(fp);
	if(l >= out_size || l < 1)
	{
		goto fail;
	}
	al_fread(fp, out, l);
	out[l] = 0;
	al_fclose(fp);

	return out;

	fail:
	{
		if(fp)
		{
			al_fclose(fp);
		}
		return NULL;
	}
}

const char * quixel_get_undo_path(const char * base, int count, char * out, int out_size)
{
	char fn[256];

	sprintf(fn, "%s.%04d", base, count);
	return t3f_get_filename(t3f_data_path, fn, out, out_size);
}

void quixel_update_undo_name(QUIXEL_CANVAS_EDITOR * cep)
{
	char undo_path[1024];
	char uname[256];

	quixel_get_undo_path("undo", cep->undo_count - 1, undo_path, 1024);
	if(quixel_get_undo_name(undo_path, uname, 256))
	{
		sprintf(cep->undo_name, "Undo %s", uname);
	}
	else
	{
		sprintf(cep->undo_name, "Undo");
	}
}

void quixel_update_redo_name(QUIXEL_CANVAS_EDITOR * cep)
{
	char redo_path[1024];
	char rname[256];

	quixel_get_undo_path("redo", cep->redo_count - 1, redo_path, 1024);
	if(quixel_get_undo_name(redo_path, rname, 256))
	{
		sprintf(cep->redo_name, "Redo %s", rname);
	}
	else
	{
		sprintf(cep->redo_name, "Redo");
	}
}

static void copy_bitmap_to_target(ALLEGRO_BITMAP * bp, ALLEGRO_BITMAP * target_bp)
{
	ALLEGRO_STATE old_state;
	ALLEGRO_TRANSFORM identity;

	al_store_state(&old_state, ALLEGRO_STATE_TARGET_BITMAP | ALLEGRO_STATE_TRANSFORM | ALLEGRO_STATE_BLENDER);
	al_identity_transform(&identity);
	al_set_target_bitmap(target_bp);
	al_use_transform(&identity);
	al_clear_to_color(al_map_rgba_f(0.0, 0.0, 0.0, 0.0));
	al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);
	al_draw_bitmap(bp, 0, 0, 0);
	al_restore_state(&old_state);
}

void quixel_finalize_undo(QUIXEL_CANVAS_EDITOR * cep)
{
	cep->undo_count++;
	cep->redo_count = 0;
	quixel_update_undo_name(cep);
	quixel_update_redo_name(cep);
	t3f_refresh_menus();
}

bool quixel_apply_undo(QUIXEL_CANVAS_EDITOR * cep, const char * fn, bool redo, bool revert)
{
	ALLEGRO_FILE * fp = NULL;
	int type;
	int i, j, l;
	char buf[1024];
	char undo_path[1024];
	ALLEGRO_BITMAP * bp;
	int layer, x, y, width, height;
	int frame_max;
	char frame_name[256] = {0};

	fp = al_fopen(fn, "rb");
	if(!fp)
	{
		goto fail;
	}
	type = al_fgetc(fp);
	l = al_fread16le(fp);
	al_fread(fp, buf, l);
	buf[l] = 0;
	switch(type)
	{
		case QUIXEL_UNDO_TYPE_TOOL:
		{
			l = al_fgetc(fp);
			if(l)
			{
				layer = al_fread32le(fp);
				x = al_fread32le(fp);
				y = al_fread32le(fp);
				bp = al_load_bitmap_flags_f(fp, ".png", ALLEGRO_NO_PREMULTIPLIED_ALPHA);
				if(!bp)
				{
					goto fail;
				}
				if(!revert)
				{
					if(!redo)
					{
						quixel_make_tool_redo(cep, buf, layer, x, y, al_get_bitmap_width(bp), al_get_bitmap_height(bp), quixel_get_undo_path("redo", cep->redo_count, undo_path, 1024));
						cep->redo_count++;
					}
					else
					{
						quixel_make_tool_undo(cep, buf, layer, x, y, al_get_bitmap_width(bp), al_get_bitmap_height(bp), quixel_get_undo_path("undo", cep->undo_count, undo_path, 1024));
						cep->undo_count++;
					}
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
			break;
		}
		case QUIXEL_UNDO_TYPE_UNFLOAT_SELECTION:
		{
			if(!redo)
			{
//				quixel_make_unfloat_selection_redo(cep, quixel_get_undo_path("redo", cep->redo_count, undo_path, 1024));
				cep->redo_count++;
			}
			else
			{
				quixel_make_unfloat_selection_undo(cep, quixel_get_undo_path("undo", cep->undo_count, undo_path, 1024));
				cep->undo_count++;
			}
			layer = al_fread32le(fp);
			cep->selection.box.start_x = al_fread32le(fp);
			cep->selection.box.start_y = al_fread32le(fp);
			cep->selection.box.width = al_fread32le(fp);
			cep->selection.box.height = al_fread32le(fp);
			bp = al_load_bitmap_flags_f(fp, ".png", ALLEGRO_NO_PREMULTIPLIED_ALPHA);
			if(!bp)
			{
				goto fail;
			}
			quixel_import_bitmap_to_canvas(cep->canvas, bp, layer, cep->selection.box.start_x, cep->selection.box.start_y);
			al_destroy_bitmap(bp);
			bp = al_load_bitmap_flags_f(fp, ".png", ALLEGRO_NO_PREMULTIPLIED_ALPHA);
			if(!bp)
			{
				goto fail;
			}
			copy_bitmap_to_target(bp, cep->scratch_bitmap);
			al_destroy_bitmap(bp);
			cep->selection.floating = true;
			break;
		}
		case QUIXEL_UNDO_TYPE_FLOAT_SELECTION:
		{
			if(!redo)
			{
//				quixel_make_unfloat_selection_redo(cep, quixel_get_undo_path("redo", cep->redo_count, undo_path, 1024));
				cep->redo_count++;
			}
			else
			{
				quixel_make_unfloat_selection_undo(cep, quixel_get_undo_path("undo", cep->undo_count, undo_path, 1024));
				cep->undo_count++;
			}
			layer = al_fread32le(fp);
			cep->selection.box.start_x = al_fread32le(fp);
			cep->selection.box.start_y = al_fread32le(fp);
			cep->selection.box.width = al_fread32le(fp);
			cep->selection.box.height = al_fread32le(fp);
			quixel_initialize_box(&cep->selection.box, cep->selection.box.start_x, cep->selection.box.start_y, cep->selection.box.width, cep->selection.box.height, cep->selection.box.bitmap);
			quixel_update_box_handles(&cep->selection.box, cep->view_x, cep->view_y, cep->view_zoom);
			bp = al_load_bitmap_flags_f(fp, ".png", ALLEGRO_NO_PREMULTIPLIED_ALPHA);
			if(!bp)
			{
				goto fail;
			}
			quixel_import_bitmap_to_canvas(cep->canvas, bp, layer, cep->selection.box.start_x, cep->selection.box.start_y);
			al_destroy_bitmap(bp);
			cep->selection.floating = false;
			break;
		}
		case QUIXEL_UNDO_TYPE_FRAME:
		{
			if(!redo)
			{
				quixel_make_frame_redo(cep, buf, quixel_get_undo_path("redo", cep->redo_count, undo_path, 1024));
				cep->redo_count++;
			}
			else
			{
				quixel_make_frame_undo(cep, buf, quixel_get_undo_path("undo", cep->undo_count, undo_path, 1024));
				cep->undo_count++;
			}
			frame_max = cep->canvas->frame_max;
			for(i = 0; i < frame_max; i++)
			{
				quixel_remove_canvas_frame(cep->canvas, 0);
			}
			frame_max = al_fread32le(fp);
			for(i = 0; i < frame_max; i++)
			{
				l = al_fread16le(fp);
				al_fread(fp, frame_name, l);
				if(l >= 256)
				{
					goto fail;
				}
				frame_name[l] = 0;
				x = al_fread32le(fp);
				y = al_fread32le(fp);
				width = al_fread32le(fp);
				height = al_fread32le(fp);
				quixel_add_canvas_frame(cep->canvas, frame_name, x, y, width, height);
			}
			break;
		}
	}
	al_fclose(fp);
	return false;

	fail:
	{
		if(fp)
		{
			al_fclose(fp);
		}
		return false;
	}
}

bool quixel_apply_redo(QUIXEL_CANVAS_EDITOR * cep, const char * fn)
{
	return quixel_apply_undo(cep, fn, true, false);
}

void quixel_undo_clean_up(QUIXEL_CANVAS_EDITOR * cep)
{
	char undo_path[1024];
	int i;

	for(i = 0; i < cep->undo_count; i++)
	{
		al_remove_filename(quixel_get_undo_path("undo", i, undo_path, 1024));
	}
	for(i = 0; i < cep->redo_count; i++)
	{
		al_remove_filename(quixel_get_undo_path("redo", i, undo_path, 1024));
	}
}
