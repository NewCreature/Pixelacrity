#ifndef PA_CANVAS_EDITOR_H
#define PA_CANVAS_EDITOR_H

#include "t3gui/t3gui.h"
#include "modules/canvas/canvas.h"
#include "modules/box.h"

#define PA_CANVAS_EDITOR_SIGNAL_NONE         0
#define PA_CANVAS_EDITOR_SIGNAL_DELETE_LAYER 1

#define PA_BACKUP_INTERVAL (10 * 60 * 60) // 10 minutes

#define PA_TOOL_PIXEL            0
#define PA_TOOL_LINE             1
#define PA_TOOL_RECTANGLE        2
#define PA_TOOL_FILLED_RECTANGLE 3
#define PA_TOOL_OVAL             4
#define PA_TOOL_FILLED_OVAL      5
#define PA_TOOL_FLOOD_FILL       6
#define PA_TOOL_DROPPER          7
#define PA_TOOL_SELECTION        8
#define PA_TOOL_ERASER           9

#define PA_TOOL_STATE_OFF        0
#define PA_TOOL_STATE_DRAWING    1
#define PA_TOOL_STATE_EDITING    2

#define PA_COLOR_PICKER_SHADES 11
#define PA_COLOR_PICKER_SCALE  12

#define PA_COLOR_PALETTE_MAX  128

typedef struct
{

	ALLEGRO_BITMAP * bitmap;
	ALLEGRO_BITMAP * combined_bitmap;
	int layer;
	PA_BOX box;

	bool moving;
	int pin_x, pin_y;

} PA_SELECTION_DATA;

typedef struct
{

	ALLEGRO_BITMAP * bitmap;

} PA_CLIPBOARD_DATA;

typedef struct
{

	ALLEGRO_BITMAP * scratch_bitmap;
	ALLEGRO_BITMAP * peg_bitmap;
	PA_CLIPBOARD_DATA clipboard;
	int scratch_offset_x; // offset of scratch in relation to view
	int scratch_offset_y;
	int tool_offset_x;
	int tool_offset_y;
	int tool_top, tool_bottom, tool_left, tool_right;
	float view_scroll_speed;
	float view_fx, view_fy;
	int view_x, view_y;
	int view_zoom;
	int hover_x, hover_y;
	int last_hover_x, last_hover_y;
	int click_x, click_y, click_button;
	ALLEGRO_COLOR click_color;
	int release_x, release_y;
	int shift_x, shift_y;
	int current_layer;
	int current_frame;
	int current_tool;
	ALLEGRO_SYSTEM_MOUSE_CURSOR old_cursor;
	ALLEGRO_SYSTEM_MOUSE_CURSOR current_cursor;
	int tool_state;
	PA_SELECTION_DATA selection;
	int modified;
	bool update_title;
	ALLEGRO_COLOR left_base_color;
	ALLEGRO_COLOR last_left_base_color;
	ALLEGRO_COLOR left_color;
	ALLEGRO_COLOR last_left_color;
	ALLEGRO_COLOR left_shade_color;
	ALLEGRO_COLOR last_left_shade_color;
	ALLEGRO_COLOR left_alpha_color;
	ALLEGRO_COLOR last_left_alpha_color;
	ALLEGRO_COLOR right_base_color;
	ALLEGRO_COLOR last_right_base_color;
	ALLEGRO_COLOR right_color;
	ALLEGRO_COLOR last_right_color;
	ALLEGRO_COLOR right_shade_color;
	ALLEGRO_COLOR last_right_shade_color;
	ALLEGRO_COLOR right_alpha_color;
	ALLEGRO_COLOR last_right_alpha_color;
	ALLEGRO_COLOR pick_color[PA_COLOR_PICKER_SHADES];
	ALLEGRO_COLOR start_color;
	ALLEGRO_COLOR hover_color;
	ALLEGRO_COLOR palette[PA_COLOR_PALETTE_MAX];

	T3GUI_ELEMENT * left_shade_slider_element;
	T3GUI_ELEMENT * left_alpha_slider_element;
	T3GUI_ELEMENT * right_shade_slider_element;
	T3GUI_ELEMENT * right_alpha_slider_element;
	T3GUI_ELEMENT * editor_element;

	int signal;
	PA_CANVAS * canvas;
	char canvas_path[4096];

	int undo_count;
	char undo_name[256];
	int redo_count;
	char redo_name[256];

	int backup_tick;

} PA_CANVAS_EDITOR;

PA_CANVAS_EDITOR * pa_create_canvas_editor(PA_CANVAS * cp);
void pa_destroy_canvas_editor(PA_CANVAS_EDITOR * cep);

void pa_center_canvas_editor(PA_CANVAS_EDITOR * cep, int frame);
bool pa_handle_float_canvas_editor_selection(PA_CANVAS_EDITOR * cep, PA_BOX * bp);
void pa_float_canvas_editor_selection(PA_CANVAS_EDITOR * cep, PA_BOX * bp);
void pa_handle_unfloat_canvas_editor_selection(PA_CANVAS_EDITOR * cep, PA_BOX * bp);
void pa_unfloat_canvas_editor_selection(PA_CANVAS_EDITOR * cep, PA_BOX * bp);
void pa_shift_canvas_editor_variables(PA_CANVAS_EDITOR * cep, int ox, int oy);
void pa_select_canvas_editor_tool(PA_CANVAS_EDITOR * cep, int tool);
bool pa_import_image(PA_CANVAS_EDITOR * cep, const char * fn);

#endif
