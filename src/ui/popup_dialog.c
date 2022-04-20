#include "t3f/t3f.h"
#include "t3gui/t3gui.h"
#include "t3gui/resource.h"
#include "popup_dialog.h"

static void get_center(ALLEGRO_DISPLAY * dp, int w, int h, int * x, int * y)
{
	int dx, dy;

	al_get_window_position(dp, &dx, &dy);
	if(x)
	{
		*x = dx + al_get_display_width(dp) / 2 - w / 2;
	}
	if(y)
	{
		*y = dy + al_get_display_height(dp) / 2 - h / 2;
	}
}

PA_POPUP_DIALOG * pa_create_popup_dialog(const char * theme_file, int w, int h, void * data)
{
	PA_POPUP_DIALOG * popup_dialog;
	ALLEGRO_STATE old_state;
	const char * val;
	int font_size = 0;
	int x, y;

	al_store_state(&old_state, ALLEGRO_STATE_TARGET_BITMAP);
	popup_dialog = malloc(sizeof(PA_POPUP_DIALOG));
	if(popup_dialog)
	{
		memset(popup_dialog, 0, sizeof(PA_POPUP_DIALOG));
		al_set_new_display_flags(ALLEGRO_WINDOWED);
		get_center(t3f_display, w, h, &x, &y);
		al_set_new_window_position(x, y);
		popup_dialog->display = al_create_display(w, h);
		if(!popup_dialog->display)
		{
			goto fail;
		}
		al_register_event_source(t3f_queue, al_get_display_event_source(popup_dialog->display));
		al_set_target_bitmap(al_get_backbuffer(popup_dialog->display));
		val = al_get_config_value(t3f_config, "Settings", "font_size_override");
		if(val)
		{
			font_size = atoi(val);
		}
		if(theme_file)
		{
			popup_dialog->theme = pa_load_theme(theme_file);
			if(!popup_dialog->theme)
			{
				goto fail;
			}
		}
		popup_dialog->dialog = t3gui_create_dialog();
		if(!popup_dialog->dialog)
		{
			goto fail;
		}
	}
	al_restore_state(&old_state);
	return popup_dialog;

	fail:
	{
		al_restore_state(&old_state);
		if(popup_dialog)
		{
			if(popup_dialog->dialog)
			{
				t3gui_destroy_dialog(popup_dialog->dialog);
			}
			if(popup_dialog->theme)
			{
				pa_destroy_theme(popup_dialog->theme);
			}
			if(popup_dialog->display)
			{
				al_destroy_display(popup_dialog->display);
			}
			free(popup_dialog);
		}
	}
	return NULL;
}

void pa_close_popup_dialog(PA_POPUP_DIALOG * dp)
{
	int i;

	for(i = 0; i < PA_UI_MAX_DIALOG_EDIT_BOXES; i++)
	{
		if(dp->edit_text[i])
		{
			free(dp->edit_text[i]);
		}
	}
	t3gui_close_dialog(dp->dialog);
	t3gui_destroy_dialog(dp->dialog);
	t3gui_unload_resources(dp->display, true);
	pa_destroy_theme(dp->theme);
	al_destroy_display(dp->display);
	free(dp);
}
