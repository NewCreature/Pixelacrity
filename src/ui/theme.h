#ifndef PA_UI_THEME_H
#define PA_UI_THEME_H

#define PA_UI_THEME_MAX        16

#define PA_UI_THEME_BOX         0
#define PA_UI_THEME_TOP_PANE    1
#define PA_UI_THEME_LEFT_PANE   2
#define PA_UI_THEME_RIGHT_PANE  3
#define PA_UI_THEME_BOTTOM_PANE 4
#define PA_UI_THEME_BUTTON      5
#define PA_UI_THEME_LIST_BOX    6

typedef struct
{

	ALLEGRO_PATH * path;
	ALLEGRO_CONFIG * config;
	T3GUI_THEME * theme[PA_UI_THEME_MAX];

} PA_UI_THEME;

PA_UI_THEME * pa_load_theme(const char * fn);
void pa_destroy_theme(PA_UI_THEME * tp);

ALLEGRO_BITMAP * pa_load_theme_bitmap(PA_UI_THEME * tp, const char * name);
T3GUI_THEME * pa_load_theme_theme(PA_UI_THEME * tp, const char * name);
int pa_get_theme_int(PA_UI_THEME * tp, const char * name, int fallback);

#endif
