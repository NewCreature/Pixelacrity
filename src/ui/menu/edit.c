#include "t3f/t3f.h"
#include "instance.h"
#include "edit_proc.h"

static int menu_undo_update_proc(ALLEGRO_MENU * mp, int item, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	al_set_menu_item_caption(mp, item, app->canvas_editor->undo_name);
	if(app->canvas_editor->undo_count > 0)
	{
		t3f_set_menu_item_flags(mp, item, 0);
	}
	else
	{
		t3f_set_menu_item_flags(mp, item, ALLEGRO_MENU_ITEM_DISABLED);
	}
	return 0;
}

static int menu_redo_update_proc(ALLEGRO_MENU * mp, int item, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	al_set_menu_item_caption(mp, item, app->canvas_editor->redo_name);
	if(app->canvas_editor->redo_count > 0)
	{
		t3f_set_menu_item_flags(mp, item, 0);
	}
	else
	{
		t3f_set_menu_item_flags(mp, item, ALLEGRO_MENU_ITEM_DISABLED);
	}
	return 0;
}

static int menu_edit_copy_update_proc(ALLEGRO_MENU * mp, int item, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	if(app->canvas_editor->selection.box.width > 0 && app->canvas_editor->selection.box.height > 0)
	{
		t3f_set_menu_item_flags(mp, item, 0);
	}
	else
	{
		t3f_set_menu_item_flags(mp, item, ALLEGRO_MENU_ITEM_DISABLED);
	}
	return 0;
}

static int menu_edit_paste_update_proc(ALLEGRO_MENU * mp, int item, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	if(app->canvas_editor->clipboard.bitmap_stack && (app->canvas_editor->clipboard.layer >= 0 || app->canvas_editor->clipboard.layer_max == app->canvas->layer_max))
	{
		t3f_set_menu_item_flags(mp, item, 0);
	}
	else
	{
		t3f_set_menu_item_flags(mp, item, ALLEGRO_MENU_ITEM_DISABLED);
	}
	return 0;
}

static int menu_edit_paste_merged_update_proc(ALLEGRO_MENU * mp, int item, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	if(app->canvas_editor->clipboard.bitmap_stack && app->canvas_editor->clipboard.layer < 0)
	{
		t3f_set_menu_item_flags(mp, item, 0);
	}
	else
	{
		t3f_set_menu_item_flags(mp, item, ALLEGRO_MENU_ITEM_DISABLED);
	}
	return 0;
}

static int menu_edit_unfloat_update_proc(ALLEGRO_MENU * mp, int item, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	if(app->canvas_editor->selection.bitmap_stack)
	{
		t3f_set_menu_item_flags(mp, item, 0);
	}
	else
	{
		t3f_set_menu_item_flags(mp, item, ALLEGRO_MENU_ITEM_DISABLED);
	}
	return 0;
}

static int menu_edit_float_update_proc(ALLEGRO_MENU * mp, int item, void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	if(app->canvas_editor->selection.box.width > 0 && app->canvas_editor->selection.box.height > 0 && !app->canvas_editor->selection.bitmap_stack)
	{
		t3f_set_menu_item_flags(mp, item, 0);
	}
	else
	{
		t3f_set_menu_item_flags(mp, item, ALLEGRO_MENU_ITEM_DISABLED);
	}
	return 0;
}

