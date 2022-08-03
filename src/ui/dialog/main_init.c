#include "modules/bitmap.h"
#include "ui/ui.h"
#include "ui/dialog.h"
#include "ui/element/color.h"
#include "ui/element/brush.h"
#include "ui/element/list.h"
#include "ui/element/map.h"
#include "ui/element/palette.h"
#include "ui/element/shader.h"
#include "ui/element/canvas_editor/element.h"
#include "ui/element_proc/button_proc.h"
#include "ui/element_proc/layer_list_proc.h"

static bool load_bitmap(PA_DIALOG * dp, int slot, const char * name)
{
	dp->bitmap[slot] = pa_load_theme_bitmap(dp->theme, name);
	if(!dp->bitmap[slot])
	{
		return false;
	}
	return true;
}

static bool load_resources(PA_DIALOG * dp)
{
	if(!load_bitmap(dp, PA_UI_BITMAP_TOOL_PIXEL, "tool_pixel"))
	{
		return false;
	}
	if(!load_bitmap(dp, PA_UI_BITMAP_TOOL_LINE, "tool_line"))
	{
		return false;
	}
	if(!load_bitmap(dp, PA_UI_BITMAP_TOOL_RECTANGLE, "tool_rectangle"))
	{
		return false;
	}
	if(!load_bitmap(dp, PA_UI_BITMAP_TOOL_FILLED_RECTANGLE, "tool_filled_rectangle"))
	{
		return false;
	}
	if(!load_bitmap(dp, PA_UI_BITMAP_TOOL_OVAL, "tool_oval"))
	{
		return false;
	}
	if(!load_bitmap(dp, PA_UI_BITMAP_TOOL_FILLED_OVAL, "tool_filled_oval"))
	{
		return false;
	}
	if(!load_bitmap(dp, PA_UI_BITMAP_TOOL_FLOOD, "tool_flood_fill"))
	{
		return false;
	}
	if(!load_bitmap(dp, PA_UI_BITMAP_TOOL_REPLACE, "tool_replace"))
	{
		return false;
	}
	if(!load_bitmap(dp, PA_UI_BITMAP_TOOL_ERASER, "tool_eraser"))
	{
		return false;
	}
	if(!load_bitmap(dp, PA_UI_BITMAP_TOOL_DROPPER, "tool_dropper"))
	{
		return false;
	}
	if(!load_bitmap(dp, PA_UI_BITMAP_TOOL_SELECTION, "tool_selection"))
	{
		return false;
	}
	if(!load_bitmap(dp, PA_UI_BITMAP_TOOL_FRAME, "tool_frame"))
	{
		return false;
	}
	if(!load_bitmap(dp, PA_UI_BITMAP_TOOL_OUTLINE, "tool_outline"))
	{
		return false;
	}
	if(!load_bitmap(dp, PA_UI_BITMAP_TOOL_COMPOSITE, "tool_composite"))
	{
		return false;
	}
	if(!load_bitmap(dp, PA_UI_BITMAP_TOOLBAR_NEW, "toolbar_new"))
	{
		return false;
	}
	if(!load_bitmap(dp, PA_UI_BITMAP_TOOLBAR_OPEN, "toolbar_open"))
	{
		return false;
	}
	if(!load_bitmap(dp, PA_UI_BITMAP_TOOLBAR_SAVE, "toolbar_save"))
	{
		return false;
	}
	if(!load_bitmap(dp, PA_UI_BITMAP_TOOLBAR_IMPORT, "toolbar_import"))
	{
		return false;
	}
	if(!load_bitmap(dp, PA_UI_BITMAP_TOOLBAR_EXPORT, "toolbar_export"))
	{
		return false;
	}
	if(!load_bitmap(dp, PA_UI_BITMAP_TOOLBAR_UNDO, "toolbar_undo"))
	{
		return false;
	}
	if(!load_bitmap(dp, PA_UI_BITMAP_TOOLBAR_REDO, "toolbar_redo"))
	{
		return false;
	}
	if(!load_bitmap(dp, PA_UI_BITMAP_TOOLBAR_CUT, "toolbar_cut"))
	{
		return false;
	}
	if(!load_bitmap(dp, PA_UI_BITMAP_TOOLBAR_COPY, "toolbar_copy"))
	{
		return false;
	}
	if(!load_bitmap(dp, PA_UI_BITMAP_TOOLBAR_PASTE, "toolbar_paste"))
	{
		return false;
	}
	if(!load_bitmap(dp, PA_UI_BITMAP_TOOLBAR_TURN_CW, "toolbar_turn_cw"))
	{
		return false;
	}
	if(!load_bitmap(dp, PA_UI_BITMAP_TOOLBAR_TURN_CCW, "toolbar_turn_ccw"))
	{
		return false;
	}
	if(!load_bitmap(dp, PA_UI_BITMAP_TOOLBAR_FLIP_H, "toolbar_flip_h"))
	{
		return false;
	}
	if(!load_bitmap(dp, PA_UI_BITMAP_TOOLBAR_FLIP_V, "toolbar_flip_v"))
	{
		return false;
	}
	if(!load_bitmap(dp, PA_UI_BITMAP_LAYER_ADD, "layer_add"))
	{
		return false;
	}
	if(!load_bitmap(dp, PA_UI_BITMAP_LAYER_REMOVE, "layer_remove"))
	{
		return false;
	}
	if(!load_bitmap(dp, PA_UI_BITMAP_BOX_HANDLE, "box_handle"))
	{
		return false;
	}
	if(!load_bitmap(dp, PA_UI_BITMAP_SELECTION_SINGLE, "single_layer_selection"))
	{
		return false;
	}
	if(!load_bitmap(dp, PA_UI_BITMAP_SELECTION_MULTI, "multi_layer_selection"))
	{
		return false;
	}

	return true;
}

