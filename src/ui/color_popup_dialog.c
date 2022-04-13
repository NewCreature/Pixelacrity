#include "t3gui/t3gui.h"
#include "popup_dialog.h"

PA_POPUP_DIALOG * pa_create_color_editor_popup_dialog(ALLEGRO_COLOR color)
{
	PA_POPUP_DIALOG * dp;

	dp = pa_create_popup_dialog(al_get_config_value(t3f_config, "App Data", "theme"), 640, 480, NULL);
	if(!dp)
	{
		goto fail;
	}
	t3gui_dialog_add_element(dp->dialog, dp->theme->theme[PA_UI_THEME_BOX], t3gui_box_proc, 0, 0, 640, 480, 0, 0, 0, 0, NULL, NULL, NULL);

	return dp;

	fail:
	{
		return NULL;
	}
}
