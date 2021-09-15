#include "t3f/t3f.h"
#include "instance.h"
#include "ui/canvas_editor/brush.h"
#include "modules/canvas/canvas_helpers.h"

int pa_menu_tool_brush_reset(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;
	ALLEGRO_BITMAP * bp;

	bp = pa_create_default_brush();
	if(bp)
	{
		al_destroy_bitmap(app->canvas_editor->brush);
		app->canvas_editor->brush = bp;
	}

	return 0;
}

static void grab_brush(PA_CANVAS_EDITOR * cep, ALLEGRO_BITMAP * bp, bool multi)
{
	ALLEGRO_STATE old_state;
	ALLEGRO_TRANSFORM identity;

	al_store_state(&old_state, ALLEGRO_STATE_TARGET_BITMAP |  ALLEGRO_STATE_BLENDER | ALLEGRO_STATE_TRANSFORM);
	al_set_target_bitmap(bp);
	al_identity_transform(&identity);
	al_use_transform(&identity);
	al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);
	if(!multi)
	{
		al_use_shader(cep->solid_shader);
	}
	if(cep->selection.bitmap_stack)
	{
		al_draw_bitmap(cep->selection.bitmap_stack->bitmap[cep->selection.layer], 0, 0, 0);
	}
	else
	{
		pa_render_canvas_layer(cep->canvas, cep->current_layer, cep->selection.box.start_x, cep->selection.box.start_y, 1, 0, 0, cep->selection.box.width, cep->selection.box.height);
	}
	al_use_shader(cep->standard_shader);
	al_restore_state(&old_state);
	al_destroy_bitmap(cep->brush);
	cep->brush = bp;
}

int pa_menu_tool_brush_grab_from_selection(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;
	ALLEGRO_BITMAP * bp;

	if(app->canvas_editor->selection.box.width > 0 && app->canvas_editor->selection.box.height > 0 && !(app->canvas_editor->selection.bitmap_stack && app->canvas_editor->selection.layer < 0))
	{
		bp = al_create_bitmap(app->canvas_editor->selection.box.width, app->canvas_editor->selection.box.height);
		if(bp)
		{
			grab_brush(app->canvas_editor, bp, false);
		}
	}
	return 0;
}

int pa_menu_tool_brush_grab_from_selection_multicolor(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;
	ALLEGRO_BITMAP * bp;

	if(app->canvas_editor->selection.box.width > 0 && app->canvas_editor->selection.box.height > 0 && !(app->canvas_editor->selection.bitmap_stack && app->canvas_editor->selection.layer < 0))
	{
		bp = al_create_bitmap(app->canvas_editor->selection.box.width, app->canvas_editor->selection.box.height);
		if(bp)
		{
			grab_brush(app->canvas_editor, bp, true);
		}
	}
	return 0;
}

static bool load_brush(PA_CANVAS_EDITOR * cep, const char * fn, bool multi)
{
	ALLEGRO_BITMAP * bp;
	ALLEGRO_STATE old_state;
	ALLEGRO_TRANSFORM identity;

	bp = al_load_bitmap_flags(fn, ALLEGRO_NO_PREMULTIPLIED_ALPHA);
	if(bp)
	{
		al_destroy_bitmap(cep->brush);
		cep->brush = al_create_bitmap(al_get_bitmap_width(bp), al_get_bitmap_height(bp));
		if(cep->brush)
		{
			al_store_state(&old_state, ALLEGRO_STATE_TARGET_BITMAP | ALLEGRO_STATE_BLENDER | ALLEGRO_STATE_TRANSFORM);
			al_set_target_bitmap(cep->brush);
			al_identity_transform(&identity);
			al_use_transform(&identity);
			al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);
			if(!multi)
			{
				al_use_shader(cep->solid_shader);
			}
			al_draw_bitmap(bp, 0, 0, 0);
			al_use_shader(cep->standard_shader);
			al_restore_state(&old_state);
			return true;
		}
	}
	return false;
}

static void load_helper(PA_CANVAS_EDITOR * cep, bool multi)
{
	ALLEGRO_FILECHOOSER * fc;
	const char * val;
	ALLEGRO_PATH * pp;
	const char * file_path;

	val = al_get_config_value(t3f_config, "App Data", "last_brush_path");
	fc = al_create_native_file_dialog(val, "Choose image file...", "*.png;*.tga;*.pcx;*.bmp;*.jpg", ALLEGRO_FILECHOOSER_FILE_MUST_EXIST);
	if(fc)
	{
		al_stop_timer(t3f_timer);
		if(al_show_native_file_dialog(t3f_display, fc))
		{
			if(al_get_native_file_dialog_count(fc) > 0)
			{
				file_path = al_get_native_file_dialog_path(fc, 0);
				if(file_path)
				{
					if(load_brush(cep, file_path, multi))
					{
						pp = al_create_path(file_path);
						if(pp)
						{
							al_set_path_filename(pp, "");
							al_set_config_value(t3f_config, "App Data", "last_brush_path", al_path_cstr(pp, '/'));
							al_destroy_path(pp);
						}
					}
				}
			}
		}
		al_destroy_native_file_dialog(fc);
		al_start_timer(t3f_timer);
	}
}

int pa_menu_tool_brush_load(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	load_helper(app->canvas_editor, false);

	return 0;
}

int pa_menu_tool_brush_load_multicolor(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	load_helper(app->canvas_editor, true);

	return 0;
}

int pa_menu_tool_pixel(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	pa_select_canvas_editor_tool(app->canvas_editor, PA_TOOL_PIXEL);

	return 0;
}

int pa_menu_tool_line(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	pa_select_canvas_editor_tool(app->canvas_editor, PA_TOOL_LINE);

	return 0;
}

int pa_menu_tool_rectangle(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	pa_select_canvas_editor_tool(app->canvas_editor, PA_TOOL_RECTANGLE);

	return 0;
}

int pa_menu_tool_filled_rectangle(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	pa_select_canvas_editor_tool(app->canvas_editor, PA_TOOL_FILLED_RECTANGLE);

	return 0;
}

int pa_menu_tool_oval(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	pa_select_canvas_editor_tool(app->canvas_editor, PA_TOOL_OVAL);

	return 0;
}

int pa_menu_tool_filled_oval(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	pa_select_canvas_editor_tool(app->canvas_editor, PA_TOOL_FILLED_OVAL);

	return 0;
}

int pa_menu_tool_flood_fill(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	pa_select_canvas_editor_tool(app->canvas_editor, PA_TOOL_FLOOD_FILL);

	return 0;
}

int pa_menu_tool_eraser(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	pa_select_canvas_editor_tool(app->canvas_editor, PA_TOOL_ERASER);

	return 0;
}

int pa_menu_tool_dropper(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	pa_select_canvas_editor_tool(app->canvas_editor, PA_TOOL_DROPPER);

	return 0;
}

int pa_menu_tool_selector(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	pa_select_canvas_editor_tool(app->canvas_editor, PA_TOOL_SELECTION);

	return 0;
}

int pa_menu_tool_frame(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	pa_select_canvas_editor_tool(app->canvas_editor, PA_TOOL_FRAME);

	return 0;
}
