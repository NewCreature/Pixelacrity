#include "t3f/t3f.h"
#include "t3gui/t3gui.h"
#include "ui.h"
#include "window.h"
#include "menu/menu.h"
#include "menu/file_proc.h"
#include "menu/edit_proc.h"
#include "gui/button_proc.h"
#include "modules/mouse_cursor.h"
#include "modules/canvas/canvas.h"
#include "modules/canvas/canvas_file.h"
#include "modules/canvas/canvas_helpers.h"
#include "gui/color.h"
#include "gui/palette.h"
#include "gui/map.h"
#include "gui/canvas_editor/element.h"
#include "modules/ega_palette.h"
#include "gui/list_proc.h"
#include "gui/layer_list_proc.h"
#include "ui/canvas_editor/undo/undo.h"

static ALLEGRO_BITMAP * make_checkerboard_bitmap(ALLEGRO_COLOR c1, ALLEGRO_COLOR c2)
{
	ALLEGRO_STATE old_state;
	ALLEGRO_TRANSFORM identity;
	ALLEGRO_BITMAP * bp;

	al_store_state(&old_state, ALLEGRO_STATE_NEW_BITMAP_PARAMETERS | ALLEGRO_STATE_TARGET_BITMAP | ALLEGRO_STATE_TRANSFORM);
	al_set_new_bitmap_flags(0);
	bp = al_create_bitmap(2, 2);
	if(bp)
	{
		al_set_target_bitmap(bp);
		al_identity_transform(&identity);
		al_use_transform(&identity);
		al_lock_bitmap(bp, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_WRITEONLY);
		al_put_pixel(0, 0, c1);
		al_put_pixel(0, 1, c2);
		al_put_pixel(1, 0, c2);
		al_put_pixel(1, 1, c1);
		al_unlock_bitmap(bp);
	}
	al_restore_state(&old_state);
	return bp;
}

static bool add_color_pickers(PA_UI * uip, PA_CANVAS_EDITOR * cep, T3GUI_DIALOG * dp, int x, int y)
{
	int i;

	for(i = 0; i < PA_COLOR_PICKER_SHADES; i++)
	{
		uip->left_shade_picker_element[i] = t3gui_dialog_add_element(dp, NULL, pa_gui_color_proc, 0, 0, 0, 0, 0, 0, 0, 0, &cep->left_shade_color[i], &cep->left_color.color, &cep->right_color.color);
	}
	for(i = 0; i < PA_COLOR_PICKER_SHADES; i++)
	{
		uip->left_alpha_picker_element[i] = t3gui_dialog_add_element(dp, NULL, pa_gui_color_proc, 0, 0, 0, 0, 0, 0, 0, 0, &cep->left_alpha_color[i], &cep->left_color.color, &cep->right_color.color);
	}
	for(i = 0; i < PA_COLOR_PICKER_SHADES; i++)
	{
		uip->right_shade_picker_element[i] = t3gui_dialog_add_element(dp, NULL, pa_gui_color_proc, 0, 0, 0, 0, 0, 0, 0, 0, &cep->right_shade_color[i], &cep->left_color.color, &cep->right_color.color);
	}
	for(i = 0; i < PA_COLOR_PICKER_SHADES; i++)
	{
		uip->right_alpha_picker_element[i] = t3gui_dialog_add_element(dp, NULL, pa_gui_color_proc, 0, 0, 0, 0, 0, 0, 0, 0, &cep->right_alpha_color[i], &cep->left_color.color, &cep->right_color.color);
	}

	return true;
}

static bool add_color_palette(PA_UI * uip, PA_CANVAS_EDITOR * cep, T3GUI_DIALOG * dp, int x, int y)
{
	int i, j;

	for(i = 0; i < PA_COLOR_PICKER_SHADES; i++)
	{
		for(j = 0; j < PA_COLOR_PICKER_SHADES; j++)
		{
			uip->palette_color_element[i * PA_COLOR_PICKER_SHADES + j] = t3gui_dialog_add_element(dp, NULL, pa_gui_color_proc, 0, 0, 0, 0, 0, 0, 0, 0, &cep->palette[i * PA_COLOR_PICKER_SHADES + j], &cep->left_color.base_color, &cep->right_color.base_color);
			uip->palette_color_element[i * PA_COLOR_PICKER_SHADES + j]->dp4 = &cep->left_color.color;
			uip->palette_color_element[i * PA_COLOR_PICKER_SHADES + j]->dp5 = &cep->right_color.color;
		}
	}

	return true;
}

static void resize_element(T3GUI_ELEMENT * ep, int x, int y, int w, int h)
{
	ep->x = x;
	ep->y = y;
	ep->w = w;
	ep->h = h;
}

static void resize_color_picker(PA_UI * uip, T3GUI_ELEMENT ** element, int size, int x, int y, int vx, int vy)
{
	int i, pos_x = x, pos_y = y;

	for(i = 0; i < PA_COLOR_PICKER_SHADES; i++)
	{
		resize_element(element[i], pos_x, pos_y, size, size);
		pos_x += vx;
		pos_y += vy;
	}
}

static void resize_color_palette(PA_UI * uip, int x, int y)
{
	int i, j;
	int color_size = pa_get_theme_int(uip->theme, "color_size", 12);

	for(i = 0; i < PA_COLOR_PICKER_SHADES; i++)
	{
		for(j = 0; j < PA_COLOR_PICKER_SHADES; j++)
		{
			resize_element(uip->palette_color_element[i * PA_COLOR_PICKER_SHADES + j], x + j * color_size, y + i * color_size, color_size, color_size);
		}
	}
}

