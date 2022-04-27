#include "modules/mouse_cursor.h"
#include "ui/canvas_editor/canvas_editor.h"
#include "ui/dialog.h"
#include "ui/menu/file.h"

static void select_button(PA_DIALOG * dp, int button)
{
	pa_get_dialog_element(dp, PA_UI_ELEMENT_BUTTON_PIXEL)->flags &= ~D_SELECTED;
	pa_get_dialog_element(dp, PA_UI_ELEMENT_BUTTON_LINE)->flags &= ~D_SELECTED;
	pa_get_dialog_element(dp, PA_UI_ELEMENT_BUTTON_RECTANGLE)->flags &= ~D_SELECTED;
	pa_get_dialog_element(dp, PA_UI_ELEMENT_BUTTON_FILLED_RECTANGLE)->flags &= ~D_SELECTED;
	pa_get_dialog_element(dp, PA_UI_ELEMENT_BUTTON_OVAL)->flags &= ~D_SELECTED;
	pa_get_dialog_element(dp, PA_UI_ELEMENT_BUTTON_FILLED_OVAL)->flags &= ~D_SELECTED;
	pa_get_dialog_element(dp, PA_UI_ELEMENT_BUTTON_ERASER)->flags &= ~D_SELECTED;
	pa_get_dialog_element(dp, PA_UI_ELEMENT_BUTTON_FLOOD_FILL)->flags &= ~D_SELECTED;
	pa_get_dialog_element(dp, PA_UI_ELEMENT_BUTTON_REPLACE)->flags &= ~D_SELECTED;
	pa_get_dialog_element(dp, PA_UI_ELEMENT_BUTTON_DROPPER)->flags &= ~D_SELECTED;
	pa_get_dialog_element(dp, PA_UI_ELEMENT_BUTTON_SELECTION)->flags &= ~D_SELECTED;
	pa_get_dialog_element(dp, PA_UI_ELEMENT_BUTTON_FRAME)->flags &= ~D_SELECTED;
	if(button >= 0)
	{
		pa_get_dialog_element(dp, button)->flags |= D_SELECTED;
	}
}

