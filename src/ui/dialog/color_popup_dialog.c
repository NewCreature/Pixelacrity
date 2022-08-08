#include "t3gui/t3gui.h"
#include "defines.h"
#include "ui/dialog.h"
#include "color_popup_dialog.h"
#include "ui/element/color.h"
#include "ui/element/shader.h"
#include "modules/color.h"
#include "modules/pixel_shader.h"
#include "ui/canvas_editor/canvas_editor.h"
#include "ui/element_proc/button_proc.h"

static int old_r, old_g, old_b, old_a;
static char old_r_text[8], old_g_text[8], old_b_text[8], old_a_text[8], old_html_text[10];
static ALLEGRO_COLOR * edit_color = NULL;

PA_DIALOG * pa_create_color_editor_popup_dialog(ALLEGRO_COLOR * color, float ox, float oy)
{
	PA_DIALOG * dp;
	T3GUI_ELEMENT * bg_box;
	T3GUI_ELEMENT * frame_box;
	T3GUI_ELEMENT * button[2];
	int pos_x;
	int pos_vx;
	int pos_y;
	int pos_vy;
	int edit_height;
	int space;
	int scale;
	int edit_width;
	int slider_width;
	int x_offset = 0;
	int y_offset = 0;
	unsigned char r, g, b, a;
	int min_x, min_y, max_x, max_y;
	const char * val;
	int color_size;
	int left_pane_width;
	int esl, esr;
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
	color_size = pa_get_theme_int(dp->theme, "color_size", 12);
	esl = pa_get_theme_int(dp->theme, "edge_left_space", 8);
	esr = pa_get_theme_int(dp->theme, "edge_right_space", 8);
	left_pane_width = PA_COLOR_PICKER_SHADES * color_size + esl + esr;
	s = left_pane_width / 2 - esl - esl / 2;

	edit_color = color;
	al_unmap_rgba(*color, &r, &g, &b, &a);
	scale = pa_get_theme_int(dp->theme, "pixel_size", 1);
	space = pa_get_theme_int(dp->theme, "edge_space_left", 4) * scale;
	x_offset = 0;
	y_offset = 0;

	t3gui_dialog_add_element(dp->dialog, NULL, pa_gui_shader_proc, 0, 0, 0, 0, 0, D_NOFOCUS, 0, 0, "data/shaders/premultiplied_alpha_shader.glsl", NULL, NULL);
	bg_box = t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_POPUP_BOX], t3gui_box_proc, 0, 0, 0, 0, 0, D_NOFOCUS, 0, 0, NULL, NULL, NULL);
	frame_box = t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_BOX], t3gui_box_proc, 0, 0, 0, 0, 0, D_NOFOCUS, 0, 0, NULL, NULL, NULL);
	pos_y = 0;
	edit_height = al_get_font_line_height(dp->theme->theme[PA_UI_THEME_LIST_BOX]->state[0].font[0]) + dp->theme->theme[PA_UI_THEME_LIST_BOX]->state[0].top_margin + dp->theme->theme[PA_UI_THEME_LIST_BOX]->state[0].bottom_margin;
	pos_vy = edit_height + pa_get_theme_int(dp->theme, "edge_top_space", 4);
	edit_width = al_get_text_width(dp->theme->theme[PA_UI_THEME_LIST_BOX]->state[0].font[0], "0000") + space + space;
	slider_width = (256) * scale + dp->theme->theme[PA_UI_THEME_SLIDER]->state[0].size;

	pos_x = 0;
	pos_y = 0;
	dp->element[PA_COLOR_DIALOG_ELEMENT_COLOR] = t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_BOX], pa_gui_color_proc, pos_x, pos_y, s, s, 0, D_NOFOCUS, 0, 0, pa_create_gui_color_data(color, NULL, NULL, NULL, NULL, NULL, NULL), NULL, NULL);

	/* R */
	dp->edit_text[0] = malloc(4);
	if(!dp->edit_text[0])
	{
		goto fail;
	}
	sprintf(dp->edit_text[0], "%d", r);
	pos_x = s + esl + esr;
	pos_vx = al_get_text_width(dp->theme->theme[PA_UI_THEME_LIST_BOX]->state[0].font[0], "R") * scale + space;
	t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_LIST_BOX], t3gui_text_proc, pos_x, pos_y, al_get_text_width(dp->theme->theme[PA_UI_THEME_LIST_BOX]->state[0].font[0], "R"), al_get_font_line_height(dp->theme->theme[PA_UI_THEME_LIST_BOX]->state[0].font[0]), 0, D_NOFOCUS, 0, 0, "R", NULL, NULL);
	pos_x += pos_vx;
	dp->element[PA_COLOR_DIALOG_ELEMENT_R_SLIDER] = t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_SLIDER], t3gui_slider_proc, pos_x, pos_y, slider_width, edit_height, 0, D_NOFOCUS, 255, r, NULL, NULL, NULL);
	pos_x += slider_width + space;
	t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_LIST_BOX], t3gui_edit_proc, pos_x, pos_y, edit_width, edit_height, 0, D_SETFOCUS, 3, 0, dp->edit_text[0], "0123456789", NULL);
	pos_x += edit_width + space + space;
	pos_y += pos_vy;

	/* G */
	dp->edit_text[1] = malloc(4);
	if(!dp->edit_text[1])
	{
		goto fail;
	}
	sprintf(dp->edit_text[1], "%d", g);
	pos_x = s + esl + esr;
	pos_vx = al_get_text_width(dp->theme->theme[PA_UI_THEME_LIST_BOX]->state[0].font[0], "R") * scale + space;
	t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_LIST_BOX], t3gui_text_proc, pos_x, pos_y, al_get_text_width(dp->theme->theme[PA_UI_THEME_LIST_BOX]->state[0].font[0], "G"), al_get_font_line_height(dp->theme->theme[PA_UI_THEME_LIST_BOX]->state[0].font[0]), 0, D_NOFOCUS, 0, 0, "G", NULL, NULL);
	pos_x += pos_vx;
	dp->element[PA_COLOR_DIALOG_ELEMENT_G_SLIDER] = t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_SLIDER], t3gui_slider_proc, pos_x, pos_y, slider_width, edit_height, 0, D_NOFOCUS, 255, g, NULL, NULL, NULL);
	pos_x += slider_width + space;
	t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_LIST_BOX], t3gui_edit_proc, pos_x, pos_y, edit_width, edit_height, 0, 0, 3, 0, dp->edit_text[1], "0123456789", NULL);
	pos_y += pos_vy;

	/* B */
	dp->edit_text[2] = malloc(4);
	if(!dp->edit_text[2])
	{
		goto fail;
	}
	sprintf(dp->edit_text[2], "%d", b);
	pos_x = s + esl + esr;
	pos_vx = al_get_text_width(dp->theme->theme[PA_UI_THEME_LIST_BOX]->state[0].font[0], "R") * scale + space;
	t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_LIST_BOX], t3gui_text_proc, pos_x, pos_y, al_get_text_width(dp->theme->theme[PA_UI_THEME_LIST_BOX]->state[0].font[0], "B"), al_get_font_line_height(dp->theme->theme[PA_UI_THEME_LIST_BOX]->state[0].font[0]), 0, D_NOFOCUS, 0, 0, "B", NULL, NULL);
	pos_x += pos_vx;
	dp->element[PA_COLOR_DIALOG_ELEMENT_B_SLIDER] = t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_SLIDER], t3gui_slider_proc, pos_x, pos_y, slider_width, edit_height, 0, D_NOFOCUS, 255, b, NULL, NULL, NULL);
	pos_x += slider_width + space;
	t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_LIST_BOX], t3gui_edit_proc, pos_x, pos_y, edit_width, edit_height, 0, 0, 3, 0, dp->edit_text[2], "0123456789", NULL);
	pos_y += pos_vy;

	/* A */
	dp->edit_text[3] = malloc(4);
	if(!dp->edit_text[3])
	{
		goto fail;
	}
	sprintf(dp->edit_text[3], "%d", a);
	pos_x = s + esl + esr;
	pos_vx = al_get_text_width(dp->theme->theme[PA_UI_THEME_LIST_BOX]->state[0].font[0], "R") * scale + space;
	t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_LIST_BOX], t3gui_text_proc, pos_x, pos_y, al_get_text_width(dp->theme->theme[PA_UI_THEME_LIST_BOX]->state[0].font[0], "A"), al_get_font_line_height(dp->theme->theme[PA_UI_THEME_LIST_BOX]->state[0].font[0]), 0, D_NOFOCUS, 0, 0, "A", NULL, NULL);
	pos_x += pos_vx;
	dp->element[PA_COLOR_DIALOG_ELEMENT_A_SLIDER] = t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_SLIDER], t3gui_slider_proc, pos_x, pos_y, slider_width, edit_height, 0, D_NOFOCUS, 255, a, NULL, NULL, NULL);
	pos_x += slider_width + space;
	t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_LIST_BOX], t3gui_edit_proc, pos_x, pos_y, edit_width, edit_height, 0, 0, 3, 0, dp->edit_text[3], "0123456789", NULL);
	pos_y += pos_vy;

	/* HTML */
	dp->edit_text[4] = malloc(9);
	if(!dp->edit_text[4])
	{
		goto fail;
	}
	pa_color_to_html(*color, dp->edit_text[4]);
	pos_x = s + esl + esr;
	pos_vx = al_get_text_width(dp->theme->theme[PA_UI_THEME_LIST_BOX]->state[0].font[0], "HTML") + space;
	t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_LIST_BOX], t3gui_text_proc, pos_x, pos_y, al_get_text_width(dp->theme->theme[PA_UI_THEME_LIST_BOX]->state[0].font[0], "HTML"), al_get_font_line_height(dp->theme->theme[PA_UI_THEME_LIST_BOX]->state[0].font[0]), 0, 0, 0, 0, "HTML", NULL, NULL);
	pos_x += pos_vx;
	t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_LIST_BOX], t3gui_edit_proc, pos_x, pos_y, al_get_text_width(dp->theme->theme[PA_UI_THEME_LIST_BOX]->state[0].font[0], "000000000") + space + space, edit_height, 0, D_USER, 8, 0, dp->edit_text[4], "0123456789aAbBcCdDeEfF", NULL);
	pos_y += pos_vy;

	/* buttons */
	pos_x = esl;
	button[0] = t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_TEXT_BUTTON], t3gui_push_button_proc, pos_x, pos_y, 240, 32 + dp->theme->theme[PA_UI_THEME_BUTTON]->state[0].top_margin + dp->theme->theme[PA_UI_THEME_BUTTON]->state[0].bottom_margin, 27, D_EXIT, 0, 0, "Cancel", pa_color_cancel_button_proc, NULL);
	pos_x += 240 + esl;
	button[1] = t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_TEXT_BUTTON], t3gui_push_button_proc, pos_x, pos_y, 240, 32 + dp->theme->theme[PA_UI_THEME_BUTTON]->state[0].top_margin + dp->theme->theme[PA_UI_THEME_BUTTON]->state[0].bottom_margin, '\r', D_EXIT, 0, 0, "Okay", pa_color_okay_button_proc, NULL);

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