static bool add_color_pickers(PA_DIALOG * dp, PA_CANVAS_EDITOR * cep, int x, int y)
{
	int i;

	dp->variable[PA_UI_VAR_LEFT_SHADE_PICKER_INDEX] = dp->dialog->elements;
	for(i = 0; i < PA_COLOR_PICKER_SHADES; i++)
	{
		t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_COLOR_SELECT], pa_gui_color_proc, 0, 0, 0, 0, 0, 0, 0, 0, pa_create_gui_color_data(&cep->left_shade_color[i], &cep->left_color, &cep->right_color, NULL, NULL, NULL, NULL), NULL, NULL);

	}
	dp->variable[PA_UI_VAR_LEFT_ALPHA_PICKER_INDEX] = dp->dialog->elements;
	for(i = 0; i < PA_COLOR_PICKER_SHADES; i++)
	{
		t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_COLOR_SELECT], pa_gui_color_proc, 0, 0, 0, 0, 0, 0, 0, 0, pa_create_gui_color_data(&cep->left_alpha_color[i], &cep->left_color, &cep->right_color, NULL, NULL, NULL, NULL), NULL, NULL);
	}
	dp->variable[PA_UI_VAR_RIGHT_SHADE_PICKER_INDEX] = dp->dialog->elements;
	for(i = 0; i < PA_COLOR_PICKER_SHADES; i++)
	{
		t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_COLOR_SELECT], pa_gui_color_proc, 0, 0, 0, 0, 0, 0, 0, 0, pa_create_gui_color_data(&cep->right_shade_color[i], &cep->left_color, &cep->right_color, NULL, NULL, NULL, NULL), NULL, NULL);
	}
	dp->variable[PA_UI_VAR_RIGHT_ALPHA_PICKER_INDEX] = dp->dialog->elements;
	for(i = 0; i < PA_COLOR_PICKER_SHADES; i++)
	{
		t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_COLOR_SELECT], pa_gui_color_proc, 0, 0, 0, 0, 0, 0, 0, 0, pa_create_gui_color_data(&cep->right_alpha_color[i], &cep->left_color, &cep->right_color, NULL, NULL, NULL, NULL), NULL, NULL);
	}

	return true;
}

static bool add_color_palette(PA_DIALOG * dp, PA_CANVAS_EDITOR * cep, int x, int y)
{
	T3GUI_ELEMENT * ep;
	int i, j;

	dp->variable[PA_UI_VAR_PALETTE_COLOR_INDEX] = dp->dialog->elements;
	for(i = 0; i < PA_COLOR_PICKER_SHADES; i++)
	{
		for(j = 0; j < PA_COLOR_PICKER_SHADES; j++)
		{
			ep = t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_COLOR_SELECT], pa_gui_color_proc, 0, 0, 0, 0, 0, 0, 1, 0, pa_create_gui_color_data(&cep->palette->color[i * PA_COLOR_PICKER_SHADES + j], &cep->left_color, &cep->right_color, &cep->left_color.color, &cep->right_color.color, &cep->left_color.clicked, &cep->right_color.clicked), NULL, NULL);
		}
	}

	return true;
}

static void add_toolbar(PA_DIALOG * dp, PA_CANVAS_EDITOR * cep)
{
	T3GUI_ELEMENT * ep;
	int scale = pa_get_theme_int(dp->theme, "graphics_scale", 1);

	ep = t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_TOP_PANE], t3gui_box_proc, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL);
	pa_track_dialog_element(dp, ep, PA_UI_ELEMENT_TOOLBAR);

	ep = t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_BUTTON], t3gui_push_button_proc, 0, 0, 0, 0, 0, 0, scale, 0, "Pixel", pa_toolbar_new_button_proc, dp->bitmap[PA_UI_BITMAP_TOOLBAR_NEW]);
	pa_track_dialog_element(dp, ep, PA_UI_ELEMENT_BUTTON_NEW);

	ep = t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_BUTTON], t3gui_push_button_proc, 0, 0, 0, 0, 0, 0, scale, 0, "Pixel", pa_toolbar_open_button_proc, dp->bitmap[PA_UI_BITMAP_TOOLBAR_OPEN]);
	pa_track_dialog_element(dp, ep, PA_UI_ELEMENT_BUTTON_OPEN);

	ep = t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_BUTTON], t3gui_push_button_proc, 0, 0, 0, 0, 0, 0, scale, 0, "Pixel", pa_toolbar_save_button_proc, dp->bitmap[PA_UI_BITMAP_TOOLBAR_SAVE]);
	pa_track_dialog_element(dp, ep, PA_UI_ELEMENT_BUTTON_SAVE);

	ep = t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_BUTTON], t3gui_push_button_proc, 0, 0, 0, 0, 0, 0, scale, 0, "Pixel", pa_toolbar_import_button_proc, dp->bitmap[PA_UI_BITMAP_TOOLBAR_IMPORT]);
	pa_track_dialog_element(dp, ep, PA_UI_ELEMENT_BUTTON_IMPORT);

	ep = t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_BUTTON], t3gui_push_button_proc, 0, 0, 0, 0, 0, 0, scale, 0, "Pixel", pa_toolbar_export_button_proc, dp->bitmap[PA_UI_BITMAP_TOOLBAR_EXPORT]);
	pa_track_dialog_element(dp, ep, PA_UI_ELEMENT_BUTTON_EXPORT);

	ep = t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_SEPARATOR], t3gui_box_proc, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL);
	pa_track_dialog_element(dp, ep, PA_UI_ELEMENT_TOOLBAR_FILE_SEPARATOR);

	ep = t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_BUTTON], t3gui_push_button_proc, 0, 0, 0, 0, 0, 0, scale, 0, "Pixel", pa_toolbar_undo_button_proc, dp->bitmap[PA_UI_BITMAP_TOOLBAR_UNDO]);
	pa_track_dialog_element(dp, ep, PA_UI_ELEMENT_BUTTON_UNDO);

	ep = t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_BUTTON], t3gui_push_button_proc, 0, 0, 0, 0, 0, 0, scale, 0, "Pixel", pa_toolbar_redo_button_proc, dp->bitmap[PA_UI_BITMAP_TOOLBAR_REDO]);
	pa_track_dialog_element(dp, ep, PA_UI_ELEMENT_BUTTON_REDO);

	ep = t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_SEPARATOR], t3gui_box_proc, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL);
	pa_track_dialog_element(dp, ep, PA_UI_ELEMENT_TOOLBAR_UNDO_SEPARATOR);

	ep = t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_BUTTON], t3gui_push_button_proc, 0, 0, 0, 0, 0, 0, scale, 0, "Pixel", pa_toolbar_cut_button_proc, dp->bitmap[PA_UI_BITMAP_TOOLBAR_CUT]);
	pa_track_dialog_element(dp, ep, PA_UI_ELEMENT_BUTTON_CUT);

	ep = t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_BUTTON], t3gui_push_button_proc, 0, 0, 0, 0, 0, 0, scale, 0, "Pixel", pa_toolbar_copy_button_proc, dp->bitmap[PA_UI_BITMAP_TOOLBAR_COPY]);
	pa_track_dialog_element(dp, ep, PA_UI_ELEMENT_BUTTON_COPY);

	ep = t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_BUTTON], t3gui_push_button_proc, 0, 0, 0, 0, 0, 0, scale, 0, "Pixel", pa_toolbar_paste_button_proc, dp->bitmap[PA_UI_BITMAP_TOOLBAR_PASTE]);
	pa_track_dialog_element(dp, ep, PA_UI_ELEMENT_BUTTON_PASTE);

	ep = t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_SEPARATOR], t3gui_box_proc, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL);
	pa_track_dialog_element(dp, ep, PA_UI_ELEMENT_TOOLBAR_CLIPBOARD_SEPARATOR);

	ep = t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_BUTTON], t3gui_push_button_proc, 0, 0, 0, 0, 0, 0, scale, 0, "Pixel", pa_toolbar_turn_cw_button_proc, dp->bitmap[PA_UI_BITMAP_TOOLBAR_TURN_CW]);
	pa_track_dialog_element(dp, ep, PA_UI_ELEMENT_BUTTON_TURN_CW);

	ep = t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_BUTTON], t3gui_push_button_proc, 0, 0, 0, 0, 0, 0, scale, 0, "Pixel", pa_toolbar_turn_ccw_button_proc, dp->bitmap[PA_UI_BITMAP_TOOLBAR_TURN_CCW]);
	pa_track_dialog_element(dp, ep, PA_UI_ELEMENT_BUTTON_TURN_CCW);

	ep = t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_BUTTON], t3gui_push_button_proc, 0, 0, 0, 0, 0, 0, scale, 0, "Pixel", pa_toolbar_flip_h_button_proc, dp->bitmap[PA_UI_BITMAP_TOOLBAR_FLIP_H]);
	pa_track_dialog_element(dp, ep, PA_UI_ELEMENT_BUTTON_FLIP_HORIZONTAL);

	ep = t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_BUTTON], t3gui_push_button_proc, 0, 0, 0, 0, 0, 0, scale, 0, "Pixel", pa_toolbar_flip_v_button_proc, dp->bitmap[PA_UI_BITMAP_TOOLBAR_FLIP_V]);
	pa_track_dialog_element(dp, ep, PA_UI_ELEMENT_BUTTON_FLIP_VERTICAL);
}