void pa_resize_ui(PA_UI * uip)
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
	int status_height;
	int button_size;
	int color_size = pa_get_theme_int(uip->theme, "color_size", 12);

	default_theme = t3gui_get_default_theme();
	if(!default_theme)
	{
		return;
	}
	status_height = al_get_font_line_height(default_theme->state[0].font[0]) + PA_UI_ELEMENT_SPACE * 2;
	esl = pa_get_theme_int(uip->theme, "edge_left_space", 8);
	esr = pa_get_theme_int(uip->theme, "edge_right_space", 8);
	est = pa_get_theme_int(uip->theme, "edge_top_space", 8);
	esb = pa_get_theme_int(uip->theme, "edge_bottom_space", 8);
	ml = pa_get_theme_int(uip->theme, "element_left_margin", 0);
	mr = pa_get_theme_int(uip->theme, "element_right_margin", 0);
	mt = pa_get_theme_int(uip->theme, "element_top_margin", 0);
	mb = pa_get_theme_int(uip->theme, "element_bottom_margin", 0);
	button_size = pa_get_theme_int(uip->theme, "button_size", 64);
	right_pane_width = button_size + button_size + mr + ml + mr + ml + mr + ml + esr + esl;
	pos_vx = button_size;
	pos_y = 0;
	pos_vy = button_size;

	pos_x = mr + ml + esl;
	pos_y = mb + mt + est;
	resize_element(uip->element[PA_UI_ELEMENT_TOOLBAR], 0, 0, t3f_default_view->width, pos_vy + mt + mb + mt + mb + est + esb);
	resize_element(uip->element[PA_UI_ELEMENT_BUTTON_NEW], pos_x, pos_y, pos_vx, pos_vy);
	pos_x += pos_vx + mr + ml;
	resize_element(uip->element[PA_UI_ELEMENT_BUTTON_OPEN], pos_x, pos_y, pos_vx, pos_vy);
	pos_x += pos_vx + mr + ml;
	resize_element(uip->element[PA_UI_ELEMENT_BUTTON_SAVE], pos_x, pos_y, pos_vx, pos_vy);
	pos_x += pos_vx + mr + ml;
	resize_element(uip->element[PA_UI_ELEMENT_BUTTON_IMPORT], pos_x, pos_y, pos_vx, pos_vy);
	pos_x += pos_vx + mr + ml;
	resize_element(uip->element[PA_UI_ELEMENT_BUTTON_EXPORT], pos_x, pos_y, pos_vx, pos_vy);
	pos_x += pos_vx + mr + ml;
	resize_element(uip->element[PA_UI_ELEMENT_BUTTON_UNDO], pos_x, pos_y, pos_vx, pos_vy);
	pos_x += pos_vx + mr + ml;
	resize_element(uip->element[PA_UI_ELEMENT_BUTTON_REDO], pos_x, pos_y, pos_vx, pos_vy);
	pos_x += pos_vx + mr + ml;
	resize_element(uip->element[PA_UI_ELEMENT_BUTTON_CUT], pos_x, pos_y, pos_vx, pos_vy);
	pos_x += pos_vx + mr + ml;
	resize_element(uip->element[PA_UI_ELEMENT_BUTTON_COPY], pos_x, pos_y, pos_vx, pos_vy);
	pos_x += pos_vx + mr + ml;
	resize_element(uip->element[PA_UI_ELEMENT_BUTTON_PASTE], pos_x, pos_y, pos_vx, pos_vy);
	pos_x += pos_vx + mr + ml;
	resize_element(uip->element[PA_UI_ELEMENT_BUTTON_TURN_CW], pos_x, pos_y, pos_vx, pos_vy);
	pos_x += pos_vx + mr + ml;
	resize_element(uip->element[PA_UI_ELEMENT_BUTTON_TURN_CCW], pos_x, pos_y, pos_vx, pos_vy);
	pos_x += pos_vx + mr + ml;
	resize_element(uip->element[PA_UI_ELEMENT_BUTTON_FLIP_HORIZONTAL], pos_x, pos_y, pos_vx, pos_vy);
	pos_x += pos_vx + mr + ml;
	resize_element(uip->element[PA_UI_ELEMENT_BUTTON_FLIP_VERTICAL], pos_x, pos_y, pos_vx, pos_vy);
	pos_y += pos_vy + mt + mb + esb;

	pos_x = t3f_default_view->width - right_pane_width;
	resize_element(uip->element[PA_UI_ELEMENT_RIGHT_PANE], pos_x, pos_y, right_pane_width, t3f_default_view->height);
	pos_y += mt + mb + est;

	pos_x = t3f_default_view->width - right_pane_width + ml + mr + esl;
	resize_element(uip->element[PA_UI_ELEMENT_BUTTON_PIXEL], pos_x + offset_x, pos_y, pos_vx, pos_vy);
	offset_x += pos_vx + mr + ml;
	if(offset_x > pos_vx + mr + ml + mr + ml)
	{
		offset_x = mr + ml;
		pos_y += pos_vy + mt + mb;
	}
	resize_element(uip->element[PA_UI_ELEMENT_BUTTON_LINE], pos_x + offset_x, pos_y, pos_vx, pos_vy);
	offset_x += pos_vx + mr + ml;
	if(offset_x > pos_vx + mr + ml + mr + ml)
	{
		offset_x = mr + ml;
		pos_y += pos_vy + mt + mb;
	}
	resize_element(uip->element[PA_UI_ELEMENT_BUTTON_RECTANGLE], pos_x + offset_x, pos_y, pos_vx, pos_vy);
	offset_x += pos_vx + mr + ml;
	if(offset_x > pos_vx + mr + ml + mr + ml)
	{
		offset_x =  mr + ml;
		pos_y += pos_vy + mt + mb;
	}
	resize_element(uip->element[PA_UI_ELEMENT_BUTTON_FILLED_RECTANGLE], pos_x + offset_x, pos_y, pos_vx, pos_vy);
	offset_x += pos_vx + mr + ml;
	if(offset_x > pos_vx + mr + ml + mr + ml)
	{
		offset_x =  mr + ml;
		pos_y += pos_vy + mt + mb;
	}
	resize_element(uip->element[PA_UI_ELEMENT_BUTTON_OVAL], pos_x + offset_x, pos_y, pos_vx, pos_vy);
	offset_x += pos_vx + mr + ml;
	if(offset_x > pos_vx + mr + ml + mr + ml)
	{
		offset_x =  mr + ml;
		pos_y += pos_vy + mt + mb;
	}
	resize_element(uip->element[PA_UI_ELEMENT_BUTTON_FILLED_OVAL], pos_x + offset_x, pos_y, pos_vx, pos_vy);
	offset_x += pos_vx + mr + ml;
	if(offset_x > pos_vx + mr + ml + mr + ml)
	{
		offset_x =  mr + ml;
		pos_y += pos_vy + mt + mb;
	}
	resize_element(uip->element[PA_UI_ELEMENT_BUTTON_FLOOD_FILL], pos_x + offset_x, pos_y, pos_vx, pos_vy);
	offset_x += pos_vx + mr + ml;
	if(offset_x > pos_vx + mr + ml + mr + ml)
	{
		offset_x = mr + ml;
		pos_y += pos_vy + mt + mb;
	}
	resize_element(uip->element[PA_UI_ELEMENT_BUTTON_ERASER], pos_x + offset_x, pos_y, pos_vx, pos_vy);
	offset_x += pos_vx + mr + ml;
	if(offset_x > pos_vx + mr + ml + mr + ml)
	{
		offset_x =  mr + ml;
		pos_y += pos_vy + mt + mb;
	}
	resize_element(uip->element[PA_UI_ELEMENT_BUTTON_DROPPER], pos_x + offset_x, pos_y, pos_vx, pos_vy);
	offset_x += pos_vx + mr + ml;
	if(offset_x > pos_vx + mr + ml + mr + ml)
	{
		offset_x =  mr + ml;
		pos_y += pos_vy + mt + mb;
	}
	resize_element(uip->element[PA_UI_ELEMENT_BUTTON_SELECTION], pos_x + offset_x, pos_y, pos_vx, pos_vy);
	offset_x += pos_vx + mr + ml;
	if(offset_x > pos_vx + mr + ml + mr + ml)
	{
		offset_x = mr + ml;
		pos_y += pos_vy + mt + mb;
	}
	resize_element(uip->element[PA_UI_ELEMENT_BUTTON_FRAME], pos_x + offset_x, pos_y, pos_vx, pos_vy);
	pos_y += pos_vy + mt + mb + esb;
	resize_element(uip->element[PA_UI_ELEMENT_LAYER_LIST], t3f_default_view->width - right_pane_width + esl + ml, pos_y, right_pane_width - mr - ml - mr - ml - esl - esr, 128);
	pos_y += 128;
	resize_element(uip->element[PA_UI_ELEMENT_BUTTON_ADD_LAYER], t3f_default_view->width - right_pane_width + ml + mr + esl, pos_y, button_size, button_size);
	resize_element(uip->element[PA_UI_ELEMENT_BUTTON_REMOVE_LAYER], t3f_default_view->width - ml - mr - esr - pos_vx, pos_y, button_size, button_size);
	pos_y += button_size + mt + mb + est + esb;
	resize_element(uip->element[PA_UI_ELEMENT_MAP], t3f_default_view->width - right_pane_width, pos_y, right_pane_width, 128);

	left_pane_width = PA_COLOR_PICKER_SHADES * color_size + esl + esr;
	pos_x = esl;
	pos_y = pos_vy + mt + mb + mt + mb + est + esb;
	s = left_pane_width / 2 - esl - esl / 2;
	resize_element(uip->element[PA_UI_ELEMENT_LEFT_PANE], 0, pos_y, left_pane_width, t3f_default_view->height);
	pos_y += esb;
	resize_element(uip->element[PA_UI_ELEMENT_LEFT_COLOR], pos_x, pos_y, s, s);
	pos_x += s + esl;
	resize_element(uip->element[PA_UI_ELEMENT_RIGHT_COLOR], pos_x, pos_y, s, s);
	pos_x = esl;
	pos_y += s;
	resize_element(uip->element[PA_UI_ELEMENT_LEFT_SHADE_SLIDER], pos_x, pos_y, s, color_size);
	pos_x += s + esl;
	resize_element(uip->element[PA_UI_ELEMENT_RIGHT_SHADE_SLIDER], pos_x, pos_y, s, color_size);
	pos_x = esl;
	pos_y += color_size;
	resize_element(uip->element[PA_UI_ELEMENT_LEFT_ALPHA_SLIDER], pos_x, pos_y, s, color_size);
	pos_x += s + esl;
	resize_element(uip->element[PA_UI_ELEMENT_RIGHT_ALPHA_SLIDER], pos_x, pos_y, s, color_size);
	pos_x = esl;
	pos_y += color_size + esb;
	resize_color_picker(uip, uip->left_shade_picker_element, color_size, pos_x, pos_y, color_size, 0);
	pos_y += color_size;
	resize_color_picker(uip, uip->left_alpha_picker_element, color_size, pos_x, pos_y, color_size, 0);
	pos_y += color_size;
	resize_color_picker(uip, uip->right_shade_picker_element, color_size, pos_x, pos_y, color_size, 0);
	pos_y += color_size;
	resize_color_picker(uip, uip->right_alpha_picker_element, color_size, pos_x, pos_y, color_size, 0);
	pos_y += color_size + esb;

	pos_x = esl;
	resize_color_palette(uip, esl, pos_y);
	pos_y += color_size * 9;
	pos_y += esb;

	resize_element(uip->element[PA_UI_ELEMENT_PALETTE], esl, pos_y, left_pane_width - esl - esr, t3f_default_view->height - pos_y - esb - status_height);
	pa_resize_palette(uip->element[PA_UI_ELEMENT_PALETTE]->w, uip->element[PA_UI_ELEMENT_PALETTE]->h);

	pos_y = t3f_default_view->height - status_height;
	resize_element(uip->element[PA_UI_ELEMENT_STATUS_BAR], 0, pos_y, t3f_default_view->width, status_height);
	pos_y += PA_UI_ELEMENT_SPACE;
	resize_element(uip->element[PA_UI_ELEMENT_STATUS_LEFT_MESSAGE], PA_UI_ELEMENT_SPACE, pos_y, t3f_default_view->width - PA_UI_ELEMENT_SPACE * 2, status_height);
	resize_element(uip->element[PA_UI_ELEMENT_STATUS_MIDDLE_MESSAGE], PA_UI_ELEMENT_SPACE, pos_y, t3f_default_view->width - PA_UI_ELEMENT_SPACE * 2, status_height);
	resize_element(uip->element[PA_UI_ELEMENT_STATUS_RIGHT_MESSAGE], PA_UI_ELEMENT_SPACE, pos_y, t3f_default_view->width - PA_UI_ELEMENT_SPACE * 2, status_height);

	resize_element(uip->element[PA_UI_ELEMENT_CANVAS_EDITOR], left_pane_width, pos_vy + mt + mb + mt + mb + est + esb, t3f_default_view->width - left_pane_width - right_pane_width, t3f_default_view->height - pos_vy - mt - mb - mt - mb - est - esb - status_height);

	canvas_editor = (PA_CANVAS_EDITOR *)uip->element[PA_UI_ELEMENT_CANVAS_EDITOR]->dp;
	t3f_adjust_view(canvas_editor->view, 0, 0, al_get_display_width(t3f_display), al_get_display_height(t3f_display), uip->element[PA_UI_ELEMENT_CANVAS_EDITOR]->w / 2, uip->element[PA_UI_ELEMENT_CANVAS_EDITOR]->h / 2, T3F_NO_SCALE);
	pa_set_canvas_editor_zoom(canvas_editor, canvas_editor->view_zoom);
}