static void update_toolbar_flags(PA_DIALOG * dp, PA_CANVAS_EDITOR * cep)
{
	if(!pa_can_export(cep))
	{
		pa_get_dialog_element(dp, PA_UI_ELEMENT_BUTTON_EXPORT)->flags |= D_DISABLED;
	}
	else
	{
		pa_get_dialog_element(dp, PA_UI_ELEMENT_BUTTON_EXPORT)->flags &= ~D_DISABLED;
	}
	if(!cep->undo_count)
	{
		pa_get_dialog_element(dp, PA_UI_ELEMENT_BUTTON_UNDO)->flags |= D_DISABLED;
		pa_get_dialog_element(dp, PA_UI_ELEMENT_BUTTON_UNDO)->flags &= ~(D_SELECTED | D_GOTFOCUS | D_GOTMOUSE);
	}
	else
	{
		pa_get_dialog_element(dp, PA_UI_ELEMENT_BUTTON_UNDO)->flags &= ~D_DISABLED;
	}
	if(!cep->redo_count)
	{
		pa_get_dialog_element(dp, PA_UI_ELEMENT_BUTTON_REDO)->flags |= D_DISABLED;
		pa_get_dialog_element(dp, PA_UI_ELEMENT_BUTTON_REDO)->flags &= ~(D_SELECTED | D_GOTFOCUS | D_GOTMOUSE);
	}
	else
	{
		pa_get_dialog_element(dp, PA_UI_ELEMENT_BUTTON_REDO)->flags &= ~D_DISABLED;
	}
	if(cep->selection.box.width > 0 && cep->selection.box.height > 0)
	{
		pa_get_dialog_element(dp, PA_UI_ELEMENT_BUTTON_CUT)->flags &= ~D_DISABLED;
		pa_get_dialog_element(dp, PA_UI_ELEMENT_BUTTON_COPY)->flags &= ~D_DISABLED;
		pa_get_dialog_element(dp, PA_UI_ELEMENT_BUTTON_TURN_CW)->flags &= ~D_DISABLED;
		pa_get_dialog_element(dp, PA_UI_ELEMENT_BUTTON_TURN_CCW)->flags &= ~D_DISABLED;
		pa_get_dialog_element(dp, PA_UI_ELEMENT_BUTTON_FLIP_HORIZONTAL)->flags &= ~D_DISABLED;
		pa_get_dialog_element(dp, PA_UI_ELEMENT_BUTTON_FLIP_VERTICAL)->flags &= ~D_DISABLED;
	}
	else
	{
		pa_get_dialog_element(dp, PA_UI_ELEMENT_BUTTON_CUT)->flags |= D_DISABLED;
		pa_get_dialog_element(dp, PA_UI_ELEMENT_BUTTON_CUT)->flags &= ~(D_SELECTED | D_GOTFOCUS | D_GOTMOUSE);
		pa_get_dialog_element(dp, PA_UI_ELEMENT_BUTTON_COPY)->flags |= D_DISABLED;
		pa_get_dialog_element(dp, PA_UI_ELEMENT_BUTTON_COPY)->flags &= ~(D_SELECTED | D_GOTFOCUS | D_GOTMOUSE);
		pa_get_dialog_element(dp, PA_UI_ELEMENT_BUTTON_TURN_CW)->flags |= D_DISABLED;
		pa_get_dialog_element(dp, PA_UI_ELEMENT_BUTTON_TURN_CW)->flags &= ~(D_SELECTED | D_GOTFOCUS | D_GOTMOUSE);
		pa_get_dialog_element(dp, PA_UI_ELEMENT_BUTTON_TURN_CCW)->flags |= D_DISABLED;
		pa_get_dialog_element(dp, PA_UI_ELEMENT_BUTTON_TURN_CCW)->flags &= ~(D_SELECTED | D_GOTFOCUS | D_GOTMOUSE);
		pa_get_dialog_element(dp, PA_UI_ELEMENT_BUTTON_FLIP_HORIZONTAL)->flags |= D_DISABLED;
		pa_get_dialog_element(dp, PA_UI_ELEMENT_BUTTON_FLIP_HORIZONTAL)->flags &= ~(D_SELECTED | D_GOTFOCUS | D_GOTMOUSE);
		pa_get_dialog_element(dp, PA_UI_ELEMENT_BUTTON_FLIP_VERTICAL)->flags |= D_DISABLED;
		pa_get_dialog_element(dp, PA_UI_ELEMENT_BUTTON_FLIP_VERTICAL)->flags &= ~(D_SELECTED | D_GOTFOCUS | D_GOTMOUSE);
	}
	if(cep->clipboard.bitmap_stack)
	{
		pa_get_dialog_element(dp, PA_UI_ELEMENT_BUTTON_PASTE)->flags &= ~D_DISABLED;
	}
	else
	{
		pa_get_dialog_element(dp, PA_UI_ELEMENT_BUTTON_PASTE)->flags |= D_DISABLED;
	}

	if(cep->tool_outline)
	{
		pa_get_dialog_element(dp, PA_UI_ELEMENT_BUTTON_TOOL_MODE_OUTLINE)->flags |= D_SELECTED;
	}
	else
	{
		pa_get_dialog_element(dp, PA_UI_ELEMENT_BUTTON_TOOL_MODE_OUTLINE)->flags &= ~D_SELECTED;
	}
}

static void update_layer_list(PA_DIALOG * dp, PA_CANVAS_EDITOR * cep)
{
	T3GUI_ELEMENT * d = pa_get_dialog_element(dp, PA_UI_ELEMENT_LAYER_LIST);
	int entry_height = al_get_font_line_height(d->theme->state[0].font[0]) + d->theme->state[0].top_margin + d->theme->state[0].bottom_margin;
  int visible_elements = d->h / entry_height;

	while(d->d1 < d->d2)
	{
		d->d2 -= visible_elements;
		if(d->d2 < 0)
		{
			d->d2 = 0;
			break;
		}
	}
	while(d->d1 >= d->d2 + visible_elements)
	{
		d->d2 += visible_elements;
		if(d->d2 + visible_elements > cep->canvas->layer_max)
		{
			d->d2 = cep->canvas->layer_max - visible_elements;
			break;
		}
	}
}