static void add_right_pane(PA_DIALOG * dp, PA_CANVAS_EDITOR * cep)
{
	T3GUI_ELEMENT * ep;
	int scale = pa_get_theme_int(dp->theme, "graphics_scale", 1);

	ep = t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_RIGHT_PANE], t3gui_box_proc, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL);
	pa_track_dialog_element(dp, ep, PA_UI_ELEMENT_RIGHT_PANE);

	ep = t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_LIST_BOX], pa_gui_brush_proc, 0, 0, 0, 0, 0, 0, 0, 0, cep, NULL, NULL);
	pa_track_dialog_element(dp, ep, PA_UI_ELEMENT_BRUSH);

	ep = t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_BUTTON], t3gui_push_button_proc, 0, 0, 0, 0, 0, 0, scale, 0, "Composite", pa_tool_composite_button_proc, dp->bitmap[PA_UI_BITMAP_TOOL_COMPOSITE]);
	pa_track_dialog_element(dp, ep, PA_UI_ELEMENT_BUTTON_TOOL_MODE_COMPOSITE);

	ep = t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_BUTTON], t3gui_push_button_proc, 0, 0, 0, 0, 0, 0, scale, 0, "Outline", pa_tool_outline_button_proc, dp->bitmap[PA_UI_BITMAP_TOOL_OUTLINE]);
	pa_track_dialog_element(dp, ep, PA_UI_ELEMENT_BUTTON_TOOL_MODE_OUTLINE);

	ep = t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_BUTTON], t3gui_push_button_proc, 0, 0, 0, 0, 0, 0, scale, 0, "Pixel", pa_tool_pixel_button_proc, dp->bitmap[PA_UI_BITMAP_TOOL_PIXEL]);
	pa_track_dialog_element(dp, ep, PA_UI_ELEMENT_BUTTON_PIXEL);

	ep = t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_BUTTON], t3gui_push_button_proc, 0, 0, 0, 0, 0, 0, scale, 0, "Line", pa_tool_line_button_proc, dp->bitmap[PA_UI_BITMAP_TOOL_LINE]);
	pa_track_dialog_element(dp, ep, PA_UI_ELEMENT_BUTTON_LINE);

	ep = t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_BUTTON], t3gui_push_button_proc, 0, 0, 0, 0, 0, 0, scale, 0, "Rectangle", pa_tool_rectangle_button_proc, dp->bitmap[PA_UI_BITMAP_TOOL_RECTANGLE]);
	pa_track_dialog_element(dp, ep, PA_UI_ELEMENT_BUTTON_RECTANGLE);

	ep = t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_BUTTON], t3gui_push_button_proc, 0, 0, 0, 0, 0, 0, scale, 0, "FRectangle", pa_tool_filled_rectangle_button_proc, dp->bitmap[PA_UI_BITMAP_TOOL_FILLED_RECTANGLE]);
	pa_track_dialog_element(dp, ep, PA_UI_ELEMENT_BUTTON_FILLED_RECTANGLE);

	ep = t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_BUTTON], t3gui_push_button_proc, 0, 0, 0, 0, 0, 0, scale, 0, "Oval", pa_tool_oval_button_proc, dp->bitmap[PA_UI_BITMAP_TOOL_OVAL]);
	pa_track_dialog_element(dp, ep, PA_UI_ELEMENT_BUTTON_OVAL);

	ep = t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_BUTTON], t3gui_push_button_proc, 0, 0, 0, 0, 0, 0, scale, 0, "FOval", pa_tool_filled_oval_button_proc, dp->bitmap[PA_UI_BITMAP_TOOL_FILLED_OVAL]);
	pa_track_dialog_element(dp, ep, PA_UI_ELEMENT_BUTTON_FILLED_OVAL);

	ep = t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_BUTTON], t3gui_push_button_proc, 0, 0, 0, 0, 0, 0, scale, 0, "Flood", pa_tool_flood_fill_button_proc, dp->bitmap[PA_UI_BITMAP_TOOL_FLOOD]);
	pa_track_dialog_element(dp, ep, PA_UI_ELEMENT_BUTTON_FLOOD_FILL);

	ep = t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_BUTTON], t3gui_push_button_proc, 0, 0, 0, 0, 0, 0, scale, 0, "Replace", pa_tool_replace_button_proc, dp->bitmap[PA_UI_BITMAP_TOOL_REPLACE]);
	pa_track_dialog_element(dp, ep, PA_UI_ELEMENT_BUTTON_REPLACE);

	ep = t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_BUTTON], t3gui_push_button_proc, 0, 0, 0, 0, 0, 0, scale, 0, "Eraser", pa_tool_eraser_button_proc, dp->bitmap[PA_UI_BITMAP_TOOL_ERASER]);
	pa_track_dialog_element(dp, ep, PA_UI_ELEMENT_BUTTON_ERASER);

	ep = t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_BUTTON], t3gui_push_button_proc, 0, 0, 0, 0, 0, 0, scale, 0, "Dropper", pa_tool_dropper_button_proc, dp->bitmap[PA_UI_BITMAP_TOOL_DROPPER]);
	pa_track_dialog_element(dp, ep, PA_UI_ELEMENT_BUTTON_DROPPER);

	ep = t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_BUTTON], t3gui_push_button_proc, 0, 0, 0, 0, 0, 0, scale, 0, "Selection", pa_tool_selection_button_proc, dp->bitmap[PA_UI_BITMAP_TOOL_SELECTION]);
	pa_track_dialog_element(dp, ep, PA_UI_ELEMENT_BUTTON_SELECTION);

	ep = t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_BUTTON], t3gui_push_button_proc, 0, 0, 0, 0, 0, 0, scale, 0, "Frame", pa_tool_frame_button_proc, dp->bitmap[PA_UI_BITMAP_TOOL_FRAME]);
	pa_track_dialog_element(dp, ep, PA_UI_ELEMENT_BUTTON_FRAME);

	ep = t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_LIST_BOX], pa_list_proc, 0, 0, 0, 0, 0, D_SETFOCUS, 0, 0, pa_layer_list_proc, NULL, cep);
	pa_track_dialog_element(dp, ep, PA_UI_ELEMENT_LAYER_LIST);

	ep = t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_BUTTON], t3gui_push_button_proc, 0, 0, 0, 0, 0, 0, scale, 0, "+", pa_layer_add_button_proc, dp->bitmap[PA_UI_BITMAP_LAYER_ADD]);
	pa_track_dialog_element(dp, ep, PA_UI_ELEMENT_BUTTON_ADD_LAYER);

	ep = t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_BUTTON], t3gui_push_button_proc, 0, 0, 0, 0, 0, 0, scale, 0, "-", pa_layer_remove_button_proc, dp->bitmap[PA_UI_BITMAP_LAYER_REMOVE]);
	pa_track_dialog_element(dp, ep, PA_UI_ELEMENT_BUTTON_REMOVE_LAYER);

	ep = t3gui_dialog_add_element(dp->dialog, NULL, pa_gui_map_proc, 0, 0, 0, 0, 0, 0, 0, 0, cep, NULL, NULL);
	pa_track_dialog_element(dp, ep, PA_UI_ELEMENT_MAP);
}