static bool load_bitmap(PA_UI * uip, int slot, const char * name)
{
	uip->bitmap[slot] = pa_load_theme_bitmap(uip->theme, name);
	if(!uip->bitmap[slot])
	{
		return false;
	}
	return true;
}

static bool load_resources(PA_UI * uip)
{
	uip->theme = pa_load_theme("data/themes/standard/theme.ini");
	if(!uip->theme)
	{
		return false;
	}
	uip->bitmap[PA_UI_BITMAP_BG] = make_checkerboard_bitmap(t3f_color_white, al_map_rgba_f(0.9, 0.9, 0.9, 1.0));
	if(!uip->bitmap[PA_UI_BITMAP_BG])
	{
		return false;
	}

	if(!load_bitmap(uip, PA_UI_BITMAP_TOOL_PIXEL, "tool_pixel"))
	{
		return false;
	}
	if(!load_bitmap(uip, PA_UI_BITMAP_TOOL_LINE, "tool_line"))
	{
		return false;
	}
	if(!load_bitmap(uip, PA_UI_BITMAP_TOOL_RECTANGLE, "tool_rectangle"))
	{
		return false;
	}
	if(!load_bitmap(uip, PA_UI_BITMAP_TOOL_FILLED_RECTANGLE, "tool_filled_rectangle"))
	{
		return false;
	}
	if(!load_bitmap(uip, PA_UI_BITMAP_TOOL_OVAL, "tool_oval"))
	{
		return false;
	}
	if(!load_bitmap(uip, PA_UI_BITMAP_TOOL_FILLED_OVAL, "tool_filled_oval"))
	{
		return false;
	}
	if(!load_bitmap(uip, PA_UI_BITMAP_TOOL_FLOOD, "tool_flood_fill"))
	{
		return false;
	}
	if(!load_bitmap(uip, PA_UI_BITMAP_TOOL_ERASER, "tool_eraser"))
	{
		return false;
	}
	if(!load_bitmap(uip, PA_UI_BITMAP_TOOL_DROPPER, "tool_dropper"))
	{
		return false;
	}
	if(!load_bitmap(uip, PA_UI_BITMAP_TOOL_SELECTION, "tool_selection"))
	{
		return false;
	}
	if(!load_bitmap(uip, PA_UI_BITMAP_TOOL_FRAME, "tool_frame"))
	{
		return false;
	}
	if(!load_bitmap(uip, PA_UI_BITMAP_TOOLBAR_NEW, "toolbar_new"))
	{
		return false;
	}
	if(!load_bitmap(uip, PA_UI_BITMAP_TOOLBAR_OPEN, "toolbar_open"))
	{
		return false;
	}
	if(!load_bitmap(uip, PA_UI_BITMAP_TOOLBAR_SAVE, "toolbar_save"))
	{
		return false;
	}
	if(!load_bitmap(uip, PA_UI_BITMAP_TOOLBAR_IMPORT, "toolbar_import"))
	{
		return false;
	}
	if(!load_bitmap(uip, PA_UI_BITMAP_TOOLBAR_EXPORT, "toolbar_export"))
	{
		return false;
	}
	if(!load_bitmap(uip, PA_UI_BITMAP_TOOLBAR_UNDO, "toolbar_undo"))
	{
		return false;
	}
	if(!load_bitmap(uip, PA_UI_BITMAP_TOOLBAR_REDO, "toolbar_redo"))
	{
		return false;
	}
	if(!load_bitmap(uip, PA_UI_BITMAP_TOOLBAR_CUT, "toolbar_cut"))
	{
		return false;
	}
	if(!load_bitmap(uip, PA_UI_BITMAP_TOOLBAR_COPY, "toolbar_copy"))
	{
		return false;
	}
	if(!load_bitmap(uip, PA_UI_BITMAP_TOOLBAR_PASTE, "toolbar_paste"))
	{
		return false;
	}
	if(!load_bitmap(uip, PA_UI_BITMAP_TOOLBAR_TURN_CW, "toolbar_turn_cw"))
	{
		return false;
	}
	if(!load_bitmap(uip, PA_UI_BITMAP_TOOLBAR_TURN_CCW, "toolbar_turn_ccw"))
	{
		return false;
	}
	if(!load_bitmap(uip, PA_UI_BITMAP_TOOLBAR_FLIP_H, "toolbar_flip_h"))
	{
		return false;
	}
	if(!load_bitmap(uip, PA_UI_BITMAP_TOOLBAR_FLIP_V, "toolbar_flip_v"))
	{
		return false;
	}
	if(!load_bitmap(uip, PA_UI_BITMAP_LAYER_ADD, "layer_add"))
	{
		return false;
	}
	if(!load_bitmap(uip, PA_UI_BITMAP_LAYER_REMOVE, "layer_remove"))
	{
		return false;
	}

	return true;
}