void pa_main_dialog_pre_logic(PA_DIALOG * dp, PA_CANVAS_EDITOR * cep)
{
	pa_get_dialog_element(dp, PA_UI_ELEMENT_LAYER_LIST)->d1 = cep->canvas->layer_max - cep->current_layer - 1;
	dp->variable[PA_UI_VAR_OLD_LAYER_D1] = pa_get_dialog_element(dp, PA_UI_ELEMENT_LAYER_LIST)->d1;
	dp->variable[PA_UI_VAR_OLD_LAYER] = cep->current_layer;
	if(cep->canvas->layer_max > 1)
	{
		pa_get_dialog_element(dp, PA_UI_ELEMENT_BUTTON_REMOVE_LAYER)->flags &= ~D_DISABLED;
	}
	else
	{
		pa_get_dialog_element(dp, PA_UI_ELEMENT_BUTTON_REMOVE_LAYER)->flags |= D_DISABLED;
	}

	if(cep->current_tool != PA_TOOL_SELECTION && cep->current_tool != PA_TOOL_FRAME)
	{
		cep->want_cursor = ALLEGRO_SYSTEM_MOUSE_CURSOR_PRECISION;
	}
	if(t3gui_get_active_dialogs() < 2 && t3gui_get_mouse_x() >= pa_get_dialog_element(dp, PA_UI_ELEMENT_CANVAS_EDITOR)->x && t3gui_get_mouse_x() < pa_get_dialog_element(dp, PA_UI_ELEMENT_CANVAS_EDITOR)->x + pa_get_dialog_element(dp, PA_UI_ELEMENT_CANVAS_EDITOR)->w && t3gui_get_mouse_y() >= pa_get_dialog_element(dp, PA_UI_ELEMENT_CANVAS_EDITOR)->y && t3gui_get_mouse_y() < pa_get_dialog_element(dp, PA_UI_ELEMENT_CANVAS_EDITOR)->y + pa_get_dialog_element(dp, PA_UI_ELEMENT_CANVAS_EDITOR)->h)
	{
		pa_set_mouse_cursor(cep->want_cursor);
	}
	else
	{
		pa_set_mouse_cursor(ALLEGRO_SYSTEM_MOUSE_CURSOR_ARROW);
	}

	update_toolbar_flags(dp, cep);
}

void pa_main_dialog_post_logic(PA_DIALOG * dp, PA_CANVAS_EDITOR * cep)
{
	/* update button selection */
	switch(cep->current_tool)
	{
		case PA_TOOL_PIXEL:
		{
			select_button(dp, PA_UI_ELEMENT_BUTTON_PIXEL);
			break;
		}
		case PA_TOOL_LINE:
		{
			select_button(dp, PA_UI_ELEMENT_BUTTON_LINE);
			break;
		}
		case PA_TOOL_RECTANGLE:
		{
			select_button(dp, PA_UI_ELEMENT_BUTTON_RECTANGLE);
			break;
		}
		case PA_TOOL_FILLED_RECTANGLE:
		{
			select_button(dp, PA_UI_ELEMENT_BUTTON_FILLED_RECTANGLE);
			break;
		}
		case PA_TOOL_OVAL:
		{
			select_button(dp, PA_UI_ELEMENT_BUTTON_OVAL);
			break;
		}
		case PA_TOOL_FILLED_OVAL:
		{
			select_button(dp, PA_UI_ELEMENT_BUTTON_FILLED_OVAL);
			break;
		}
		case PA_TOOL_FLOOD_FILL:
		{
			select_button(dp, PA_UI_ELEMENT_BUTTON_FLOOD_FILL);
			break;
		}
		case PA_TOOL_REPLACE:
		{
			select_button(dp, PA_UI_ELEMENT_BUTTON_REPLACE);
			break;
		}
		case PA_TOOL_ERASER:
		{
			select_button(dp, PA_UI_ELEMENT_BUTTON_ERASER);
			break;
		}
		case PA_TOOL_DROPPER:
		{
			select_button(dp, PA_UI_ELEMENT_BUTTON_DROPPER);
			break;
		}
		case PA_TOOL_SELECTION:
		{
			select_button(dp, PA_UI_ELEMENT_BUTTON_SELECTION);
			break;
		}
		case PA_TOOL_FRAME:
		{
			select_button(dp, PA_UI_ELEMENT_BUTTON_FRAME);
			break;
		}
		default:
		{
			select_button(dp, -1);
		}
	}
	if(pa_get_dialog_element(dp, PA_UI_ELEMENT_LAYER_LIST)->d1 != dp->variable[PA_UI_VAR_OLD_LAYER_D1])
	{
		pa_select_canvas_editor_layer(cep, cep->canvas->layer_max - pa_get_dialog_element(dp, PA_UI_ELEMENT_LAYER_LIST)->d1 - 1);
	}
	pa_get_dialog_element(dp, PA_UI_ELEMENT_LAYER_LIST)->d1 = cep->canvas->layer_max - cep->current_layer - 1;
	if(cep->current_layer != dp->variable[PA_UI_VAR_OLD_LAYER])
	{
		update_layer_list(dp, cep);
	}
}