static void add_left_pane(PA_DIALOG * dp, PA_CANVAS_EDITOR * cep)
{
	T3GUI_ELEMENT * ep;

	ep = t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_LEFT_PANE], t3gui_box_proc, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL);
	pa_track_dialog_element(dp, ep, PA_UI_ELEMENT_LEFT_PANE);

	ep = t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_COLOR_SELECT], pa_gui_color_proc, 0, 0, 0, 0, 0, 0, 1, 0, pa_create_gui_color_data(&cep->left_color.color, NULL, NULL, NULL, NULL, NULL, NULL), NULL, NULL);
	pa_track_dialog_element(dp, ep, PA_UI_ELEMENT_LEFT_COLOR);

	ep = t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_COLOR_SELECT], pa_gui_color_proc, 0, 0, 0, 0, 0, 0, 1, 0, pa_create_gui_color_data(&cep->right_color.color, NULL, NULL, NULL, NULL, NULL, NULL), NULL, NULL);
	pa_track_dialog_element(dp, ep, PA_UI_ELEMENT_RIGHT_COLOR);

	cep->left_color.shade_slider_element = t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_SLIDER], t3gui_slider_proc, 0, 0, 0, 0, 0, 0, 1000, 0, NULL, NULL, NULL);
	pa_track_dialog_element(dp, cep->left_color.shade_slider_element, PA_UI_ELEMENT_LEFT_SHADE_SLIDER);

	cep->right_color.shade_slider_element = t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_SLIDER], t3gui_slider_proc, 0, 0, 0, 0, 0, 0, 1000, 0, NULL, NULL, NULL);
	pa_track_dialog_element(dp, cep->right_color.shade_slider_element, PA_UI_ELEMENT_RIGHT_SHADE_SLIDER);

	cep->left_color.alpha_slider_element = t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_SLIDER], t3gui_slider_proc, 0, 0, 0, 0, 0, 0, 1000, 0, NULL, NULL, NULL);
	pa_track_dialog_element(dp, cep->left_color.alpha_slider_element, PA_UI_ELEMENT_LEFT_ALPHA_SLIDER);

	cep->right_color.alpha_slider_element = t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_SLIDER], t3gui_slider_proc, 0, 0, 0, 0, 0, 0, 1000, 0, NULL, NULL, NULL);
	pa_track_dialog_element(dp, cep->right_color.alpha_slider_element, PA_UI_ELEMENT_RIGHT_ALPHA_SLIDER);

	add_color_pickers(dp, cep, 0, 0);

	add_color_palette(dp, cep, 0, 0);

	ep = t3gui_dialog_add_element(dp->dialog, NULL, pa_gui_palette_proc, 0, 0, 0, 0, 0, 0, 0, 0, &cep->left_color.base_color, &cep->right_color.base_color, NULL);
	pa_track_dialog_element(dp, ep, PA_UI_ELEMENT_PALETTE);
}

