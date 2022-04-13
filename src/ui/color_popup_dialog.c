#include "t3gui/t3gui.h"
#include "popup_dialog.h"
#include "color_popup_dialog.h"

static int old_r, old_g, old_b, old_a;
static char old_r_text[8], old_g_text[8], old_b_text[8], old_a_text[8];
static ALLEGRO_COLOR * edit_color = NULL;

PA_POPUP_DIALOG * pa_create_color_editor_popup_dialog(ALLEGRO_COLOR * color)
{
	PA_POPUP_DIALOG * dp;
	int pos_x;
	int pos_vx;
	int pos_y;
	int pos_vy;
	int space;
	unsigned char r, g, b, a;

	dp = pa_create_popup_dialog(al_get_config_value(t3f_config, "App Data", "theme"), 640, 480, NULL);
	if(!dp)
	{
		goto fail;
	}
	edit_color = color;
	al_unmap_rgba(*color, &r, &g, &b, &a);
	space = pa_get_theme_int(dp->theme, "edge_space_left", 4);
	t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_BOX], t3gui_box_proc, 0, 0, 640, 480, 0, 0, 0, 0, NULL, NULL, NULL);
	pos_y = pa_get_theme_int(dp->theme, "edge_top_space", 4);
	pos_vy = al_get_font_line_height(dp->theme->theme[PA_UI_THEME_LIST_BOX]->state[0].font[0]) + pos_y;

	/* R */
	dp->edit_text[0] = malloc(4);
	if(!dp->edit_text[0])
	{
		goto fail;
	}
	sprintf(dp->edit_text[0], "%d", r);
	pos_x = space;
	pos_vx = al_get_text_width(dp->theme->theme[PA_UI_THEME_LIST_BOX]->state[0].font[0], "R") + space;
	t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_LIST_BOX], t3gui_text_proc, pos_x, pos_y, 640, al_get_font_line_height(dp->theme->theme[PA_UI_THEME_LIST_BOX]->state[0].font[0]), 0, 0, 0, 0, "R", NULL, NULL);
	pos_x += pos_vx;
	dp->element[PA_COLOR_DIALOG_ELEMENT_R_SLIDER] = t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_SLIDER], t3gui_slider_proc, pos_x, pos_y, 256, pos_vy, 0, 0, 255, r, NULL, NULL, NULL);
	pos_x += 256 + space;
	t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_LIST_BOX], t3gui_edit_proc, pos_x, pos_y, 128, pos_vy, 0, 0, 3, 0, dp->edit_text[0], NULL, NULL);
	pos_y += pos_vy;

	/* G */
	dp->edit_text[1] = malloc(4);
	if(!dp->edit_text[1])
	{
		goto fail;
	}
	sprintf(dp->edit_text[1], "%d", g);
	pos_x = space;
	pos_vx = al_get_text_width(dp->theme->theme[PA_UI_THEME_LIST_BOX]->state[0].font[0], "R") + space;
	t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_LIST_BOX], t3gui_text_proc, pos_x, pos_y, 640, al_get_font_line_height(dp->theme->theme[PA_UI_THEME_LIST_BOX]->state[0].font[0]), 0, 0, 0, 0, "G", NULL, NULL);
	pos_x += pos_vx;
	dp->element[PA_COLOR_DIALOG_ELEMENT_G_SLIDER] = t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_SLIDER], t3gui_slider_proc, pos_x, pos_y, 256, pos_vy, 0, 0, 255, g, NULL, NULL, NULL);
	pos_x += 256 + space;
	t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_LIST_BOX], t3gui_edit_proc, pos_x, pos_y, 128, pos_vy, 0, 0, 3, 0, dp->edit_text[1], NULL, NULL);
	pos_y += pos_vy;

	/* B */
	dp->edit_text[2] = malloc(4);
	if(!dp->edit_text[2])
	{
		goto fail;
	}
	sprintf(dp->edit_text[2], "%d", b);
	pos_x = space;
	pos_vx = al_get_text_width(dp->theme->theme[PA_UI_THEME_LIST_BOX]->state[0].font[0], "R") + space;
	t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_LIST_BOX], t3gui_text_proc, pos_x, pos_y, 640, al_get_font_line_height(dp->theme->theme[PA_UI_THEME_LIST_BOX]->state[0].font[0]), 0, 0, 0, 0, "B", NULL, NULL);
	pos_x += pos_vx;
	dp->element[PA_COLOR_DIALOG_ELEMENT_B_SLIDER] = t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_SLIDER], t3gui_slider_proc, pos_x, pos_y, 256, pos_vy, 0, 0, 255, b, NULL, NULL, NULL);
	pos_x += 256 + space;
	t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_LIST_BOX], t3gui_edit_proc, pos_x, pos_y, 128, pos_vy, 0, 0, 3, 0, dp->edit_text[2], NULL, NULL);
	pos_y += pos_vy;

	/* A */
	dp->edit_text[3] = malloc(4);
	if(!dp->edit_text[3])
	{
		goto fail;
	}
	sprintf(dp->edit_text[3], "%d", a);
	pos_x = space;
	pos_vx = al_get_text_width(dp->theme->theme[PA_UI_THEME_LIST_BOX]->state[0].font[0], "R") + space;
	t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_LIST_BOX], t3gui_text_proc, pos_x, pos_y, 640, al_get_font_line_height(dp->theme->theme[PA_UI_THEME_LIST_BOX]->state[0].font[0]), 0, 0, 0, 0, "A", NULL, NULL);
	pos_x += pos_vx;
	dp->element[PA_COLOR_DIALOG_ELEMENT_A_SLIDER] = t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_SLIDER], t3gui_slider_proc, pos_x, pos_y, 256, pos_vy, 0, 0, 255, a, NULL, NULL, NULL);
	pos_x += 256 + space;
	t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_LIST_BOX], t3gui_edit_proc, pos_x, pos_y, 128, pos_vy, 0, 0, 3, 0, dp->edit_text[3], NULL, NULL);
	pos_y += pos_vy;

	return dp;

	fail:
	{
		if(dp)
		{
			pa_close_popup_dialog(dp);
		}
		return NULL;
	}
}

