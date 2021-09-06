#ifndef PA_BOX_H
#define PA_BOX_H

#define PA_BOX_HANDLE_TYPE_NONE         0
#define PA_BOX_HANDLE_TYPE_TOP_LEFT     1
#define PA_BOX_HANDLE_TYPE_TOP_RIGHT    2
#define PA_BOX_HANDLE_TYPE_BOTTOM_LEFT  3
#define PA_BOX_HANDLE_TYPE_BOTTOM_RIGHT 4
#define PA_BOX_HANDLE_TYPE_TOP          5
#define PA_BOX_HANDLE_TYPE_BOTTOM       6
#define PA_BOX_HANDLE_TYPE_LEFT         7
#define PA_BOX_HANDLE_TYPE_RIGHT        8
#define PA_BOX_HANDLE_TYPE_MOVE         9

#define PA_BOX_HANDLE_STATE_IDLE        0
#define PA_BOX_HANDLE_STATE_HOVER       1
#define PA_BOX_HANDLE_STATE_MOVING      2

#define PA_BOX_STATE_IDLE               0
#define PA_BOX_STATE_HOVER              1
#define PA_BOX_STATE_HOVER_HANDLE       2
#define PA_BOX_STATE_MOVING             3
#define PA_BOX_STATE_RESIZING           4
#define PA_BOX_STATE_DRAWING            5

typedef struct
{

	int type;
	int offset_x, offset_y;
	int screen_x, screen_y;

	int * link_x;
	int * link_y;

	int state;

} PA_BOX_HANDLE;

typedef struct
{

	int start_x, start_y;
	int middle_x, middle_y;
	int end_x, end_y;
	int width, height;
	PA_BOX_HANDLE handle[9];
	int hover_tick; // goes up each tick that the mouse is hovering over this box
	int hover_handle;
	int hover_x, hover_y;
	int click_tick;
	int click_x, click_y;
	int click_start_x, click_start_y;
	int state;

} PA_BOX;

void pa_initialize_box(PA_BOX * bp, int x, int y, int width, int height);
void pa_setup_box(PA_BOX * bp, int x, int y, int width, int height);
void pa_update_box_handles(PA_BOX * bp, int view_x, int view_y, int view_zoom);
void pa_box_logic(PA_BOX * bp, int view_x, int view_y, int view_zoom, int offset_x, int offset_y, bool snap, ALLEGRO_BITMAP * handle_bitmap);
void pa_box_render(PA_BOX * bp, int style, int view_x, int view_y, int view_zoom, int offset_x, int offset_y, ALLEGRO_COLOR color, ALLEGRO_BITMAP * handle_bitmap);

#endif
