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
#define PA_TOOL_ERASER           7
#define PA_TOOL_DROPPER          8
#define PA_TOOL_SELECTION        9

#define PA_TOOL_STATE_OFF        0
#define PA_TOOL_STATE_DRAWING    1
#define PA_TOOL_STATE_EDITING    2

#define PA_COLOR_PICKER_SHADES 11
#define PA_COLOR_PICKER_SCALE  12

#define PA_COLOR_PALETTE_MAX  128

typedef struct
{

	ALLEGRO_BITMAP ** bitmap;
	int layer_max;
	int layer;
	PA_BOX box;

	bool moving;
	int pin_x, pin_y;

} PA_SELECTION_DATA;

typedef struct
{

	ALLEGRO_BITMAP ** bitmap;
	int layer_max;
	int layer;
	int x, y;

} PA_CLIPBOARD_DATA;

typedef struct
{

	ALLEGRO_COLOR base_color;
	ALLEGRO_COLOR last_base_color;
	ALLEGRO_COLOR color;
	ALLEGRO_COLOR last_color;
	ALLEGRO_COLOR shade_color;
	ALLEGRO_COLOR last_shade_color;
	ALLEGRO_COLOR alpha_color;
	ALLEGRO_COLOR last_alpha_color;
	T3GUI_ELEMENT * shade_slider_element;
	int old_shade_slider_d2;
	T3GUI_ELEMENT * alpha_slider_element;
	int old_alpha_slider_d2;

} PA_COLOR_INFO;

typedef struct
{

	ALLEGRO_SHADER * standard_shader;
	ALLEGRO_SHADER * premultiplied_alpha_shader;
	ALLEGRO_SHADER * conditional_copy_shader;
	ALLEGRO_SHADER * solid_shader;
	ALLEGRO_BITMAP * tool_bitmap;
	ALLEGRO_BITMAP * scratch_bitmap;
	ALLEGRO_BITMAP * peg_bitmap;
	ALLEGRO_BITMAP * multilayer_bitmap;
	ALLEGRO_BITMAP * singlelayer_bitmap;
	ALLEGRO_BITMAP * brush;
	PA_CLIPBOARD_DATA clipboard;
	int scratch_offset_x; // offset of scratch in relation to view
	int scratch_offset_y;
	int tool_offset_x;
	int tool_offset_y;
	int tool_top, tool_bottom, tool_left, tool_right;
	float view_fx, view_fy;
	int view_x, view_y;
	int view_width, view_height;
	int view_zoom;
	bool view_isolate;
	int hover_x, hover_y;
	int last_hover_x, last_hover_y;
	int click_x, click_y, click_button;
	ALLEGRO_COLOR click_color;
	ALLEGRO_COLOR click_outline_color;
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
	PA_COLOR_INFO left_color;
	PA_COLOR_INFO right_color;
	ALLEGRO_COLOR pick_color[PA_COLOR_PICKER_SHADES];
	ALLEGRO_COLOR start_color;
	ALLEGRO_COLOR hover_color;
	ALLEGRO_COLOR palette[PA_COLOR_PALETTE_MAX];

	T3GUI_ELEMENT * editor_element;
	bool simulate_mouse_move;

	int signal;
	PA_CANVAS * canvas;
	char canvas_path[4096];
	char * export_path;

	int undo_count;
	char undo_name[256];
	int redo_count;
	char redo_name[256];

	int backup_tick;

	ALLEGRO_CONFIG * config;

} PA_CANVAS_EDITOR;

void pa_reset_canvas_editor(PA_CANVAS_EDITOR * cep);
PA_CANVAS_EDITOR * pa_create_canvas_editor(PA_CANVAS * cp);
void pa_destroy_canvas_editor(PA_CANVAS_EDITOR * cep);

bool pa_load_canvas_editor_state(PA_CANVAS_EDITOR * cep, const char * fn);
bool pa_save_canvas_editor_state(PA_CANVAS_EDITOR * cep, const char * fn);

void pa_set_color(PA_COLOR_INFO * cip, ALLEGRO_COLOR color);

void pa_center_canvas_editor(PA_CANVAS_EDITOR * cep, int frame);
void pa_set_canvas_editor_zoom(PA_CANVAS_EDITOR * cep, int level);
void pa_shift_canvas_editor_variables(PA_CANVAS_EDITOR * cep, int ox, int oy);
void pa_select_canvas_editor_tool(PA_CANVAS_EDITOR * cep, int tool);
void pa_select_canvas_editor_layer(PA_CANVAS_EDITOR * cep, int layer);
bool pa_import_image(PA_CANVAS_EDITOR * cep, const char * fn);
void pa_initialize_tool_variables(PA_CANVAS_EDITOR * cep);
void pa_update_tool_variables(PA_CANVAS_EDITOR * cep);
void pa_set_tool_boundaries(PA_CANVAS_EDITOR * cep, int start_x, int start_y, int end_x, int end_y);
void pa_canvas_editor_update_pick_colors(PA_CANVAS_EDITOR * cep);

#endif