ALLEGRO_MENU * pa_create_edit_menu(ALLEGRO_MENU * multilayer_mp)
{
	ALLEGRO_MENU * mp = NULL;

	mp = al_create_menu();
	if(!mp)
	{
		return NULL;
	}
	t3f_add_menu_item(mp, "Undo", ALLEGRO_MENU_ITEM_DISABLED, NULL, pa_menu_edit_undo, menu_undo_update_proc);
	t3f_add_menu_item(mp, "Redo", ALLEGRO_MENU_ITEM_DISABLED, NULL, pa_menu_edit_redo, menu_redo_update_proc);
	t3f_add_menu_item(mp, NULL, 0, NULL, NULL, NULL);
	t3f_add_menu_item(mp, "Multi-Layer", 0, multilayer_mp, NULL, menu_edit_copy_update_proc);
	t3f_add_menu_item(mp, "Cut", 0, NULL, pa_menu_edit_cut, menu_edit_copy_update_proc);
	t3f_add_menu_item(mp, "Copy", 0, NULL, pa_menu_edit_copy, menu_edit_copy_update_proc);
	t3f_add_menu_item(mp, "Paste", 0, NULL, pa_menu_edit_paste, menu_edit_paste_update_proc);
	t3f_add_menu_item(mp, "Paste In Place", 0, NULL, pa_menu_edit_paste_in_place, menu_edit_paste_update_proc);
	t3f_add_menu_item(mp, "Paste Merged", 0, NULL, pa_menu_edit_paste_merged, menu_edit_paste_merged_update_proc);
	t3f_add_menu_item(mp, NULL, 0, NULL, NULL, NULL);
	t3f_add_menu_item(mp, "Flip Horizontally", 0, NULL, pa_menu_edit_flip_horizontal, menu_edit_copy_update_proc);
	t3f_add_menu_item(mp, "Flip Vertically", 0, NULL, pa_menu_edit_flip_vertical, menu_edit_copy_update_proc);
	t3f_add_menu_item(mp, "Turn Clockwise", 0, NULL, pa_menu_edit_turn_clockwise, menu_edit_copy_update_proc);
	t3f_add_menu_item(mp, "Turn Counter-Clockwise", 0, NULL, pa_menu_edit_turn_counter_clockwise, menu_edit_copy_update_proc);
	t3f_add_menu_item(mp, NULL, 0, NULL, NULL, NULL);
	t3f_add_menu_item(mp, "Float Selection", 0, NULL, pa_menu_edit_float_selection, menu_edit_float_update_proc);
	t3f_add_menu_item(mp, "Unfloat Selection", 0, NULL, pa_menu_edit_unfloat_selection, menu_edit_unfloat_update_proc);
	t3f_add_menu_item(mp, NULL, 0, NULL, NULL, NULL);
	t3f_add_menu_item(mp, "Delete", 0, NULL, pa_menu_edit_delete, menu_edit_copy_update_proc);

	return mp;
}

ALLEGRO_MENU * pa_create_edit_multilayer_menu(void)
{
	ALLEGRO_MENU * mp = NULL;

	mp = al_create_menu();
	if(!mp)
	{
		return NULL;
	}
	t3f_add_menu_item(mp, "Cut", 0, NULL, pa_menu_edit_multilayer_cut, menu_edit_copy_update_proc);
	t3f_add_menu_item(mp, "Copy", 0, NULL, pa_menu_edit_multilayer_copy, menu_edit_copy_update_proc);
	t3f_add_menu_item(mp, "Paste", 0, NULL, pa_menu_edit_paste, menu_edit_paste_update_proc);
	t3f_add_menu_item(mp, "Paste In Place", 0, NULL, pa_menu_edit_paste_in_place, menu_edit_paste_update_proc);
	t3f_add_menu_item(mp, NULL, 0, NULL, NULL, NULL);
	t3f_add_menu_item(mp, "Flip Horizontally", 0, NULL, pa_menu_edit_multilayer_flip_horizontal, menu_edit_copy_update_proc);
	t3f_add_menu_item(mp, "Flip Vertically", 0, NULL, pa_menu_edit_multilayer_flip_vertical, menu_edit_copy_update_proc);
	t3f_add_menu_item(mp, "Turn Clockwise", 0, NULL, pa_menu_edit_multilayer_turn_clockwise, menu_edit_copy_update_proc);
	t3f_add_menu_item(mp, "Turn Counter-Clockwise", 0, NULL, pa_menu_edit_multilayer_turn_counter_clockwise, menu_edit_copy_update_proc);
	t3f_add_menu_item(mp, NULL, 0, NULL, NULL, NULL);
	t3f_add_menu_item(mp, "Float Selection", 0, NULL, pa_menu_edit_multilayer_float_selection, menu_edit_float_update_proc);
	t3f_add_menu_item(mp, "Unfloat Selection", 0, NULL, pa_menu_edit_unfloat_selection, menu_edit_unfloat_update_proc);
	t3f_add_menu_item(mp, NULL, 0, NULL, NULL, NULL);
	t3f_add_menu_item(mp, "Delete", 0, NULL, pa_menu_edit_multilayer_delete, menu_edit_copy_update_proc);

	return mp;
}