static void add_status_bar(PA_DIALOG * dp, PA_CANVAS_EDITOR * cep)
{
	T3GUI_ELEMENT * ep;

	ep = t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_BOTTOM_PANE], t3gui_box_proc, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL);
	pa_track_dialog_element(dp, ep, PA_UI_ELEMENT_STATUS_BAR);

	ep = t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_LIST_BOX], t3gui_text_proc, 0, 0, 0, 0, 0, 0, 0, 0, dp->string[PA_UI_STRING_LEFT_MESSAGE], NULL, NULL);
	pa_track_dialog_element(dp, ep, PA_UI_ELEMENT_STATUS_LEFT_MESSAGE);

	ep = t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_LIST_BOX], t3gui_text_proc, 0, 0, 0, 0, 0, 0, 0, 0, dp->string[PA_UI_STRING_MIDDLE_MESSAGE], NULL, NULL);
	pa_track_dialog_element(dp, ep, PA_UI_ELEMENT_STATUS_MIDDLE_MESSAGE);

	ep = t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_LIST_BOX], t3gui_text_proc, 0, 0, 0, 0, 0, 0, 0, 0, dp->string[PA_UI_STRING_RIGHT_MESSAGE], NULL, NULL);
	pa_track_dialog_element(dp, ep, PA_UI_ELEMENT_STATUS_RIGHT_MESSAGE);
}

PA_DIALOG * pa_create_main_dialog(PA_CANVAS_EDITOR * cep)
{
	PA_DIALOG * dp;
	T3GUI_ELEMENT * ep;

	dp = pa_create_dialog(t3f_display, al_get_config_value(t3f_config, "App Data", "theme"), 0, 0, NULL);
	if(!dp)
	{
		goto fail;
	}
	if(!load_resources(dp))
	{
		goto fail;
	}
	cep->palette = pa_create_palette();
	if(!cep->palette)
	{
		goto fail;
	}
	if(!pa_load_palette(cep->palette, "data/default.pxp"))
	{
		goto fail;
	}
	t3gui_dialog_add_element(dp->dialog, NULL, pa_gui_shader_proc, 0, 0, 0, 0, 0, 0, 0, 0, NULL, cep->premultiplied_alpha_shader, NULL);
	add_toolbar(dp, cep);
	add_right_pane(dp, cep);
	add_left_pane(dp, cep);
	add_status_bar(dp, cep);

	cep->editor_element = t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_LIST_BOX], pa_gui_canvas_editor_proc, 0, 0, 0, 0, 0, 0, 0, 0, cep, NULL, NULL);
	pa_track_dialog_element(dp, cep->editor_element, PA_UI_ELEMENT_CANVAS_EDITOR);
	ep = t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_COLOR_SELECT], pa_gui_color_proc, 0, 0, 0, 0, 0, D_DISABLED, 0, 0, NULL, NULL, NULL);
	pa_track_dialog_element(dp, ep, PA_UI_ELEMENT_FLOATING_COLOR);
	cep->peg_bitmap = dp->bitmap[PA_UI_BITMAP_BOX_HANDLE];
	cep->singlelayer_bitmap = dp->bitmap[PA_UI_BITMAP_SELECTION_SINGLE];
	cep->multilayer_bitmap = dp->bitmap[PA_UI_BITMAP_SELECTION_MULTI];
	return dp;

	fail:
	{
		if(dp)
		{
			free(dp);
		}
	}
	return NULL;
}

static void resize_element(T3GUI_ELEMENT * ep, int x, int y, int w, int h)
{
	ep->x = x;
	ep->y = y;
	ep->w = w;
	ep->h = h;
}

static void resize_color_picker(PA_DIALOG * dp, int start, int size, int x, int y, int vx, int vy)
{
	int i, pos_x = x, pos_y = y;

	for(i = 0; i < PA_COLOR_PICKER_SHADES; i++)
	{
		resize_element(&dp->dialog->element[start + i], pos_x, pos_y, size, size);
		pos_x += vx;
		pos_y += vy;
	}
}

static void resize_color_palette(PA_DIALOG * dp, int x, int y)
{
	int i, j;
	int color_size = pa_get_theme_int(dp->theme, "color_size", 12);

	for(i = 0; i < PA_COLOR_PICKER_SHADES; i++)
	{
		for(j = 0; j < PA_COLOR_PICKER_SHADES; j++)
		{
			resize_element(&dp->dialog->element[dp->variable[PA_UI_VAR_PALETTE_COLOR_INDEX] + i * PA_COLOR_PICKER_SHADES + j], x + j * color_size, y + i * color_size, color_size, color_size);
		}
	}
}

