#include "t3f/t3f.h"
#include "t3f/file_utils.h"
#include "instance.h"

static char window_title_text[1024];
static bool update_title = false;

static void pa_set_window_title(PA_CANVAS_EDITOR * cep)
{
	const char * fn;

	fn = t3f_get_path_filename(cep->canvas_path);
	if(fn)
	{
		sprintf(window_title_text, T3F_APP_TITLE " - %s%s", fn, cep->modified ? "*" : "");
	}
	else
	{
		sprintf(window_title_text, T3F_APP_TITLE " - Untitled%s", cep->modified ? "*" : "");
	}
	al_set_window_title(t3f_display, window_title_text);
}

void pa_set_window_message(const char * message)
{
	char buf[1024];

	if(message && strlen(message))
	{
		sprintf(buf, "%s (%s)", window_title_text, message);
		al_set_window_title(t3f_display, buf);
	}
	update_title = true;
}

void pa_update_window_title(PA_CANVAS_EDITOR * cep)
{
	if(update_title)
	{
		pa_set_window_title(cep);
		update_title = false;
	}
}
