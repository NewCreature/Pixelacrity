#include "t3f/t3f.h"
#include "instance.h"
#include "ui/menu/file_proc.h"
#include "ui/menu/edit_proc.h"
#include "ui/menu/layer_proc.h"
#include "ui/menu/frame_proc.h"
#include "ui/element/canvas_editor/MSG_WHEEL.h"
#include "modules/canvas/canvas_helpers.h"
#include "modules/color.h"

void pa_handle_shortcuts(APP_INSTANCE * app)
{
	int x, y, width, height;
	float speed;
	bool step = false;
	const char * val;

	if((t3f_key[ALLEGRO_KEY_LCTRL] || t3f_key[ALLEGRO_KEY_RCTRL] || t3f_key[ALLEGRO_KEY_COMMAND]) && (t3f_key[ALLEGRO_KEY_LSHIFT] || t3f_key[ALLEGRO_KEY_RSHIFT]) && t3f_key[ALLEGRO_KEY_X])
	{
		pa_menu_edit_multilayer_cut(0, app);
		t3f_key[ALLEGRO_KEY_X] = 0;
	}
	else if((t3f_key[ALLEGRO_KEY_LCTRL] || t3f_key[ALLEGRO_KEY_RCTRL] || t3f_key[ALLEGRO_KEY_COMMAND]) && t3f_key[ALLEGRO_KEY_X])
	{
		pa_menu_edit_cut(0, app);
		t3f_key[ALLEGRO_KEY_X] = 0;
	}
	else if((t3f_key[ALLEGRO_KEY_LCTRL] || t3f_key[ALLEGRO_KEY_RCTRL] || t3f_key[ALLEGRO_KEY_COMMAND]) && (t3f_key[ALLEGRO_KEY_LSHIFT] || t3f_key[ALLEGRO_KEY_RSHIFT]) && t3f_key[ALLEGRO_KEY_C])
	{
		pa_menu_edit_multilayer_copy(0, app);
		t3f_key[ALLEGRO_KEY_C] = 0;
	}
	else if((t3f_key[ALLEGRO_KEY_LCTRL] || t3f_key[ALLEGRO_KEY_RCTRL] || t3f_key[ALLEGRO_KEY_COMMAND]) && t3f_key[ALLEGRO_KEY_C])
	{
		pa_menu_edit_copy(0, app);
		t3f_key[ALLEGRO_KEY_C] = 0;
	}
	else if((t3f_key[ALLEGRO_KEY_LCTRL] || t3f_key[ALLEGRO_KEY_RCTRL] || t3f_key[ALLEGRO_KEY_COMMAND]) && (t3f_key[ALLEGRO_KEY_ALT] || t3f_key[ALLEGRO_KEY_ALTGR]) && t3f_key[ALLEGRO_KEY_V])
	{
		pa_menu_edit_paste_in_place(0, app);
		t3f_key[ALLEGRO_KEY_V] = 0;
	}
	else if((t3f_key[ALLEGRO_KEY_LCTRL] || t3f_key[ALLEGRO_KEY_RCTRL] || t3f_key[ALLEGRO_KEY_COMMAND]) && (t3f_key[ALLEGRO_KEY_LSHIFT] || t3f_key[ALLEGRO_KEY_RSHIFT]) && t3f_key[ALLEGRO_KEY_V])
	{
		pa_menu_edit_paste_merged(-1, app);
		t3f_key[ALLEGRO_KEY_V] = 0;
	}
	else if((t3f_key[ALLEGRO_KEY_LCTRL] || t3f_key[ALLEGRO_KEY_RCTRL] || t3f_key[ALLEGRO_KEY_COMMAND]) && t3f_key[ALLEGRO_KEY_V])
	{
		pa_menu_edit_paste(-1, app);
		t3f_key[ALLEGRO_KEY_V] = 0;
	}
	else if((t3f_key[ALLEGRO_KEY_LSHIFT] || t3f_key[ALLEGRO_KEY_RSHIFT]) && (t3f_key[ALLEGRO_KEY_DELETE] || t3f_key[ALLEGRO_KEY_BACKSPACE]))
	{
		pa_menu_edit_multilayer_delete(0, app);
		t3f_key[ALLEGRO_KEY_DELETE] = 0;
		t3f_key[ALLEGRO_KEY_BACKSPACE] = 0;
	}
	else if(t3f_key[ALLEGRO_KEY_DELETE] || t3f_key[ALLEGRO_KEY_BACKSPACE])
	{
		if(app->canvas_editor->current_tool == PA_TOOL_FRAME)
		{
			pa_menu_frame_delete(0, app);
		}
		else if(app->canvas_editor->current_tool == PA_TOOL_SELECTION)
		{
			pa_menu_edit_delete(0, app);
		}
		t3f_key[ALLEGRO_KEY_DELETE] = 0;
		t3f_key[ALLEGRO_KEY_BACKSPACE] = 0;
	}
	else if((t3f_key[ALLEGRO_KEY_LCTRL] || t3f_key[ALLEGRO_KEY_RCTRL] || t3f_key[ALLEGRO_KEY_COMMAND]) && t3f_key[ALLEGRO_KEY_Z])
	{
		pa_menu_edit_undo(0, app);
		t3f_key[ALLEGRO_KEY_Z] = 0;
	}
	else if((t3f_key[ALLEGRO_KEY_LCTRL] || t3f_key[ALLEGRO_KEY_RCTRL] || t3f_key[ALLEGRO_KEY_COMMAND]) && t3f_key[ALLEGRO_KEY_Y])
	{
		pa_menu_edit_redo(0, app);
		t3f_key[ALLEGRO_KEY_Y] = 0;
	}
	else if((t3f_key[ALLEGRO_KEY_LCTRL] || t3f_key[ALLEGRO_KEY_RCTRL] || t3f_key[ALLEGRO_KEY_COMMAND]) && t3f_key[ALLEGRO_KEY_PGUP])
	{
		pa_menu_layer_move_up(0, app);
		t3f_key[ALLEGRO_KEY_PGUP] = 0;
	}
	else if((t3f_key[ALLEGRO_KEY_LCTRL] || t3f_key[ALLEGRO_KEY_RCTRL] || t3f_key[ALLEGRO_KEY_COMMAND]) && t3f_key[ALLEGRO_KEY_PGDN])
	{
		pa_menu_layer_move_down(0, app);
		t3f_key[ALLEGRO_KEY_PGDN] = 0;
	}
	else if((t3f_key[ALLEGRO_KEY_LCTRL] || t3f_key[ALLEGRO_KEY_RCTRL] || t3f_key[ALLEGRO_KEY_COMMAND]) && t3f_key[ALLEGRO_KEY_S])
	{
		pa_menu_file_save(0, app);
		t3f_key[ALLEGRO_KEY_S] = 0;
	}
	else if((t3f_key[ALLEGRO_KEY_LCTRL] || t3f_key[ALLEGRO_KEY_RCTRL] || t3f_key[ALLEGRO_KEY_COMMAND]) && t3f_key[ALLEGRO_KEY_A])
	{
		pa_menu_file_save_as(0, app);
		t3f_key[ALLEGRO_KEY_A] = 0;
	}
	else if((t3f_key[ALLEGRO_KEY_LCTRL] || t3f_key[ALLEGRO_KEY_RCTRL] || t3f_key[ALLEGRO_KEY_COMMAND]) && t3f_key[ALLEGRO_KEY_O])
	{
		pa_menu_file_load(0, app);
		t3f_key[ALLEGRO_KEY_O] = 0;
	}
	else if((t3f_key[ALLEGRO_KEY_LCTRL] || t3f_key[ALLEGRO_KEY_RCTRL] || t3f_key[ALLEGRO_KEY_COMMAND]) && t3f_key[ALLEGRO_KEY_E])
	{
		pa_menu_file_export(0, app);
		t3f_key[ALLEGRO_KEY_E] = 0;
	}
	else if((t3f_key[ALLEGRO_KEY_LSHIFT] || t3f_key[ALLEGRO_KEY_RSHIFT]) && t3f_key[ALLEGRO_KEY_H])
	{
		pa_menu_edit_multilayer_flip_horizontal(0, app);
		t3f_key[ALLEGRO_KEY_H] = 0;
	}
	else if((t3f_key[ALLEGRO_KEY_LSHIFT] || t3f_key[ALLEGRO_KEY_RSHIFT]) && t3f_key[ALLEGRO_KEY_V])
	{
		pa_menu_edit_multilayer_flip_vertical(0, app);
		t3f_key[ALLEGRO_KEY_V] = 0;
	}
	else if((t3f_key[ALLEGRO_KEY_LSHIFT] || t3f_key[ALLEGRO_KEY_RSHIFT]) && t3f_key[ALLEGRO_KEY_FULLSTOP])
	{
		pa_menu_edit_multilayer_turn_clockwise(0, app);
		t3f_key[ALLEGRO_KEY_FULLSTOP] = 0;
	}
	else if((t3f_key[ALLEGRO_KEY_LSHIFT] || t3f_key[ALLEGRO_KEY_RSHIFT]) && t3f_key[ALLEGRO_KEY_COMMA])
	{
		pa_menu_edit_multilayer_turn_counter_clockwise(0, app);
		t3f_key[ALLEGRO_KEY_COMMA] = 0;
	}
	else if(t3f_key[ALLEGRO_KEY_H])
	{
		pa_menu_edit_flip_horizontal(0, app);
		t3f_key[ALLEGRO_KEY_H] = 0;
	}
	else if(t3f_key[ALLEGRO_KEY_V])
	{
		pa_menu_edit_flip_vertical(0, app);
		t3f_key[ALLEGRO_KEY_V] = 0;
	}
	else if(t3f_key[ALLEGRO_KEY_FULLSTOP])
	{
		pa_menu_edit_turn_clockwise(0, app);
		t3f_key[ALLEGRO_KEY_FULLSTOP] = 0;
	}
	else if(t3f_key[ALLEGRO_KEY_COMMA])
	{
		pa_menu_edit_turn_counter_clockwise(0, app);
		t3f_key[ALLEGRO_KEY_COMMA] = 0;
	}
	else if(t3f_key[ALLEGRO_KEY_QUOTE])
	{
		pa_menu_frame_next(0, app);
		t3f_key[ALLEGRO_KEY_QUOTE] = 0;
	}
	else if(t3f_key[ALLEGRO_KEY_SEMICOLON])
	{
		pa_menu_frame_previous(0, app);
		t3f_key[ALLEGRO_KEY_SEMICOLON] = 0;
	}
	else if((t3f_key[ALLEGRO_KEY_LCTRL] || t3f_key[ALLEGRO_KEY_RCTRL] || t3f_key[ALLEGRO_KEY_COMMAND]) && t3f_key[ALLEGRO_KEY_F])
	{
		pa_menu_frame_add_from_selection(0, app);
		t3f_key[ALLEGRO_KEY_F] = 0;
	}
	else if(t3f_key[ALLEGRO_KEY_C])
	{
		pa_get_canvas_dimensions(app->canvas_editor->canvas, &x, &y, &width, &height, 0, false);
		app->canvas_editor->view_x = x + width / 2 - (app->canvas_editor->editor_element->w / 2) / app->canvas_editor->view_zoom;
		app->canvas_editor->view_y = y + height / 2 - (app->canvas_editor->editor_element->h / 2) / app->canvas_editor->view_zoom;
		app->canvas_editor->view_fx = app->canvas_editor->view_x;
		app->canvas_editor->view_fy = app->canvas_editor->view_y;
		t3f_key[ALLEGRO_KEY_C] = 0;
	}
	else if(t3f_key[ALLEGRO_KEY_PGUP])
	{
		app->canvas_editor->current_layer++;
		t3f_key[ALLEGRO_KEY_PGUP] = 0;
	}
	else if(t3f_key[ALLEGRO_KEY_PGDN])
	{
		if(app->canvas_editor->current_layer > 0)
		{
			app->canvas_editor->current_layer--;
		}
		t3f_key[ALLEGRO_KEY_PGDN] = 0;
	}
	else if(t3f_key[ALLEGRO_KEY_1])
	{
		pa_select_canvas_editor_tool(app->canvas_editor, 0);
		t3f_key[ALLEGRO_KEY_1] = 0;
	}
	else if(t3f_key[ALLEGRO_KEY_2])
	{
		pa_select_canvas_editor_tool(app->canvas_editor, 1);
		t3f_key[ALLEGRO_KEY_2] = 0;
	}
	else if(t3f_key[ALLEGRO_KEY_3])
	{
		pa_select_canvas_editor_tool(app->canvas_editor, 2);
		t3f_key[ALLEGRO_KEY_3] = 0;
	}
	else if(t3f_key[ALLEGRO_KEY_4])
	{
		pa_select_canvas_editor_tool(app->canvas_editor, 3);
		t3f_key[ALLEGRO_KEY_4] = 0;
	}
	else if(t3f_key[ALLEGRO_KEY_5])
	{
		pa_select_canvas_editor_tool(app->canvas_editor, 4);
		t3f_key[ALLEGRO_KEY_5] = 0;
	}
	else if(t3f_key[ALLEGRO_KEY_6])
	{
		pa_select_canvas_editor_tool(app->canvas_editor, 5);
		t3f_key[ALLEGRO_KEY_6] = 0;
	}
	else if(t3f_key[ALLEGRO_KEY_7])
	{
		pa_select_canvas_editor_tool(app->canvas_editor, 6);
		t3f_key[ALLEGRO_KEY_7] = 0;
	}
	else if(t3f_key[ALLEGRO_KEY_8])
	{
		pa_select_canvas_editor_tool(app->canvas_editor, 7);
		t3f_key[ALLEGRO_KEY_8] = 0;
	}
	else if(t3f_key[ALLEGRO_KEY_9])
	{
		pa_select_canvas_editor_tool(app->canvas_editor, 8);
		t3f_key[ALLEGRO_KEY_9] = 0;
	}
	else if(t3f_key[ALLEGRO_KEY_0])
	{
		pa_select_canvas_editor_tool(app->canvas_editor, 9);
		t3f_key[ALLEGRO_KEY_0] = 0;
	}
	else if(t3f_key[ALLEGRO_KEY_I])
	{
		app->canvas_editor->view_isolate = !app->canvas_editor->view_isolate;
		t3f_key[ALLEGRO_KEY_I] = 0;
	}
	else if(t3f_key[ALLEGRO_KEY_B])
	{
		app->canvas_editor->view_break_out = !app->canvas_editor->view_break_out;
		app->canvas_editor->view_break_out_frame = 0;
		t3f_key[ALLEGRO_KEY_B] = 0;
	}
	else if((t3f_key[ALLEGRO_KEY_LCTRL] || t3f_key[ALLEGRO_KEY_RCTRL] || t3f_key[ALLEGRO_KEY_COMMAND]) && t3f_key[ALLEGRO_KEY_G])
	{
		if(app->canvas_editor->grid[0].space)
		{
			app->canvas_editor->grid[0].space = 0;
		}
		else
		{
			app->canvas_editor->grid[0].space = 1;
		}
		app->canvas_editor->grid[0].color = app->canvas_editor->grid_color;
		t3f_key[ALLEGRO_KEY_G] = 0;
	}
	else if(t3f_key[ALLEGRO_KEY_F8])
	{
		pa_optimize_canvas(app->canvas_editor, 1, 0);
		t3f_key[ALLEGRO_KEY_F8] = 0;
	}
	else if(t3f_key[ALLEGRO_KEY_F9])
	{
		pa_optimize_canvas(app->canvas_editor, 0, 1);
		t3f_key[ALLEGRO_KEY_F9] = 0;
	}
	else
	{
		speed = (8.0 / (float)app->canvas_editor->view_zoom);
		if(t3f_key[ALLEGRO_KEY_LSHIFT] || t3f_key[ALLEGRO_KEY_RSHIFT])
		{
			if(!t3f_key[ALLEGRO_KEY_LCTRL] && !t3f_key[ALLEGRO_KEY_RCTRL] && !t3f_key[ALLEGRO_KEY_COMMAND])
			{
				speed *= 2.0;
			}
			else
			{
				speed = app->canvas_editor->view_width - 16;
				step = true;
			}
		}
		else if(t3f_key[ALLEGRO_KEY_LCTRL] || t3f_key[ALLEGRO_KEY_RCTRL] || t3f_key[ALLEGRO_KEY_COMMAND])
		{
			speed = 1.0;
			step = true;
		}
		if(speed < 0.25)
		{
			speed = 0.25;
		}
		if(t3f_key[ALLEGRO_KEY_LEFT] || t3f_key[ALLEGRO_KEY_A])
		{
			app->canvas_editor->view_fx -= speed;
			app->canvas_editor->simulate_mouse_move = true;
			if(step)
			{
				t3f_key[ALLEGRO_KEY_LEFT] = 0;
				t3f_key[ALLEGRO_KEY_A] = 0;
			}
		}
		if(t3f_key[ALLEGRO_KEY_RIGHT] || t3f_key[ALLEGRO_KEY_D])
		{
			app->canvas_editor->view_fx += speed;
			app->canvas_editor->simulate_mouse_move = true;
			if(step)
			{
				t3f_key[ALLEGRO_KEY_RIGHT] = 0;
				t3f_key[ALLEGRO_KEY_D] = 0;
			}
		}
		if(t3f_key[ALLEGRO_KEY_UP] || t3f_key[ALLEGRO_KEY_W])
		{
			app->canvas_editor->view_fy -= speed;
			app->canvas_editor->simulate_mouse_move = true;
			if(step)
			{
				t3f_key[ALLEGRO_KEY_UP] = 0;
				t3f_key[ALLEGRO_KEY_W] = 0;
			}
		}
		if(t3f_key[ALLEGRO_KEY_DOWN] || t3f_key[ALLEGRO_KEY_S])
		{
			app->canvas_editor->view_fy += speed;
			app->canvas_editor->simulate_mouse_move = true;
			if(step)
			{
				t3f_key[ALLEGRO_KEY_DOWN] = 0;
				t3f_key[ALLEGRO_KEY_S] = 0;
			}
		}
		if(t3f_key[ALLEGRO_KEY_MINUS] || t3f_key[ALLEGRO_KEY_PAD_MINUS])
		{
			pa_canvas_editor_MSG_WHEEL(app->ui->element[PA_UI_ELEMENT_CANVAS_EDITOR], -1);
			t3f_key[ALLEGRO_KEY_MINUS] = 0;
			t3f_key[ALLEGRO_KEY_PAD_MINUS] = 0;
		}
		if(t3f_key[ALLEGRO_KEY_EQUALS] || t3f_key[ALLEGRO_KEY_PAD_PLUS])
		{
			pa_canvas_editor_MSG_WHEEL(app->ui->element[PA_UI_ELEMENT_CANVAS_EDITOR], 1);
			t3f_key[ALLEGRO_KEY_EQUALS] = 0;
			t3f_key[ALLEGRO_KEY_PAD_PLUS] = 0;
		}
	}
}