static void set_default_palette(PA_CANVAS_EDITOR * cep)
{
	int i;

	cep->palette[0] = al_map_rgb_f(1.0, 0.0, 0.0);
	cep->palette[1] = al_map_rgb_f(0.0, 1.0, 0.0);
	cep->palette[2] = al_map_rgb_f(0.0, 0.0, 1.0);
	cep->palette[3] = al_map_rgb_f(1.0, 1.0, 0.0);
	cep->palette[4] = al_map_rgb_f(1.0, 0.0, 1.0);
	cep->palette[5] = al_map_rgb_f(0.0, 1.0, 1.0);
	cep->palette[6] = al_map_rgb_f(1.0, 1.0, 1.0);
	cep->palette[7] = al_map_rgb_f(0.0, 0.0, 0.0);
	cep->palette[8] = al_map_rgb_f(0.5, 0.5, 0.5);
	for(i = 0; i < 64; i++)
	{
		cep->palette[9 + i] = pa_get_ega_palette_color(i);
	}
}

static void add_toolbar(PA_UI * uip, PA_CANVAS_EDITOR * cep)
{
	int scale = pa_get_theme_int(uip->theme, "graphics_scale", 1);

	uip->element[PA_UI_ELEMENT_TOOLBAR] = t3gui_dialog_add_element(uip->dialog[PA_UI_DIALOG_MAIN], uip->theme->theme[PA_UI_THEME_BUTTON], t3gui_box_proc, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL);
	uip->element[PA_UI_ELEMENT_BUTTON_NEW] = t3gui_dialog_add_element(uip->dialog[PA_UI_DIALOG_MAIN], uip->theme->theme[PA_UI_THEME_BUTTON], t3gui_push_button_proc, 0, 0, 0, 0, 0, 0, scale, 0, "Pixel", pa_toolbar_new_button_proc, uip->bitmap[PA_UI_BITMAP_TOOLBAR_NEW]);
	uip->element[PA_UI_ELEMENT_BUTTON_OPEN] = t3gui_dialog_add_element(uip->dialog[PA_UI_DIALOG_MAIN], uip->theme->theme[PA_UI_THEME_BUTTON], t3gui_push_button_proc, 0, 0, 0, 0, 0, 0, scale, 0, "Pixel", pa_toolbar_open_button_proc, uip->bitmap[PA_UI_BITMAP_TOOLBAR_OPEN]);
	uip->element[PA_UI_ELEMENT_BUTTON_SAVE] = t3gui_dialog_add_element(uip->dialog[PA_UI_DIALOG_MAIN], uip->theme->theme[PA_UI_THEME_BUTTON], t3gui_push_button_proc, 0, 0, 0, 0, 0, 0, scale, 0, "Pixel", pa_toolbar_save_button_proc, uip->bitmap[PA_UI_BITMAP_TOOLBAR_SAVE]);
	uip->element[PA_UI_ELEMENT_BUTTON_IMPORT] = t3gui_dialog_add_element(uip->dialog[PA_UI_DIALOG_MAIN], uip->theme->theme[PA_UI_THEME_BUTTON], t3gui_push_button_proc, 0, 0, 0, 0, 0, 0, scale, 0, "Pixel", pa_toolbar_import_button_proc, uip->bitmap[PA_UI_BITMAP_TOOLBAR_IMPORT]);
	uip->element[PA_UI_ELEMENT_BUTTON_EXPORT] = t3gui_dialog_add_element(uip->dialog[PA_UI_DIALOG_MAIN], uip->theme->theme[PA_UI_THEME_BUTTON], t3gui_push_button_proc, 0, 0, 0, 0, 0, 0, scale, 0, "Pixel", pa_toolbar_export_button_proc, uip->bitmap[PA_UI_BITMAP_TOOLBAR_EXPORT]);
	uip->element[PA_UI_ELEMENT_BUTTON_UNDO] = t3gui_dialog_add_element(uip->dialog[PA_UI_DIALOG_MAIN], uip->theme->theme[PA_UI_THEME_BUTTON], t3gui_push_button_proc, 0, 0, 0, 0, 0, 0, scale, 0, "Pixel", pa_toolbar_undo_button_proc, uip->bitmap[PA_UI_BITMAP_TOOLBAR_UNDO]);
	uip->element[PA_UI_ELEMENT_BUTTON_REDO] = t3gui_dialog_add_element(uip->dialog[PA_UI_DIALOG_MAIN], uip->theme->theme[PA_UI_THEME_BUTTON], t3gui_push_button_proc, 0, 0, 0, 0, 0, 0, scale, 0, "Pixel", pa_toolbar_redo_button_proc, uip->bitmap[PA_UI_BITMAP_TOOLBAR_REDO]);
	uip->element[PA_UI_ELEMENT_BUTTON_CUT] = t3gui_dialog_add_element(uip->dialog[PA_UI_DIALOG_MAIN], uip->theme->theme[PA_UI_THEME_BUTTON], t3gui_push_button_proc, 0, 0, 0, 0, 0, 0, scale, 0, "Pixel", pa_toolbar_cut_button_proc, uip->bitmap[PA_UI_BITMAP_TOOLBAR_CUT]);
	uip->element[PA_UI_ELEMENT_BUTTON_COPY] = t3gui_dialog_add_element(uip->dialog[PA_UI_DIALOG_MAIN], uip->theme->theme[PA_UI_THEME_BUTTON], t3gui_push_button_proc, 0, 0, 0, 0, 0, 0, scale, 0, "Pixel", pa_toolbar_copy_button_proc, uip->bitmap[PA_UI_BITMAP_TOOLBAR_COPY]);
	uip->element[PA_UI_ELEMENT_BUTTON_PASTE] = t3gui_dialog_add_element(uip->dialog[PA_UI_DIALOG_MAIN], uip->theme->theme[PA_UI_THEME_BUTTON], t3gui_push_button_proc, 0, 0, 0, 0, 0, 0, scale, 0, "Pixel", pa_toolbar_paste_button_proc, uip->bitmap[PA_UI_BITMAP_TOOLBAR_PASTE]);
	uip->element[PA_UI_ELEMENT_BUTTON_TURN_CW] = t3gui_dialog_add_element(uip->dialog[PA_UI_DIALOG_MAIN], uip->theme->theme[PA_UI_THEME_BUTTON], t3gui_push_button_proc, 0, 0, 0, 0, 0, 0, scale, 0, "Pixel", pa_toolbar_turn_cw_button_proc, uip->bitmap[PA_UI_BITMAP_TOOLBAR_TURN_CW]);
	uip->element[PA_UI_ELEMENT_BUTTON_TURN_CCW] = t3gui_dialog_add_element(uip->dialog[PA_UI_DIALOG_MAIN], uip->theme->theme[PA_UI_THEME_BUTTON], t3gui_push_button_proc, 0, 0, 0, 0, 0, 0, scale, 0, "Pixel", pa_toolbar_turn_ccw_button_proc, uip->bitmap[PA_UI_BITMAP_TOOLBAR_TURN_CCW]);
	uip->element[PA_UI_ELEMENT_BUTTON_FLIP_HORIZONTAL] = t3gui_dialog_add_element(uip->dialog[PA_UI_DIALOG_MAIN], uip->theme->theme[PA_UI_THEME_BUTTON], t3gui_push_button_proc, 0, 0, 0, 0, 0, 0, scale, 0, "Pixel", pa_toolbar_flip_h_button_proc, uip->bitmap[PA_UI_BITMAP_TOOLBAR_FLIP_H]);
	uip->element[PA_UI_ELEMENT_BUTTON_FLIP_VERTICAL] = t3gui_dialog_add_element(uip->dialog[PA_UI_DIALOG_MAIN], uip->theme->theme[PA_UI_THEME_BUTTON], t3gui_push_button_proc, 0, 0, 0, 0, 0, 0, scale, 0, "Pixel", pa_toolbar_flip_v_button_proc, uip->bitmap[PA_UI_BITMAP_TOOLBAR_FLIP_V]);
}