void pa_color_dialog_pre_logic(PA_DIALOG * dp)
{
	old_r = dp->element[PA_COLOR_DIALOG_ELEMENT_R_SLIDER]->d2;
	strcpy(old_r_text, dp->edit_text[0]);
	old_g = dp->element[PA_COLOR_DIALOG_ELEMENT_G_SLIDER]->d2;
	strcpy(old_g_text, dp->edit_text[1]);
	old_b = dp->element[PA_COLOR_DIALOG_ELEMENT_B_SLIDER]->d2;
	strcpy(old_b_text, dp->edit_text[2]);
	old_a = dp->element[PA_COLOR_DIALOG_ELEMENT_A_SLIDER]->d2;
	strcpy(old_a_text, dp->edit_text[3]);
	strcpy(old_html_text, dp->edit_text[4]);
}

static bool update_slider_or_text(T3GUI_ELEMENT * ep, int old, char * text, const char * old_text)
{
	if(ep->d2 != old)
	{
		sprintf(text, "%d", ep->d2);
		return true;
	}
	else if(strlen(text) && strcmp(old_text, text))
	{
		ep->d2 = atoi(text);
		if(ep->d2 < 0)
		{
			ep->d2 = 0;
		}
		else if(ep->d2 > 255)
		{
			ep->d2 = 255;
		}
		sprintf(text, "%d", ep->d2);
		return true;
	}
	return false;
}

