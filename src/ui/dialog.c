#include "t3f/t3f.h"
#include "t3gui/t3gui.h"
#include "t3gui/resource.h"
#include "dialog.h"

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

PA_DIALOG * pa_create_dialog(ALLEGRO_DISPLAY * dp, const char * theme_file, int w, int h, void * data)
{
	PA_DIALOG * dialog;
	ALLEGRO_STATE old_state;
	const char * val;
	int font_size = 0;
	int x, y;

	al_store_state(&old_state, ALLEGRO_STATE_TARGET_BITMAP);
	dialog = malloc(sizeof(PA_DIALOG));
	if(dialog)
	{
		memset(dialog, 0, sizeof(PA_DIALOG));
		get_center(t3f_display, w, h, &x, &y);
		if(!dp)
		{
			al_set_new_display_flags(ALLEGRO_WINDOWED);
			al_set_new_window_position(x, y);
			dialog->display = al_create_display(w, h);
			if(!dialog->display)
			{
				goto fail;
			}
			al_register_event_source(t3f_queue, al_get_display_event_source(dialog->display));
		}
		else
		{
			dialog->display = dp;
		}
		if(theme_file)
		{
			dialog->theme = pa_load_theme(theme_file);
			if(!dialog->theme)
			{
				goto fail;
			}
		}
		dialog->dialog = t3gui_create_dialog();
		if(!dialog->dialog)
		{
			goto fail;
		}
	}
	al_restore_state(&old_state);
	return dialog;

	fail:
	{
		al_restore_state(&old_state);
		if(dialog)
		{
			if(dialog->dialog)
			{
				t3gui_destroy_dialog(dialog->dialog);
			}
			if(dialog->theme)
			{
				pa_destroy_theme(dialog->theme);
			}
			if(dialog->display && dialog->display != dp)
			{
				al_destroy_display(dialog->display);
			}
			free(dialog);
		}
	}
	return NULL;
}

void pa_close_dialog(PA_DIALOG * dp)
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
	if(dp->display != t3f_display)
	{
		t3gui_unload_resources(dp->display, true);
	}
	pa_destroy_theme(dp->theme);
	if(dp->display != t3f_display)
	{
		al_destroy_display(dp->display);
	}
	free(dp);
}

void pa_track_dialog_element(PA_DIALOG * dp, T3GUI_ELEMENT * ep, int id)
{
	if(dp->element_count < PA_UI_MAX_DIALOG_ELEMENTS)
	{
		dp->element[dp->element_count] = ep;
		dp->element_id[id] = dp->element_count;
		dp->element_count++;
	}
}

T3GUI_ELEMENT * pa_get_dialog_element(PA_DIALOG * dp, int id)
{
	return dp->element[dp->element_id[id]];
}