static void add_right_pane(PA_UI * uip, PA_CANVAS_EDITOR * cep)
{
	int scale = pa_get_theme_int(uip->theme, "graphics_scale", 1);

	uip->element[PA_UI_ELEMENT_RIGHT_PANE] = t3gui_dialog_add_element(uip->dialog[PA_UI_DIALOG_MAIN], uip->theme->theme[PA_UI_THEME_BOX], t3gui_box_proc, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL);
	uip->element[PA_UI_ELEMENT_BUTTON_PIXEL] = t3gui_dialog_add_element(uip->dialog[PA_UI_DIALOG_MAIN], uip->theme->theme[PA_UI_THEME_BUTTON], t3gui_push_button_proc, 0, 0, 0, 0, 0, 0, scale, 0, "Pixel", pa_tool_pixel_button_proc, uip->bitmap[PA_UI_BITMAP_TOOL_PIXEL]);
	uip->element[PA_UI_ELEMENT_BUTTON_LINE] = t3gui_dialog_add_element(uip->dialog[PA_UI_DIALOG_MAIN], uip->theme->theme[PA_UI_THEME_BUTTON], t3gui_push_button_proc, 0, 0, 0, 0, 0, 0, scale, 0, "Line", pa_tool_line_button_proc, uip->bitmap[PA_UI_BITMAP_TOOL_LINE]);
	uip->element[PA_UI_ELEMENT_BUTTON_RECTANGLE] = t3gui_dialog_add_element(uip->dialog[PA_UI_DIALOG_MAIN], uip->theme->theme[PA_UI_THEME_BUTTON], t3gui_push_button_proc, 0, 0, 0, 0, 0, 0, scale, 0, "Rectangle", pa_tool_rectangle_button_proc, uip->bitmap[PA_UI_BITMAP_TOOL_RECTANGLE]);
	uip->element[PA_UI_ELEMENT_BUTTON_FILLED_RECTANGLE] = t3gui_dialog_add_element(uip->dialog[PA_UI_DIALOG_MAIN], uip->theme->theme[PA_UI_THEME_BUTTON], t3gui_push_button_proc, 0, 0, 0, 0, 0, 0, scale, 0, "FRectangle", pa_tool_filled_rectangle_button_proc, uip->bitmap[PA_UI_BITMAP_TOOL_FILLED_RECTANGLE]);
	uip->element[PA_UI_ELEMENT_BUTTON_OVAL] = t3gui_dialog_add_element(uip->dialog[PA_UI_DIALOG_MAIN], uip->theme->theme[PA_UI_THEME_BUTTON], t3gui_push_button_proc, 0, 0, 0, 0, 0, 0, scale, 0, "Oval", pa_tool_oval_button_proc, uip->bitmap[PA_UI_BITMAP_TOOL_OVAL]);
	uip->element[PA_UI_ELEMENT_BUTTON_FILLED_OVAL] = t3gui_dialog_add_element(uip->dialog[PA_UI_DIALOG_MAIN], uip->theme->theme[PA_UI_THEME_BUTTON], t3gui_push_button_proc, 0, 0, 0, 0, 0, 0, scale, 0, "FOval", pa_tool_filled_oval_button_proc, uip->bitmap[PA_UI_BITMAP_TOOL_FILLED_OVAL]);
	uip->element[PA_UI_ELEMENT_BUTTON_FLOOD_FILL] = t3gui_dialog_add_element(uip->dialog[PA_UI_DIALOG_MAIN], uip->theme->theme[PA_UI_THEME_BUTTON], t3gui_push_button_proc, 0, 0, 0, 0, 0, 0, scale, 0, "Flood", pa_tool_flood_fill_button_proc, uip->bitmap[PA_UI_BITMAP_TOOL_FLOOD]);
	uip->element[PA_UI_ELEMENT_BUTTON_ERASER] = t3gui_dialog_add_element(uip->dialog[PA_UI_DIALOG_MAIN], uip->theme->theme[PA_UI_THEME_BUTTON], t3gui_push_button_proc, 0, 0, 0, 0, 0, 0, scale, 0, "Eraser", pa_tool_eraser_button_proc, uip->bitmap[PA_UI_BITMAP_TOOL_ERASER]);
	uip->element[PA_UI_ELEMENT_BUTTON_DROPPER] = t3gui_dialog_add_element(uip->dialog[PA_UI_DIALOG_MAIN], uip->theme->theme[PA_UI_THEME_BUTTON], t3gui_push_button_proc, 0, 0, 0, 0, 0, 0, scale, 0, "Dropper", pa_tool_dropper_button_proc, uip->bitmap[PA_UI_BITMAP_TOOL_DROPPER]);
	uip->element[PA_UI_ELEMENT_BUTTON_SELECTION] = t3gui_dialog_add_element(uip->dialog[PA_UI_DIALOG_MAIN], uip->theme->theme[PA_UI_THEME_BUTTON], t3gui_push_button_proc, 0, 0, 0, 0, 0, 0, scale, 0, "Selection", pa_tool_selection_button_proc, uip->bitmap[PA_UI_BITMAP_TOOL_SELECTION]);
	uip->element[PA_UI_ELEMENT_BUTTON_FRAME] = t3gui_dialog_add_element(uip->dialog[PA_UI_DIALOG_MAIN], uip->theme->theme[PA_UI_THEME_BUTTON], t3gui_push_button_proc, 0, 0, 0, 0, 0, 0, scale, 0, "Frame", pa_tool_frame_button_proc, uip->bitmap[PA_UI_BITMAP_TOOL_FRAME]);
	uip->element[PA_UI_ELEMENT_LAYER_LIST] = t3gui_dialog_add_element(uip->dialog[PA_UI_DIALOG_MAIN], NULL, pa_list_proc, 0, 0, 0, 0, 0, D_SETFOCUS, 0, 0, pa_layer_list_proc, NULL, cep);
	uip->element[PA_UI_ELEMENT_BUTTON_ADD_LAYER] = t3gui_dialog_add_element(uip->dialog[PA_UI_DIALOG_MAIN], uip->theme->theme[PA_UI_THEME_BUTTON], t3gui_push_button_proc, 0, 0, 0, 0, 0, 0, scale, 0, "+", pa_layer_add_button_proc, uip->bitmap[PA_UI_BITMAP_LAYER_ADD]);
	uip->element[PA_UI_ELEMENT_BUTTON_REMOVE_LAYER] = t3gui_dialog_add_element(uip->dialog[PA_UI_DIALOG_MAIN], uip->theme->theme[PA_UI_THEME_BUTTON], t3gui_push_button_proc, 0, 0, 0, 0, 0, 0, scale, 0, "-", pa_layer_remove_button_proc, uip->bitmap[PA_UI_BITMAP_LAYER_REMOVE]);
	uip->element[PA_UI_ELEMENT_MAP] = t3gui_dialog_add_element(uip->dialog[PA_UI_DIALOG_MAIN], NULL, pa_gui_map_proc, 0, 0, 0, 0, 0, 0, 0, 0, cep, NULL, NULL);
}

