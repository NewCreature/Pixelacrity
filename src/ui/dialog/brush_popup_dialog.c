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
	esl = pa_get_theme_int(dp->theme, "edge_left_space", 8);
	esr = pa_get_theme_int(dp->theme, "edge_right_space", 8);
	est = pa_get_theme_int(dp->theme, "edge_top_space", 8);
	esb = pa_get_theme_int(dp->theme, "edge_bottom_space", 8);
	scale = pa_get_theme_int(dp->theme, "pixel_size", 1);
	space = pa_get_theme_int(dp->theme, "edge_space_left", 4) * scale;
	x_offset = 0;
	y_offset = 0;

	t3gui_dialog_add_element(dp->dialog, NULL, pa_gui_shader_proc, 0, 0, 0, 0, 0, D_NOFOCUS, 0, 0, "data/shaders/premultiplied_alpha_shader.glsl", NULL, NULL);
	bg_box = t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_POPUP_BOX], t3gui_box_proc, 0, 0, 0, 0, 0, D_NOFOCUS, 0, 0, NULL, NULL, NULL);
	frame_box = t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_BOX], t3gui_box_proc, 0, 0, 0, 0, 0, D_NOFOCUS, 0, 0, NULL, NULL, NULL);

	pos_x = 0;
	pos_y = 0;
	t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_BOX], pa_gui_brush_proc, pos_x, pos_y, width, height, 0, D_NOFOCUS, 0, 0, cep, NULL, NULL);
  pos_y += est + height;

	/* buttons */
	button[0] = t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_TEXT_BUTTON], t3gui_push_button_proc, pos_x, pos_y, 240, 32 + dp->theme->theme[PA_UI_THEME_BUTTON]->state[0].top_margin + dp->theme->theme[PA_UI_THEME_BUTTON]->state[0].bottom_margin, 27, D_EXIT, 0, 0, "Cancel", pa_brush_cancel_button_proc, NULL);
	pos_x += 240 + esl;
	button[1] = t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_TEXT_BUTTON], t3gui_push_button_proc, pos_x, pos_y, 240, 32 + dp->theme->theme[PA_UI_THEME_BUTTON]->state[0].top_margin + dp->theme->theme[PA_UI_THEME_BUTTON]->state[0].bottom_margin, '\r', D_EXIT, 0, 0, "Okay", pa_brush_okay_button_proc, NULL);

	/* fix size of dialog */
	t3gui_get_dialog_bounding_box(dp->dialog->element, &min_x, &min_y, &max_x, &max_y);
	button[0]->x = min_x;
	button[0]->w = (max_x - min_x) / 2 - space / 2;
	button[1]->x = (max_x - min_x) / 2 + space / 2;
	button[1]->w = button[0]->w;
	frame_box->x = min_x - space;
	frame_box->y = min_y - space;
	frame_box->w = max_x - min_x + space * 2;
	frame_box->h = max_y - min_y + space * 2;
	t3gui_position_dialog(dp->dialog->element, ox, oy);
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
