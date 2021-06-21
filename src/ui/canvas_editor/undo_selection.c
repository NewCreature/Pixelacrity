#include "t3f/t3f.h"
#include "modules/canvas/canvas.h"
#include "modules/canvas/canvas_helpers.h"
#include "modules/primitives.h"
#include "canvas_editor.h"
#include "undo.h"

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

bool quixel_make_unfloat_selection_undo(QUIXEL_CANVAS_EDITOR * cep, const char * fn)
{
	ALLEGRO_STATE old_state;
	ALLEGRO_BITMAP * bp = NULL;
	ALLEGRO_FILE * fp = NULL;

	t3f_debug_message("Enter quixel_make_unfloat_selection_undo()\n");
	al_store_state(&old_state, ALLEGRO_STATE_NEW_BITMAP_PARAMETERS);
	al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
	bp = al_create_bitmap(cep->selection.box.width, cep->selection.box.height);
	al_restore_state(&old_state);
	if(!bp)
	{
		goto fail;
	}
	quixel_render_canvas_to_bitmap(cep->canvas, cep->current_layer, cep->current_layer + 1, cep->selection.box.start_x, cep->selection.box.start_y, cep->selection.box.width, cep->selection.box.height, 0, bp);
	quixel_get_canvas_shift(cep->canvas, cep->tool_left, cep->tool_top, &cep->shift_x, &cep->shift_y);
	fp = al_fopen(fn, "wb");
	if(!fp)
	{
		printf("fail: %s\n", fn);
		goto fail;
	}
	quixel_write_undo_header(fp, QUIXEL_UNDO_TYPE_UNFLOAT_SELECTION, "Unfloat Selection");
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
	if(!al_save_bitmap_f(fp, ".png", cep->selection.bitmap))
	{
		printf("float fail\n");
		goto fail;
	}
	al_destroy_bitmap(bp);
	al_fclose(fp);
	t3f_debug_message("Exit quixel_make_unfloat_selection_undo()\n");
	return true;

	fail:
	{
		t3f_debug_message("Fail quixel_make_unfloat_selection_undo()\n");
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

bool quixel_make_unfloat_selection_redo(QUIXEL_CANVAS_EDITOR * cep, const char * fn)
{
	ALLEGRO_STATE old_state;
	ALLEGRO_BITMAP * bp = NULL;
	ALLEGRO_FILE * fp = NULL;

	t3f_debug_message("Enter quixel_make_unfloat_selection_redo()\n");
	bp = al_create_bitmap(cep->selection.box.width, cep->selection.box.height);
	if(!bp)
	{
		goto fail;
	}
	fp = al_fopen(fn, "wb");
	if(!fp)
	{
		printf("fail: %s\n", fn);
		goto fail;
	}
	quixel_write_undo_header(fp, QUIXEL_REDO_TYPE_UNFLOAT_SELECTION, "Unfloat Selection");
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
	t3f_debug_message("Exit quixel_make_unfloat_selection_redo()\n");
	return true;

	fail:
	{
		t3f_debug_message("Fail quixel_make_unfloat_selection_redo()\n");
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

bool quixel_make_float_selection_undo(QUIXEL_CANVAS_EDITOR * cep, QUIXEL_BOX * box, const char * fn)
{
	ALLEGRO_STATE old_state;
	ALLEGRO_BITMAP * bp = NULL;
	ALLEGRO_FILE * fp = NULL;

	t3f_debug_message("Enter quixel_make_float_selection_undo()\n");
	al_store_state(&old_state, ALLEGRO_STATE_NEW_BITMAP_PARAMETERS);
	al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
	bp = al_create_bitmap(box->width, box->height);
	al_restore_state(&old_state);
	if(!bp)
	{
		goto fail;
	}
	quixel_render_canvas_to_bitmap(cep->canvas, cep->current_layer, cep->current_layer + 1, box->start_x, box->start_y, box->width, box->height, 0, bp);
	fp = al_fopen(fn, "wb");
	if(!fp)
	{
		printf("fail: %s\n", fn);
		goto fail;
	}
	quixel_write_undo_header(fp, QUIXEL_UNDO_TYPE_FLOAT_SELECTION, "Float Selection");
	al_fwrite32le(fp, cep->current_layer);
	al_fwrite32le(fp, box->start_x);
	al_fwrite32le(fp, box->start_y);
	if(!al_save_bitmap_f(fp, ".png", bp))
	{
		printf("fail: %s\n", fn);
		goto fail;
	}
	al_destroy_bitmap(bp);
	al_fclose(fp);
	t3f_debug_message("Exit quixel_make_float_selection_undo()\n");
	return true;

	fail:
	{
		t3f_debug_message("Fail quixel_make_float_selection_undo()\n");
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

bool quixel_make_float_selection_redo(QUIXEL_CANVAS_EDITOR * cep, int new_x, int new_y, ALLEGRO_BITMAP * bp, const char * fn)
{
	ALLEGRO_STATE old_state;
	ALLEGRO_FILE * fp = NULL;

	t3f_debug_message("Enter quixel_make_float_selection_redo()\n");
	al_store_state(&old_state, ALLEGRO_STATE_NEW_BITMAP_PARAMETERS);
	al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
	fp = al_fopen(fn, "wb");
	if(!fp)
	{
		printf("fail: %s\n", fn);
		goto fail;
	}
	quixel_write_undo_header(fp, QUIXEL_REDO_TYPE_FLOAT_SELECTION, "Float Selection");
	al_fwrite32le(fp, cep->current_layer);
	al_fwrite32le(fp, new_x);
	al_fwrite32le(fp, new_y);
	al_fwrite32le(fp, cep->selection.box.start_x);
	al_fwrite32le(fp, cep->selection.box.start_y);
	if(!al_save_bitmap_f(fp, ".png", bp))
	{
		printf("fail: %s\n", fn);
		goto fail;
	}
	al_fclose(fp);

	t3f_debug_message("Exit quixel_make_float_selection_redo()\n");
	return true;

	fail:
	{
		t3f_debug_message("Fail quixel_make_float_selection_redo()\n");
		if(fp)
		{
			al_fclose(fp);
		}
		return false;
	}
}

bool quixel_apply_unfloat_selection_undo(QUIXEL_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp, const char * action, bool revert)
{
	char undo_path[1024];
	ALLEGRO_BITMAP * bp = NULL;
	int layer;
	int shift_x = 0;
	int shift_y = 0;

	t3f_debug_message("Enter quixel_apply_unfloat_selection_undo()\n");
	layer = al_fread32le(fp);
	cep->selection.box.start_x = al_fread32le(fp);
	cep->selection.box.start_y = al_fread32le(fp);
	cep->selection.box.width = al_fread32le(fp);
	cep->selection.box.height = al_fread32le(fp);
	quixel_get_canvas_shift(cep->canvas, cep->selection.box.start_x, cep->selection.box.start_y, &shift_x, &shift_y);
	quixel_make_unfloat_selection_redo(cep, quixel_get_undo_path("redo", cep->redo_count, undo_path, 1024));
	cep->redo_count++;
	bp = al_load_bitmap_flags_f(fp, ".png", ALLEGRO_NO_PREMULTIPLIED_ALPHA);
	if(!bp)
	{
		goto fail;
	}
	cep->selection.bitmap = al_load_bitmap_flags_f(fp, ".png", ALLEGRO_NO_PREMULTIPLIED_ALPHA);
	if(!cep->selection.bitmap)
	{
		goto fail;
	}
	cep->selection.combined_bitmap = al_create_bitmap(al_get_bitmap_width(cep->selection.bitmap), al_get_bitmap_height(cep->selection.bitmap));
	if(!cep->selection.combined_bitmap)
	{
		goto fail;
	}

	quixel_import_bitmap_to_canvas(cep->canvas, bp, layer, cep->selection.box.start_x + shift_x * cep->canvas->bitmap_size, cep->selection.box.start_y + shift_y * cep->canvas->bitmap_size);
	al_destroy_bitmap(bp);
	bp = NULL;
	quixel_initialize_box(&cep->selection.box, cep->selection.box.start_x + shift_x * cep->canvas->bitmap_size, cep->selection.box.start_y + shift_y * cep->canvas->bitmap_size, cep->selection.box.width, cep->selection.box.height, cep->selection.box.bitmap);
	if(shift_x || shift_y)
	{
		quixel_shift_canvas_bitmap_array(cep->canvas, -shift_x, -shift_y);
		quixel_shift_canvas_editor_variables(cep, -shift_x * cep->canvas->bitmap_size, -shift_y * cep->canvas->bitmap_size);
	}
	quixel_update_box_handles(&cep->selection.box, cep->view_x, cep->view_y, cep->view_zoom);
	quixel_select_canvas_editor_tool(cep, QUIXEL_TOOL_SELECTION);
	t3f_debug_message("Exit quixel_apply_unfloat_selection_undo()\n");
	return true;

	fail:
	{
		t3f_debug_message("Fail quixel_apply_unfloat_selection_undo()\n");
		if(bp)
		{
			al_destroy_bitmap(bp);
		}
		if(cep->selection.bitmap)
		{
			al_destroy_bitmap(cep->selection.bitmap);
			cep->selection.bitmap = NULL;
		}
		if(cep->selection.combined_bitmap)
		{
			al_destroy_bitmap(cep->selection.combined_bitmap);
			cep->selection.combined_bitmap = NULL;
		}
		return false;
	}
}

bool quixel_apply_float_selection_undo(QUIXEL_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp, const char * action, bool revert)
{
	char undo_path[1024];
	ALLEGRO_BITMAP * bp = NULL;
	int layer;
	int new_x, new_y, new_width, new_height;
	int shift_x, shift_y;

	t3f_debug_message("Enter quixel_apply_float_selection_undo()\n");
	layer = al_fread32le(fp);
	new_x = al_fread32le(fp);
	new_y = al_fread32le(fp);
	bp = al_load_bitmap_flags_f(fp, ".png", ALLEGRO_NO_PREMULTIPLIED_ALPHA);
	if(!bp)
	{
		goto fail;
	}
	new_width = al_get_bitmap_width(bp);
	new_height = al_get_bitmap_height(bp);
	quixel_get_canvas_shift(cep->canvas, new_x, new_y, &shift_x, &shift_y);
	quixel_make_float_selection_redo(cep, new_x, new_y, bp,  quixel_get_undo_path("redo", cep->redo_count, undo_path, 1024));
	cep->redo_count++;
	quixel_import_bitmap_to_canvas(cep->canvas, bp, layer, new_x, new_y);
	al_destroy_bitmap(bp);
	if(shift_x || shift_y)
	{
		quixel_shift_canvas_bitmap_array(cep->canvas, -shift_x, -shift_y);
	}
	if(cep->selection.bitmap)
	{
		al_destroy_bitmap(cep->selection.bitmap);
		cep->selection.bitmap = NULL;
	}
	if(cep->selection.combined_bitmap)
	{
		al_destroy_bitmap(cep->selection.combined_bitmap);
		cep->selection.combined_bitmap = NULL;
	}
	quixel_initialize_box(&cep->selection.box, new_x, new_y, new_width, new_height, cep->selection.box.bitmap);
	quixel_update_box_handles(&cep->selection.box, cep->view_x, cep->view_y, cep->view_zoom);
	t3f_debug_message("Exit quixel_apply_float_selection_undo()\n");
	return true;

	fail:
	{
		t3f_debug_message("Fail quixel_apply_float_selection_undo()\n");
		if(bp)
		{
			al_destroy_bitmap(bp);
		}
		return false;
	}
}

bool quixel_apply_unfloat_selection_redo(QUIXEL_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp, const char * action)
{
	char undo_path[1024];
	ALLEGRO_BITMAP * bp = NULL;
	int layer;

	t3f_debug_message("Enter quixel_apply_unfloat_selection_redo()\n");
	layer = al_fread32le(fp);
	cep->selection.box.start_x = al_fread32le(fp);
	cep->selection.box.start_y = al_fread32le(fp);
	cep->selection.bitmap = al_load_bitmap_flags_f(fp, ".png", ALLEGRO_NO_PREMULTIPLIED_ALPHA);
	if(!cep->selection.bitmap)
	{
		goto fail;
	}
	cep->selection.combined_bitmap = al_create_bitmap(al_get_bitmap_width(cep->selection.bitmap), al_get_bitmap_height(cep->selection.bitmap));
	if(!cep->selection.combined_bitmap)
	{
		goto fail;
	}
	cep->selection.box.width = al_fread32le(fp);
	cep->selection.box.height = al_fread32le(fp);
	quixel_make_unfloat_selection_undo(cep, quixel_get_undo_path("undo", cep->undo_count, undo_path, 1024));
	cep->undo_count++;
	quixel_shift_canvas_editor_variables(cep, cep->shift_x * cep->canvas->bitmap_size, cep->shift_y * cep->canvas->bitmap_size);
	quixel_import_bitmap_to_canvas(cep->canvas, bp, layer, cep->selection.box.start_x, cep->selection.box.start_y);
	al_destroy_bitmap(bp);
	cep->selection.box.width = 0;
	cep->selection.box.height = 0;
	if(cep->selection.bitmap)
	{
		printf("selection undo error 1\n");
		al_destroy_bitmap(cep->selection.bitmap);
		cep->selection.bitmap = NULL;
	}
	t3f_debug_message("Exit quixel_apply_unfloat_selection_redo()\n");
	return true;

	fail:
	{
		t3f_debug_message("Fail quixel_apply_unfloat_selection_redo()\n");
		if(bp)
		{
			al_destroy_bitmap(bp);
		}
		if(cep->selection.bitmap)
		{
			al_destroy_bitmap(cep->selection.bitmap);
			cep->selection.bitmap = NULL;
		}
		if(cep->selection.combined_bitmap)
		{
			al_destroy_bitmap(cep->selection.combined_bitmap);
			cep->selection.combined_bitmap = NULL;
		}
		return false;
	}
}

bool quixel_apply_float_selection_redo(QUIXEL_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp, const char * action)
{
	char undo_path[1024];
	int layer;
	int new_x, new_y;

	t3f_debug_message("Enter quixel_apply_float_selection_redo()\n");
	quixel_make_float_selection_undo(cep, &cep->selection.box, quixel_get_undo_path("undo", cep->undo_count, undo_path, 1024));
	cep->undo_count++;
	layer = al_fread32le(fp);
	new_x = al_fread32le(fp);
	new_y = al_fread32le(fp);
	cep->selection.box.start_x = al_fread32le(fp);
	cep->selection.box.start_y = al_fread32le(fp);
	cep->selection.bitmap = al_load_bitmap_flags_f(fp, ".png", ALLEGRO_NO_PREMULTIPLIED_ALPHA);
	if(!cep->selection.bitmap)
	{
		goto fail;
	}
	cep->selection.combined_bitmap = al_create_bitmap(al_get_bitmap_width(cep->selection.bitmap), al_get_bitmap_height(cep->selection.bitmap));
	if(!cep->selection.combined_bitmap)
	{
		goto fail;
	}
	quixel_initialize_box(&cep->selection.box, cep->selection.box.start_x, cep->selection.box.start_y, cep->selection.box.width, cep->selection.box.height, cep->selection.box.bitmap);
	quixel_draw_primitive_to_canvas(cep->canvas, layer, new_x, new_y, new_x + cep->selection.box.width - 1, new_y + cep->selection.box.height - 1, NULL, al_map_rgba_f(0.0, 0.0, 0.0, 0.0), QUIXEL_RENDER_COPY, quixel_draw_filled_rectangle);
	quixel_update_box_handles(&cep->selection.box, cep->view_x, cep->view_y, cep->view_zoom);

	t3f_debug_message("Exit quixel_apply_float_selection_redo()\n");
	return true;

	fail:
	{
		t3f_debug_message("Fail quixel_apply_float_selection_redo()\n");
		if(cep->selection.bitmap)
		{
			al_destroy_bitmap(cep->selection.bitmap);
			cep->selection.bitmap = NULL;
		}
		if(cep->selection.combined_bitmap)
		{
			al_destroy_bitmap(cep->selection.combined_bitmap);
			cep->selection.combined_bitmap = NULL;
		}
		return false;
	}
}