static void add_left_pane(PA_UI * uip, PA_CANVAS_EDITOR * cep)
{
	uip->element[PA_UI_ELEMENT_LEFT_PANE] = t3gui_dialog_add_element(uip->dialog[PA_UI_DIALOG_MAIN], NULL, t3gui_box_proc, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL);
	uip->element[PA_UI_ELEMENT_LEFT_COLOR] = t3gui_dialog_add_element(uip->dialog[PA_UI_DIALOG_MAIN], NULL, pa_gui_color_proc, 0, 0, 0, 0, 0, 0, 0, 0, &cep->left_color.color, NULL, NULL);
	uip->element[PA_UI_ELEMENT_RIGHT_COLOR] = t3gui_dialog_add_element(uip->dialog[PA_UI_DIALOG_MAIN], NULL, pa_gui_color_proc, 0, 0, 0, 0, 0, 0, 0, 0, &cep->right_color.color, NULL, NULL);
	cep->left_color.shade_slider_element = t3gui_dialog_add_element(uip->dialog[PA_UI_DIALOG_MAIN], NULL, t3gui_slider_proc, 0, 0, 0, 0, 0, 0, 1000, 0, NULL, NULL, NULL);
	uip->element[PA_UI_ELEMENT_LEFT_SHADE_SLIDER] = cep->left_color.shade_slider_element;
	cep->right_color.shade_slider_element = t3gui_dialog_add_element(uip->dialog[PA_UI_DIALOG_MAIN], NULL, t3gui_slider_proc, 0, 0, 0, 0, 0, 0, 1000, 0, NULL, NULL, NULL);
	uip->element[PA_UI_ELEMENT_RIGHT_SHADE_SLIDER] = cep->right_color.shade_slider_element;
	cep->left_color.alpha_slider_element = t3gui_dialog_add_element(uip->dialog[PA_UI_DIALOG_MAIN], NULL, t3gui_slider_proc, 0, 0, 0, 0, 0, 0, 1000, 0, NULL, NULL, NULL);
	uip->element[PA_UI_ELEMENT_LEFT_ALPHA_SLIDER] = cep->left_color.alpha_slider_element;
	cep->right_color.alpha_slider_element = t3gui_dialog_add_element(uip->dialog[PA_UI_DIALOG_MAIN], NULL, t3gui_slider_proc, 0, 0, 0, 0, 0, 0, 1000, 0, NULL, NULL, NULL);
	uip->element[PA_UI_ELEMENT_RIGHT_ALPHA_SLIDER] = cep->right_color.alpha_slider_element;

	add_color_pickers(uip, cep, uip->dialog[PA_UI_DIALOG_MAIN], 0, 0);

	add_color_palette(uip, cep, uip->dialog[PA_UI_DIALOG_MAIN], 0, 0);

	uip->element[PA_UI_ELEMENT_PALETTE] = t3gui_dialog_add_element(uip->dialog[PA_UI_DIALOG_MAIN], NULL, pa_gui_palette_proc, 0, 0, 0, 0, 0, 0, 0, 0, &cep->left_color.base_color, &cep->right_color.base_color, NULL);
}