void pa_resize_main_dialog(PA_DIALOG * dp)
{
	PA_CANVAS_EDITOR * canvas_editor = NULL;
	T3GUI_THEME * default_theme;
	int left_pane_width;
	int right_pane_width;
	int esl, esr, est, esb;
	int ml, mr, mt, mb;
	int pos_x;
	int offset_x = 0;
	int pos_vx;
	int pos_y;
	int pos_vy;
	int s;
	int separator_space;
	int separator_offset;
	int separator_size;
	int status_height;
	int status_offset;
	int button_size;
	int color_size = pa_get_theme_int(dp->theme, "color_size", 12);

	default_theme = t3gui_get_default_theme();
	if(!default_theme)
	{
		return;
	}
	separator_space = pa_get_theme_int(dp->theme, "separator_space", 8);
	separator_offset = pa_get_theme_int(dp->theme, "separator_offset", 8);
	separator_size = pa_get_theme_int(dp->theme, "separator_size", 2);
	esl = pa_get_theme_int(dp->theme, "edge_left_space", 8);
	esr = pa_get_theme_int(dp->theme, "edge_right_space", 8);
	est = pa_get_theme_int(dp->theme, "edge_top_space", 8);
	esb = pa_get_theme_int(dp->theme, "edge_bottom_space", 8);
	ml = pa_get_theme_int(dp->theme, "element_left_margin", 0);
	mr = pa_get_theme_int(dp->theme, "element_right_margin", 0);
	mt = pa_get_theme_int(dp->theme, "element_top_margin", 0);
	mb = pa_get_theme_int(dp->theme, "element_bottom_margin", 0);
	status_height = al_get_font_line_height(dp->theme->theme[PA_UI_THEME_LIST_BOX]->state[0].font[0]) + est + esb;
	status_offset = pa_get_theme_int(dp->theme, "status_offset", 8);
	button_size = pa_get_theme_int(dp->theme, "button_size", 64);
	right_pane_width = button_size + button_size + mr + ml + mr + ml + mr + ml + esr + esl;
	pos_vx = button_size;
	pos_y = 0;
	pos_vy = button_size;

	pos_x = mr + ml + esl;
	pos_y = mb + mt + est;
	resize_element(pa_get_dialog_element(dp, PA_UI_ELEMENT_TOOLBAR), 0, 0, t3f_default_view->width, pos_vy + mt + mb + mt + mb + est + esb);
	resize_element(pa_get_dialog_element(dp, PA_UI_ELEMENT_BUTTON_NEW), pos_x, pos_y, pos_vx, pos_vy);
	pos_x += pos_vx + mr + ml;
	resize_element(pa_get_dialog_element(dp, PA_UI_ELEMENT_BUTTON_OPEN), pos_x, pos_y, pos_vx, pos_vy);
	pos_x += pos_vx + mr + ml;
	resize_element(pa_get_dialog_element(dp, PA_UI_ELEMENT_BUTTON_SAVE), pos_x, pos_y, pos_vx, pos_vy);
	pos_x += pos_vx + mr + ml;
	resize_element(pa_get_dialog_element(dp, PA_UI_ELEMENT_BUTTON_IMPORT), pos_x, pos_y, pos_vx, pos_vy);
	pos_x += pos_vx + mr + ml;
	resize_element(pa_get_dialog_element(dp, PA_UI_ELEMENT_BUTTON_EXPORT), pos_x, pos_y, pos_vx, pos_vy);
	pos_x += pos_vx + separator_space;
	resize_element(pa_get_dialog_element(dp, PA_UI_ELEMENT_TOOLBAR_FILE_SEPARATOR), pos_x, pos_y + separator_offset, separator_size, pos_vy - separator_offset * 2);
	pos_x += separator_space;
	resize_element(pa_get_dialog_element(dp, PA_UI_ELEMENT_BUTTON_UNDO), pos_x, pos_y, pos_vx, pos_vy);
	pos_x += pos_vx + mr + ml;
	resize_element(pa_get_dialog_element(dp, PA_UI_ELEMENT_BUTTON_REDO), pos_x, pos_y, pos_vx, pos_vy);
	pos_x += pos_vx + separator_space;
	resize_element(pa_get_dialog_element(dp, PA_UI_ELEMENT_TOOLBAR_UNDO_SEPARATOR), pos_x, pos_y + separator_offset, separator_size, pos_vy - separator_offset * 2);
	pos_x += separator_space;
	resize_element(pa_get_dialog_element(dp, PA_UI_ELEMENT_BUTTON_CUT), pos_x, pos_y, pos_vx, pos_vy);
	pos_x += pos_vx + mr + ml;
	resize_element(pa_get_dialog_element(dp, PA_UI_ELEMENT_BUTTON_COPY), pos_x, pos_y, pos_vx, pos_vy);
	pos_x += pos_vx + mr + ml;
	resize_element(pa_get_dialog_element(dp, PA_UI_ELEMENT_BUTTON_PASTE), pos_x, pos_y, pos_vx, pos_vy);
	pos_x += pos_vx + mr + ml;
	resize_element(pa_get_dialog_element(dp, PA_UI_ELEMENT_BUTTON_TURN_CW), pos_x, pos_y, pos_vx, pos_vy);
	pos_x += pos_vx + mr + ml;
	resize_element(pa_get_dialog_element(dp, PA_UI_ELEMENT_BUTTON_TURN_CCW), pos_x, pos_y, pos_vx, pos_vy);
	pos_x += pos_vx + separator_space;
	resize_element(pa_get_dialog_element(dp, PA_UI_ELEMENT_TOOLBAR_CLIPBOARD_SEPARATOR), pos_x, pos_y + separator_offset, separator_size, pos_vy - separator_offset * 2);
	pos_x += separator_space;
	resize_element(pa_get_dialog_element(dp, PA_UI_ELEMENT_BUTTON_FLIP_HORIZONTAL), pos_x, pos_y, pos_vx, pos_vy);
	pos_x += pos_vx + mr + ml;
	resize_element(pa_get_dialog_element(dp, PA_UI_ELEMENT_BUTTON_FLIP_VERTICAL), pos_x, pos_y, pos_vx, pos_vy);
	pos_y += pos_vy + mt + mb + esb;

	pos_x = t3f_default_view->width - right_pane_width;
	resize_element(pa_get_dialog_element(dp, PA_UI_ELEMENT_RIGHT_PANE), pos_x, pos_y, right_pane_width, t3f_default_view->height);
	pos_y += mt + mb + est;

	pos_x = t3f_default_view->width - right_pane_width + ml + mr + esl;
	resize_element(pa_get_dialog_element(dp, PA_UI_ELEMENT_BUTTON_TOOL_MODE_COMPOSITE), pos_x, pos_y, pos_vx, pos_vy);
	pos_x += pos_vx;
	resize_element(pa_get_dialog_element(dp, PA_UI_ELEMENT_BUTTON_TOOL_MODE_OUTLINE), pos_x, pos_y, pos_vx, pos_vy);
	pos_y += pos_vy + mt + mb;
	pos_x = t3f_default_view->width - right_pane_width + ml + mr + esl;
	resize_element(pa_get_dialog_element(dp, PA_UI_ELEMENT_BRUSH), pos_x, pos_y, pos_vx * 2, pos_vy * 2);
	pos_y += pos_vy * 2 + mt + mb + esb;
	resize_element(pa_get_dialog_element(dp, PA_UI_ELEMENT_BUTTON_PIXEL), pos_x + offset_x, pos_y, pos_vx, pos_vy);
	offset_x += pos_vx + mr + ml;
	if(offset_x > pos_vx + mr + ml + mr + ml)
	{
		offset_x = mr + ml;
		pos_y += pos_vy + mt + mb;
	}
	resize_element(pa_get_dialog_element(dp, PA_UI_ELEMENT_BUTTON_LINE), pos_x + offset_x, pos_y, pos_vx, pos_vy);
	offset_x += pos_vx + mr + ml;
	if(offset_x > pos_vx + mr + ml + mr + ml)
	{
		offset_x = mr + ml;
		pos_y += pos_vy + mt + mb;
	}
	resize_element(pa_get_dialog_element(dp, PA_UI_ELEMENT_BUTTON_RECTANGLE), pos_x + offset_x, pos_y, pos_vx, pos_vy);
	offset_x += pos_vx + mr + ml;
	if(offset_x > pos_vx + mr + ml + mr + ml)
	{
		offset_x =  mr + ml;
		pos_y += pos_vy + mt + mb;
	}
	resize_element(pa_get_dialog_element(dp, PA_UI_ELEMENT_BUTTON_FILLED_RECTANGLE), pos_x + offset_x, pos_y, pos_vx, pos_vy);
	offset_x += pos_vx + mr + ml;
	if(offset_x > pos_vx + mr + ml + mr + ml)
	{
		offset_x =  mr + ml;
		pos_y += pos_vy + mt + mb;
	}
	resize_element(pa_get_dialog_element(dp, PA_UI_ELEMENT_BUTTON_OVAL), pos_x + offset_x, pos_y, pos_vx, pos_vy);
	offset_x += pos_vx + mr + ml;
	if(offset_x > pos_vx + mr + ml + mr + ml)
	{
		offset_x =  mr + ml;
		pos_y += pos_vy + mt + mb;
	}
	resize_element(pa_get_dialog_element(dp, PA_UI_ELEMENT_BUTTON_FILLED_OVAL), pos_x + offset_x, pos_y, pos_vx, pos_vy);
	offset_x += pos_vx + mr + ml;
	if(offset_x > pos_vx + mr + ml + mr + ml)
	{
		offset_x =  mr + ml;
		pos_y += pos_vy + mt + mb;
	}
	resize_element(pa_get_dialog_element(dp, PA_UI_ELEMENT_BUTTON_FLOOD_FILL), pos_x + offset_x, pos_y, pos_vx, pos_vy);
	offset_x += pos_vx + mr + ml;
	if(offset_x > pos_vx + mr + ml + mr + ml)
	{
		offset_x = mr + ml;
		pos_y += pos_vy + mt + mb;
	}
	resize_element(pa_get_dialog_element(dp, PA_UI_ELEMENT_BUTTON_REPLACE), pos_x + offset_x, pos_y, pos_vx, pos_vy);
	offset_x += pos_vx + mr + ml;
	if(offset_x > pos_vx + mr + ml + mr + ml)
	{
		offset_x = mr + ml;
		pos_y += pos_vy + mt + mb;
	}
	resize_element(pa_get_dialog_element(dp, PA_UI_ELEMENT_BUTTON_ERASER), pos_x + offset_x, pos_y, pos_vx, pos_vy);
	offset_x += pos_vx + mr + ml;
	if(offset_x > pos_vx + mr + ml + mr + ml)
	{
		offset_x =  mr + ml;
		pos_y += pos_vy + mt + mb;
	}
	resize_element(pa_get_dialog_element(dp, PA_UI_ELEMENT_BUTTON_DROPPER), pos_x + offset_x, pos_y, pos_vx, pos_vy);
	offset_x += pos_vx + mr + ml;
	if(offset_x > pos_vx + mr + ml + mr + ml)
	{
		offset_x =  mr + ml;
		pos_y += pos_vy + mt + mb;
	}
	resize_element(pa_get_dialog_element(dp, PA_UI_ELEMENT_BUTTON_SELECTION), pos_x + offset_x, pos_y, pos_vx, pos_vy);
	offset_x += pos_vx + mr + ml;
	if(offset_x > pos_vx + mr + ml + mr + ml)
	{
		offset_x = mr + ml;
		pos_y += pos_vy + mt + mb;
	}
	resize_element(pa_get_dialog_element(dp, PA_UI_ELEMENT_BUTTON_FRAME), pos_x + offset_x, pos_y, pos_vx, pos_vy);
	pos_y += pos_vy + mt + mb + esb;
	s = (al_get_font_line_height(dp->theme->theme[PA_UI_THEME_LIST_BOX]->state[0].font[0]) + dp->theme->theme[PA_UI_THEME_LIST_BOX]->state[0].top_margin + dp->theme->theme[PA_UI_THEME_LIST_BOX]->state[0].bottom_margin) * 4;
	resize_element(pa_get_dialog_element(dp, PA_UI_ELEMENT_LAYER_LIST), t3f_default_view->width - right_pane_width + esl + ml, pos_y, right_pane_width - mr - ml - mr - ml - esl - esr, s);
	pos_y += s;
	resize_element(pa_get_dialog_element(dp, PA_UI_ELEMENT_BUTTON_ADD_LAYER), t3f_default_view->width - right_pane_width + ml + mr + esl, pos_y, button_size, button_size);
	resize_element(pa_get_dialog_element(dp, PA_UI_ELEMENT_BUTTON_REMOVE_LAYER), t3f_default_view->width - ml - mr - esr - pos_vx, pos_y, button_size, button_size);
	pos_y += button_size + mt + mb + est + esb;
	resize_element(pa_get_dialog_element(dp, PA_UI_ELEMENT_MAP), t3f_default_view->width - right_pane_width + esl, pos_y, right_pane_width - esl - esr, 128);

	left_pane_width = PA_COLOR_PICKER_SHADES * color_size + esl + esr;
	pos_x = esl;
	pos_y = pos_vy + mt + mb + mt + mb + est + esb;
	s = left_pane_width / 2 - esl - esl / 2;
	resize_element(pa_get_dialog_element(dp, PA_UI_ELEMENT_LEFT_PANE), 0, pos_y, left_pane_width, t3f_default_view->height);
	pos_y += esb;
	resize_element(pa_get_dialog_element(dp, PA_UI_ELEMENT_LEFT_COLOR), pos_x, pos_y, s, s);
	pos_x += s + esl;
	resize_element(pa_get_dialog_element(dp, PA_UI_ELEMENT_RIGHT_COLOR), pos_x, pos_y, s, s);
	pos_x = esl;
	pos_y += s;
	resize_element(pa_get_dialog_element(dp, PA_UI_ELEMENT_LEFT_SHADE_SLIDER), pos_x, pos_y, s, dp->theme->theme[PA_UI_THEME_SLIDER]->state[0].size);
	pos_x += s + esl;
	resize_element(pa_get_dialog_element(dp, PA_UI_ELEMENT_RIGHT_SHADE_SLIDER), pos_x, pos_y, s, dp->theme->theme[PA_UI_THEME_SLIDER]->state[0].size);
	pos_x = esl;
	pos_y += dp->theme->theme[PA_UI_THEME_SLIDER]->state[0].size;
	resize_element(pa_get_dialog_element(dp, PA_UI_ELEMENT_LEFT_ALPHA_SLIDER), pos_x, pos_y, s, dp->theme->theme[PA_UI_THEME_SLIDER]->state[0].size);
	pos_x += s + esl;
	resize_element(pa_get_dialog_element(dp, PA_UI_ELEMENT_RIGHT_ALPHA_SLIDER), pos_x, pos_y, s, dp->theme->theme[PA_UI_THEME_SLIDER]->state[0].size);
	pos_x = esl;
	pos_y += color_size + esb;
	resize_color_picker(dp, dp->variable[PA_UI_VAR_LEFT_SHADE_PICKER_INDEX], color_size, pos_x, pos_y, color_size, 0);
	pos_y += color_size;
	resize_color_picker(dp, dp->variable[PA_UI_VAR_LEFT_ALPHA_PICKER_INDEX], color_size, pos_x, pos_y, color_size, 0);
	pos_y += color_size;
	resize_color_picker(dp, dp->variable[PA_UI_VAR_RIGHT_SHADE_PICKER_INDEX], color_size, pos_x, pos_y, color_size, 0);
	pos_y += color_size;
	resize_color_picker(dp, dp->variable[PA_UI_VAR_RIGHT_ALPHA_PICKER_INDEX], color_size, pos_x, pos_y, color_size, 0);
	pos_y += color_size + esb;

	pos_x = esl;
	resize_color_palette(dp, esl, pos_y);
	pos_y += color_size * 9;
	pos_y += esb;

	resize_element(pa_get_dialog_element(dp, PA_UI_ELEMENT_PALETTE), esl, pos_y, left_pane_width - esl - esr, t3f_default_view->height - pos_y - esb - status_height);
	pa_resize_palette(pa_get_dialog_element(dp, PA_UI_ELEMENT_PALETTE)->w, pa_get_dialog_element(dp, PA_UI_ELEMENT_PALETTE)->h);

	pos_y = t3f_default_view->height - status_height;
	resize_element(pa_get_dialog_element(dp, PA_UI_ELEMENT_STATUS_BAR), 0, pos_y, t3f_default_view->width, status_height);
	pos_y += status_offset;
	resize_element(pa_get_dialog_element(dp, PA_UI_ELEMENT_STATUS_LEFT_MESSAGE), esl, pos_y, t3f_default_view->width - esl - esr, status_height - status_offset);
	resize_element(pa_get_dialog_element(dp, PA_UI_ELEMENT_STATUS_MIDDLE_MESSAGE), esl, pos_y, t3f_default_view->width - esl - esr, status_height - status_offset);
	resize_element(pa_get_dialog_element(dp, PA_UI_ELEMENT_STATUS_RIGHT_MESSAGE), esl, pos_y, t3f_default_view->width - esl - esr, status_height - status_offset);

	resize_element(pa_get_dialog_element(dp, PA_UI_ELEMENT_CANVAS_EDITOR), left_pane_width, pos_vy + mt + mb + mt + mb + est + esb, t3f_default_view->width - left_pane_width - right_pane_width, t3f_default_view->height - pos_vy - mt - mb - mt - mb - est - esb - status_height);

	canvas_editor = (PA_CANVAS_EDITOR *)pa_get_dialog_element(dp, PA_UI_ELEMENT_CANVAS_EDITOR)->dp;
	t3f_adjust_view(canvas_editor->view, 0, 0, al_get_display_width(t3f_display), al_get_display_height(t3f_display), pa_get_dialog_element(dp, PA_UI_ELEMENT_CANVAS_EDITOR)->w / 2, pa_get_dialog_element(dp, PA_UI_ELEMENT_CANVAS_EDITOR)->h / 2, T3F_NO_SCALE);
	pa_set_canvas_editor_zoom(canvas_editor, canvas_editor->view_zoom);
	canvas_editor->frame_text_offset = est + esb;
}