static void update_color_dialog(PA_DIALOG * dp)
{
	unsigned char r, g, b, a;

	al_unmap_rgba(*edit_color, &r, &g, &b, &a);
	sprintf(dp->edit_text[0], "%d", r);
	sprintf(dp->edit_text[1], "%d", g);
	sprintf(dp->edit_text[2], "%d", b);
	sprintf(dp->edit_text[3], "%d", a);
//	pa_color_to_html(*edit_color, dp->edit_text[4]);
	dp->element[PA_COLOR_DIALOG_ELEMENT_R_SLIDER]->d2 = r;
	dp->element[PA_COLOR_DIALOG_ELEMENT_G_SLIDER]->d2 = g;
	dp->element[PA_COLOR_DIALOG_ELEMENT_B_SLIDER]->d2 = b;
	dp->element[PA_COLOR_DIALOG_ELEMENT_A_SLIDER]->d2 = a;
}

static void update_color_html(PA_DIALOG * dp)
{
	unsigned char r, g, b, a;

	al_unmap_rgba(*edit_color, &r, &g, &b, &a);
	sprintf(dp->edit_text[4], "%02X%02X%02X%02X", r, g, b, a);
}

void pa_color_dialog_post_logic(PA_DIALOG * dp)
{
	bool update_html = false;

	if(update_slider_or_text(dp->element[PA_COLOR_DIALOG_ELEMENT_R_SLIDER], old_r, dp->edit_text[0], old_r_text))
	{
		update_html = true;
	}
	if(update_slider_or_text(dp->element[PA_COLOR_DIALOG_ELEMENT_G_SLIDER], old_g, dp->edit_text[1], old_g_text))
	{
		update_html = true;
	}
	if(update_slider_or_text(dp->element[PA_COLOR_DIALOG_ELEMENT_B_SLIDER], old_b, dp->edit_text[2], old_b_text))
	{
		update_html = true;
	}
	if(update_slider_or_text(dp->element[PA_COLOR_DIALOG_ELEMENT_A_SLIDER], old_a, dp->edit_text[3], old_a_text))
	{
		update_html = true;
	}
	if(strcmp(dp->edit_text[4], old_html_text))
	{
		if(strlen(dp->edit_text[4]) == 6 || strlen(dp->edit_text[4]) == 8)
		{
			*edit_color = pa_get_color_from_html(dp->edit_text[4]);
		}
		update_color_dialog(dp);
	}
	*edit_color = al_map_rgba(dp->element[PA_COLOR_DIALOG_ELEMENT_R_SLIDER]->d2, dp->element[PA_COLOR_DIALOG_ELEMENT_G_SLIDER]->d2, dp->element[PA_COLOR_DIALOG_ELEMENT_B_SLIDER]->d2, dp->element[PA_COLOR_DIALOG_ELEMENT_A_SLIDER]->d2);
	if(update_html)
	{
		update_color_html(dp);
	}
}