static void add_status_bar(PA_UI * uip, PA_CANVAS_EDITOR * cep)
{
	uip->element[PA_UI_ELEMENT_STATUS_BAR] = t3gui_dialog_add_element(uip->dialog[PA_UI_DIALOG_MAIN], NULL, t3gui_box_proc, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL);
	uip->element[PA_UI_ELEMENT_STATUS_LEFT_MESSAGE] = t3gui_dialog_add_element(uip->dialog[PA_UI_DIALOG_MAIN], NULL, t3gui_text_proc, 0, 0, 0, 0, 0, 0, 0, 0, uip->status_left_message, NULL, NULL);
	uip->element[PA_UI_ELEMENT_STATUS_MIDDLE_MESSAGE] = t3gui_dialog_add_element(uip->dialog[PA_UI_DIALOG_MAIN], NULL, t3gui_text_proc, 0, 0, 0, 0, 0, 0, 0, 0, uip->status_middle_message, NULL, NULL);
	uip->element[PA_UI_ELEMENT_STATUS_RIGHT_MESSAGE] = t3gui_dialog_add_element(uip->dialog[PA_UI_DIALOG_MAIN], NULL, t3gui_text_proc, 0, 0, 0, 0, 0, 0, 0, 0, uip->status_right_message, NULL, NULL);
}

static bool create_gui(PA_UI * uip, PA_CANVAS_EDITOR * cep)
{
	uip->dialog[PA_UI_DIALOG_MAIN] = t3gui_create_dialog();
	if(!uip->dialog[PA_UI_DIALOG_MAIN])
	{
		return false;
	}
	add_toolbar(uip, cep);
	add_right_pane(uip, cep);
	add_left_pane(uip, cep);
	add_status_bar(uip, cep);

	cep->editor_element = t3gui_dialog_add_element(uip->dialog[PA_UI_DIALOG_MAIN], NULL, pa_gui_canvas_editor_proc, 0, 0, 0, 0, 0, 0, 0, 0, cep, NULL, NULL);
	uip->element[PA_UI_ELEMENT_CANVAS_EDITOR] = cep->editor_element;

	return true;
}

PA_UI * pa_create_ui(PA_CANVAS_EDITOR * cep)
{
	PA_UI * uip;

	uip = malloc(sizeof(PA_UI));
	if(uip)
	{
		memset(uip, 0, sizeof(PA_UI));

		if(!load_resources(uip))
		{
			goto fail;
		}
		set_default_palette(cep);
		if(!pa_setup_menus(uip))
		{
			goto fail;
		}
		pa_update_undo_name(cep);
		pa_update_redo_name(cep);
		t3f_attach_menu(uip->menu[PA_UI_MENU_MAIN]);

		if(!create_gui(uip, cep))
		{
			goto fail;
		}
		pa_set_color(&cep->left_color, al_map_rgba_f(1.0, 0.0, 0.0, 1.0));
		pa_set_color(&cep->right_color, al_map_rgba_f(0.0, 0.0, 0.0, 1.0));
		pa_canvas_editor_update_pick_colors(cep);
	}
	return uip;

	fail:
	{
		pa_destroy_ui(uip);
		return NULL;
	}
}

void pa_destroy_ui(PA_UI * uip)
{
	int i;

	for(i = 0; i < PA_UI_MAX_BITMAPS; i++)
	{
		if(uip->bitmap[i])
		{
			al_destroy_bitmap(uip->bitmap[i]);
		}
	}
	pa_destroy_menus(uip);
	for(i = 0; i < PA_UI_MAX_DIALOGS; i++)
	{
		if(uip->dialog[i])
		{
			t3gui_destroy_dialog(uip->dialog[i]);
		}
	}
	if(uip->theme)
	{
		pa_destroy_theme(uip->theme);
	}
	free(uip);
}

