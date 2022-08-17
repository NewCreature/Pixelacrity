#include "t3gui/t3gui.h"
#include "defines.h"
#include "ui/dialog.h"
#include "color_popup_dialog.h"
#include "ui/element/brush.h"
#include "ui/element/shader.h"
#include "modules/color.h"
#include "modules/pixel_shader.h"
#include "ui/canvas_editor/canvas_editor.h"
#include "ui/element_proc/button_proc.h"
#include "brush_popup_dialog.h"

static int old_size;

static bool load_bitmap(PA_DIALOG * dp, int slot, const char * name)
{
	dp->bitmap[slot] = pa_load_theme_bitmap(dp->theme, name);
	if(!dp->bitmap[slot])
	{
		return false;
	}
	return true;
}

PA_DIALOG * pa_create_brush_popup_dialog(PA_CANVAS_EDITOR * cep, float ox, float oy, int width, int height)
{
	PA_DIALOG * dp;
	T3GUI_ELEMENT * bg_box;
	T3GUI_ELEMENT * frame_box;
	T3GUI_ELEMENT * button[2];
	int pos_x;
	int pos_vx;
	int pos_y;
	int pos_vy;
	int space;
	int scale;
  int graphics_scale;
  int button_size;
	int x_offset = 0;
	int y_offset = 0;
	int min_x, min_y, max_x, max_y;
	const char * val;
	int esl, esr, est, esb;
	int s;

	val = al_get_config_value(t3f_config, "App Data", "theme");
	if(!val)
	{
		val = PA_DEFAULT_THEME;
	}
	dp = pa_create_dialog(t3f_display, val, 640, 480, NULL);
	if(!dp)
	{
		goto fail;
	}
	if(!load_bitmap(dp, PA_UI_BITMAP_BRUSH_SQUARE, "brush_square"))
	{
    goto fail;
	}
	if(!load_bitmap(dp, PA_UI_BITMAP_BRUSH_CIRCLE, "brush_circle"))
	{
    goto fail;
	}
	if(!load_bitmap(dp, PA_UI_BITMAP_BRUSH_VLINE, "brush_vline"))
	{
    goto fail;
	}
	if(!load_bitmap(dp, PA_UI_BITMAP_BRUSH_DLINE_UR, "brush_dline_ur"))
	{
    goto fail;
	}
	if(!load_bitmap(dp, PA_UI_BITMAP_BRUSH_HLINE, "brush_hline"))
	{
    goto fail;
	}
	if(!load_bitmap(dp, PA_UI_BITMAP_BRUSH_DLINE_DR, "brush_dline_dr"))
	{
    goto fail;
	}
	if(!load_bitmap(dp, PA_UI_BITMAP_BRUSH_DIAMOND, "brush_diamond"))
	{
    goto fail;
	}
	esl = pa_get_theme_int(dp->theme, "edge_left_space", 8);
	esr = pa_get_theme_int(dp->theme, "edge_right_space", 8);
	est = pa_get_theme_int(dp->theme, "edge_top_space", 8);
	esb = pa_get_theme_int(dp->theme, "edge_bottom_space", 8);
	scale = pa_get_theme_int(dp->theme, "pixel_size", 1);
	space = pa_get_theme_int(dp->theme, "edge_space_left", 4) * scale;
  button_size = pa_get_theme_int(dp->theme, "button_size", 64);
  graphics_scale = pa_get_theme_int(dp->theme, "graphics_scale", 64);
	x_offset = 0;
	y_offset = 0;

	t3gui_dialog_add_element(dp->dialog, NULL, pa_gui_shader_proc, 0, 0, 0, 0, 0, D_NOFOCUS, 0, 0, "data/shaders/premultiplied_alpha_shader.glsl", NULL, NULL);
	bg_box = t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_POPUP_BOX], t3gui_box_proc, 0, 0, 0, 0, 0, D_NOFOCUS, 0, 0, NULL, NULL, NULL);
	frame_box = t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_BOX], t3gui_box_proc, 0, 0, 0, 0, 0, D_NOFOCUS, 0, 0, NULL, NULL, NULL);

	pos_x = 0;
	pos_y = 0;
	dp->element[PA_BRUSH_DIALOG_ELEMENT_BRUSH] = t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_BOX], pa_gui_brush_proc, pos_x, pos_y, width, height, 0, D_NOFOCUS, 0, 0, cep, NULL, NULL);
  dp->element[PA_BRUSH_DIALOG_ELEMENT_SIZE_SLIDER] = t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_SLIDER], t3gui_slider_proc, pos_x, pos_y + height + space, width, dp->theme->theme[PA_UI_THEME_SLIDER]->state[0].size, 0, 0, 64, cep->brush_size - 1, NULL, NULL, NULL);
  pos_x = pos_x - button_size - esl;
  t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_BUTTON], t3gui_push_button_proc, pos_x, pos_y, button_size, button_size, 0, 0, graphics_scale, 0, "Diamond", pa_brush_diamond_proc, dp->bitmap[PA_UI_BITMAP_BRUSH_DIAMOND]);
  pos_x = pos_x - button_size;
  t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_BUTTON], t3gui_push_button_proc, pos_x, pos_y, button_size, button_size, 0, 0, graphics_scale, 0, "DLine DR", pa_brush_dline_dr_proc, dp->bitmap[PA_UI_BITMAP_BRUSH_DLINE_DR]);
  pos_x = pos_x - button_size;
  t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_BUTTON], t3gui_push_button_proc, pos_x, pos_y, button_size, button_size, 0, 0, graphics_scale, 0, "HLine", pa_brush_hline_proc, dp->bitmap[PA_UI_BITMAP_BRUSH_HLINE]);
  pos_x = pos_x - button_size;
  t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_BUTTON], t3gui_push_button_proc, pos_x, pos_y, button_size, button_size, 0, 0, graphics_scale, 0, "DLine UR", pa_brush_dline_ur_proc, dp->bitmap[PA_UI_BITMAP_BRUSH_DLINE_UR]);
  pos_x = pos_x - button_size;
  t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_BUTTON], t3gui_push_button_proc, pos_x, pos_y, button_size, button_size, 0, 0, graphics_scale, 0, "VLine", pa_brush_vline_proc, dp->bitmap[PA_UI_BITMAP_BRUSH_VLINE]);
  pos_x = pos_x - button_size;
  t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_BUTTON], t3gui_push_button_proc, pos_x, pos_y, button_size, button_size, 0, 0, graphics_scale, 0, "Circle", pa_brush_circle_proc, dp->bitmap[PA_UI_BITMAP_BRUSH_CIRCLE]);
  pos_x = pos_x - button_size;
  t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_BUTTON], t3gui_push_button_proc, pos_x, pos_y, button_size, button_size, 0, 0, graphics_scale, 0, "Square", pa_brush_square_proc, dp->bitmap[PA_UI_BITMAP_BRUSH_SQUARE]);
  pos_y += est + height + space + dp->theme->theme[PA_UI_THEME_SLIDER]->state[0].size;

	/* buttons */
	button[0] = t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_TEXT_BUTTON], t3gui_push_button_proc, pos_x, pos_y, 240, 32 + dp->theme->theme[PA_UI_THEME_BUTTON]->state[0].top_margin + dp->theme->theme[PA_UI_THEME_BUTTON]->state[0].bottom_margin, 27, D_EXIT, 0, 0, "Cancel", pa_brush_cancel_button_proc, NULL);
	pos_x += 240 + esl;
	button[1] = t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_TEXT_BUTTON], t3gui_push_button_proc, pos_x, pos_y, 240, 32 + dp->theme->theme[PA_UI_THEME_BUTTON]->state[0].top_margin + dp->theme->theme[PA_UI_THEME_BUTTON]->state[0].bottom_margin, '\r', D_EXIT, 0, 0, "Okay", pa_brush_okay_button_proc, NULL);

	/* fix size of dialog */
	t3gui_get_dialog_bounding_box(dp->dialog->element, &min_x, &min_y, &max_x, &max_y);
	button[0]->x = min_x;
	button[0]->w = (max_x - min_x) / 2 - space / 2;
	button[1]->x = button[0]->x + button[0]->w + space;
	button[1]->w = button[0]->w;
	frame_box->x = min_x - space;
	frame_box->y = min_y - space;
	frame_box->w = max_x - min_x + space * 2;
	frame_box->h = max_y - min_y + space * 2;
	t3gui_align_dialog_element(dp->dialog->element, dp->element[PA_BRUSH_DIALOG_ELEMENT_BRUSH], ox, oy);
	bg_box->x = 0;
	bg_box->y = 0;
	bg_box->w = al_get_display_width(t3f_display);
	bg_box->h = al_get_display_height(t3f_display);


	return dp;

	fail:
	{
		if(dp)
		{
			pa_close_dialog(dp);
		}
		return NULL;
	}
}

void pa_brush_dialog_pre_logic(PA_DIALOG * dp)
{
  old_size = dp->element[PA_BRUSH_DIALOG_ELEMENT_SIZE_SLIDER]->d2;
}

void pa_brush_dialog_post_logic(PA_DIALOG * dp, void * dp3)
{
  PA_CANVAS_EDITOR * cep = (PA_CANVAS_EDITOR *)dp->element[PA_BRUSH_DIALOG_ELEMENT_BRUSH]->dp;

  if(old_size != dp->element[PA_BRUSH_DIALOG_ELEMENT_SIZE_SLIDER]->d2)
  {
    cep->brush_size = dp->element[PA_BRUSH_DIALOG_ELEMENT_SIZE_SLIDER]->d2 + 1;
    pa_brush_square_proc(NULL, dp3);
  }
}