void pa_color_dialog_pre_logic(PA_POPUP_DIALOG * dp)
{
	old_r = dp->element[PA_COLOR_DIALOG_ELEMENT_R_SLIDER]->d2;
	strcpy(old_r_text, dp->edit_text[0]);
	old_g = dp->element[PA_COLOR_DIALOG_ELEMENT_G_SLIDER]->d2;
	strcpy(old_g_text, dp->edit_text[0]);
	old_b = dp->element[PA_COLOR_DIALOG_ELEMENT_B_SLIDER]->d2;
	strcpy(old_b_text, dp->edit_text[0]);
	old_a = dp->element[PA_COLOR_DIALOG_ELEMENT_A_SLIDER]->d2;
	strcpy(old_a_text, dp->edit_text[0]);
}

static void update_slider_or_text(T3GUI_ELEMENT * ep, int old, char * text, const char * old_text)
{
	if(ep->d2 != old)
	{
		sprintf(text, "%d", ep->d2);
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
	}
}

void pa_color_dialog_post_logic(PA_POPUP_DIALOG * dp)
{
	update_slider_or_text(dp->element[PA_COLOR_DIALOG_ELEMENT_R_SLIDER], old_r, dp->edit_text[0], old_r_text);
	update_slider_or_text(dp->element[PA_COLOR_DIALOG_ELEMENT_G_SLIDER], old_g, dp->edit_text[1], old_g_text);
	update_slider_or_text(dp->element[PA_COLOR_DIALOG_ELEMENT_B_SLIDER], old_b, dp->edit_text[2], old_b_text);
	update_slider_or_text(dp->element[PA_COLOR_DIALOG_ELEMENT_A_SLIDER], old_a, dp->edit_text[3], old_a_text);
	*edit_color = al_map_rgba(dp->element[PA_COLOR_DIALOG_ELEMENT_R_SLIDER]->d2, dp->element[PA_COLOR_DIALOG_ELEMENT_G_SLIDER]->d2, dp->element[PA_COLOR_DIALOG_ELEMENT_B_SLIDER]->d2, dp->element[PA_COLOR_DIALOG_ELEMENT_A_SLIDER]->d2);
}
