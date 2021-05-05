#include "instance.h"
#include "modules/canvas/canvas.h"
#include "ui/canvas_editor/undo.h"

static void make_frame_undo(QUIXEL_CANVAS_EDITOR * cep)
{
	char undo_path[1024];

	quixel_get_undo_path("undo", cep->undo_count, undo_path, 1024);
	if(quixel_make_frame_undo(cep, "Add Frame", undo_path))
	{
		cep->undo_count++;
		cep->redo_count = 0;
		quixel_update_undo_name(cep);
		quixel_update_redo_name(cep);
	}
}

int quixel_menu_frame_add_from_selection(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;
	char buf[256];

	if(app->canvas_editor->selection.box.width > 0 && app->canvas_editor->selection.box.height > 0)
	{
		make_frame_undo(app->canvas_editor);
		sprintf(buf, "Frame %d", app->canvas->frame_max + 1);
		quixel_add_canvas_frame(app->canvas, buf, app->canvas_editor->selection.box.start_x, app->canvas_editor->selection.box.start_y, app->canvas_editor->selection.box.width, app->canvas_editor->selection.box.height);
		app->canvas_editor->current_frame = app->canvas->frame_max - 1;
		t3f_refresh_menus();
	}
	return 0;
}

static bool add_frame(QUIXEL_CANVAS_EDITOR * cep, int width, int height)
{
	char buf[256];

	make_frame_undo(cep);
	sprintf(buf, "Frame %d", cep->canvas->frame_max + 1);
	quixel_add_canvas_frame(cep->canvas, buf, cep->view_x, cep->view_y, width, height);
	cep->current_frame = cep->canvas->frame_max - 1;
	t3f_refresh_menus();
	return true;
}

int quixel_menu_frame_add_8x8(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	add_frame(app->canvas_editor, 8, 8);
	return 0;
}

int quixel_menu_frame_add_12x12(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	add_frame(app->canvas_editor, 12, 12);
	return 0;
}

int quixel_menu_frame_add_16x16(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	add_frame(app->canvas_editor, 16, 16);
	return 0;
}

int quixel_menu_frame_add_24x24(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	add_frame(app->canvas_editor, 24, 24);
	return 0;
}

int quixel_menu_frame_add_32x32(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	add_frame(app->canvas_editor, 32, 32);
	return 0;
}

int quixel_menu_frame_add_48x48(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	add_frame(app->canvas_editor, 48, 48);
	return 0;
}

int quixel_menu_frame_add_64x64(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	add_frame(app->canvas_editor, 64, 64);
	return 0;
}

int quixel_menu_frame_add_72x72(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	add_frame(app->canvas_editor, 72, 72);
	return 0;
}

int quixel_menu_frame_add_96x96(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	add_frame(app->canvas_editor, 96, 96);
	return 0;
}

int quixel_menu_frame_add_128x128(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	add_frame(app->canvas_editor, 128, 128);
	return 0;
}

int quixel_menu_frame_add_256x256(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	add_frame(app->canvas_editor, 256, 256);
	return 0;
}

int quixel_menu_frame_add_512x512(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	add_frame(app->canvas_editor, 512, 512);
	return 0;
}


int quixel_menu_frame_add_256x224(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	add_frame(app->canvas_editor, 256, 224);
	return 0;
}

int quixel_menu_frame_add_320x200(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	add_frame(app->canvas_editor, 320, 200);
	return 0;
}

int quixel_menu_frame_add_320x240(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	add_frame(app->canvas_editor, 320, 240);
	return 0;
}

int quixel_menu_frame_add_640x480(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	add_frame(app->canvas_editor, 640, 480);
	return 0;
}


int quixel_menu_frame_delete(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	if(app->canvas_editor->current_frame < app->canvas->frame_max)
	{
		make_frame_undo(app->canvas_editor);
		quixel_remove_canvas_frame(app->canvas, app->canvas_editor->current_frame);
		t3f_refresh_menus();
	}
	return 0;
}

int quixel_menu_frame_previous(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	app->canvas_editor->current_frame--;
	if(app->canvas_editor->current_frame < 0)
	{
		app->canvas_editor->current_frame = app->canvas->frame_max - 1;
		if(app->canvas_editor->current_frame < 0)
		{
			app->canvas_editor->current_frame = 0;
		}
	}
	return 0;
}

int quixel_menu_frame_next(int id, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	app->canvas_editor->current_frame++;
	if(app->canvas_editor->current_frame >= app->canvas->frame_max)
	{
		app->canvas_editor->current_frame = 0;
	}
	return 0;
}