static void select_button(PA_UI * uip, int button)
{
	uip->element[PA_UI_ELEMENT_BUTTON_PIXEL]->flags &= ~D_SELECTED;
	uip->element[PA_UI_ELEMENT_BUTTON_LINE]->flags &= ~D_SELECTED;
	uip->element[PA_UI_ELEMENT_BUTTON_RECTANGLE]->flags &= ~D_SELECTED;
	uip->element[PA_UI_ELEMENT_BUTTON_FILLED_RECTANGLE]->flags &= ~D_SELECTED;
	uip->element[PA_UI_ELEMENT_BUTTON_OVAL]->flags &= ~D_SELECTED;
	uip->element[PA_UI_ELEMENT_BUTTON_FILLED_OVAL]->flags &= ~D_SELECTED;
	uip->element[PA_UI_ELEMENT_BUTTON_ERASER]->flags &= ~D_SELECTED;
	uip->element[PA_UI_ELEMENT_BUTTON_FLOOD_FILL]->flags &= ~D_SELECTED;
	uip->element[PA_UI_ELEMENT_BUTTON_DROPPER]->flags &= ~D_SELECTED;
	uip->element[PA_UI_ELEMENT_BUTTON_SELECTION]->flags &= ~D_SELECTED;
	uip->element[PA_UI_ELEMENT_BUTTON_FRAME]->flags &= ~D_SELECTED;
	if(button >= 0)
	{
		uip->element[button]->flags |= D_SELECTED;
	}
}

void pa_process_ui(PA_UI * uip)
{
	PA_CANVAS_EDITOR * cep = (PA_CANVAS_EDITOR *)uip->element[PA_UI_ELEMENT_CANVAS_EDITOR]->dp;
	int old_layer_d1;

	uip->element[PA_UI_ELEMENT_LAYER_LIST]->d1 = cep->canvas->layer_max - cep->current_layer - 1;
	old_layer_d1 = uip->element[PA_UI_ELEMENT_LAYER_LIST]->d1;
	if(cep->canvas->layer_max > 1)
	{
		uip->element[PA_UI_ELEMENT_BUTTON_REMOVE_LAYER]->flags &= ~D_DISABLED;
	}
	else
	{
		uip->element[PA_UI_ELEMENT_BUTTON_REMOVE_LAYER]->flags |= D_DISABLED;
	}

	if(cep->current_tool != PA_TOOL_SELECTION && cep->current_tool != PA_TOOL_FRAME)
	{
		cep->want_cursor = ALLEGRO_SYSTEM_MOUSE_CURSOR_PRECISION;
	}
	if(t3gui_get_mouse_x() >= uip->element[PA_UI_ELEMENT_CANVAS_EDITOR]->x && t3gui_get_mouse_x() < uip->element[PA_UI_ELEMENT_CANVAS_EDITOR]->x + uip->element[PA_UI_ELEMENT_CANVAS_EDITOR]->w && t3gui_get_mouse_y() >= uip->element[PA_UI_ELEMENT_CANVAS_EDITOR]->y && t3gui_get_mouse_y() < uip->element[PA_UI_ELEMENT_CANVAS_EDITOR]->y + uip->element[PA_UI_ELEMENT_CANVAS_EDITOR]->h)
	{
		pa_set_mouse_cursor(cep->want_cursor);
	}
	else
	{
		pa_set_mouse_cursor(ALLEGRO_SYSTEM_MOUSE_CURSOR_ARROW);
	}

	t3gui_logic();

	/* update button selection */
	switch(cep->current_tool)
	{
		case PA_TOOL_PIXEL:
		{
			select_button(uip, PA_UI_ELEMENT_BUTTON_PIXEL);
			break;
		}
		case PA_TOOL_LINE:
		{
			select_button(uip, PA_UI_ELEMENT_BUTTON_LINE);
			break;
		}
		case PA_TOOL_RECTANGLE:
		{
			select_button(uip, PA_UI_ELEMENT_BUTTON_RECTANGLE);
			break;
		}
		case PA_TOOL_FILLED_RECTANGLE:
		{
			select_button(uip, PA_UI_ELEMENT_BUTTON_FILLED_RECTANGLE);
			break;
		}
		case PA_TOOL_OVAL:
		{
			select_button(uip, PA_UI_ELEMENT_BUTTON_OVAL);
			break;
		}
		case PA_TOOL_FILLED_OVAL:
		{
			select_button(uip, PA_UI_ELEMENT_BUTTON_FILLED_OVAL);
			break;
		}
		case PA_TOOL_FLOOD_FILL:
		{
			select_button(uip, PA_UI_ELEMENT_BUTTON_FLOOD_FILL);
			break;
		}
		case PA_TOOL_ERASER:
		{
			select_button(uip, PA_UI_ELEMENT_BUTTON_ERASER);
			break;
		}
		case PA_TOOL_DROPPER:
		{
			select_button(uip, PA_UI_ELEMENT_BUTTON_DROPPER);
			break;
		}
		case PA_TOOL_SELECTION:
		{
			select_button(uip, PA_UI_ELEMENT_BUTTON_SELECTION);
			break;
		}
		case PA_TOOL_FRAME:
		{
			select_button(uip, PA_UI_ELEMENT_BUTTON_FRAME);
			break;
		}
		default:
		{
			select_button(uip, -1);
		}
	}

	if(uip->element[PA_UI_ELEMENT_LAYER_LIST]->d1 != old_layer_d1)
	{
		pa_select_canvas_editor_layer(cep, cep->canvas->layer_max - uip->element[PA_UI_ELEMENT_LAYER_LIST]->d1 - 1);
	}
	pa_update_window_title(cep);
}

void pa_render_ui(PA_UI * uip)
{
	PA_CANVAS_EDITOR * cep = (PA_CANVAS_EDITOR *)uip->element[PA_UI_ELEMENT_CANVAS_EDITOR]->dp;
	int i, j;
	int tw, th;

	al_use_shader(cep->premultiplied_alpha_shader);
	tw = t3f_default_view->width / PA_UI_BG_SCALE + 1;
	th = t3f_default_view->height / PA_UI_BG_SCALE + 1;
	for(i = 0; i < th; i++)
	{
		for(j = 0; j < tw; j++)
		{
			t3f_draw_scaled_bitmap(uip->bitmap[PA_UI_BITMAP_BG], t3f_color_white, j * PA_UI_BG_SCALE, i * PA_UI_BG_SCALE, 0, PA_UI_BG_SCALE, PA_UI_BG_SCALE, 0);
		}
	}
	t3gui_render();
	al_use_shader(cep->standard_shader);
}
