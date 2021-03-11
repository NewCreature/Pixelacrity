#ifndef QUIXEL_BOX_H
#define QUIXEL_BOX_H

#define QUIXEL_BOX_HANDLE_TYPE_NONE         0
#define QUIXEL_BOX_HANDLE_TYPE_TOP_LEFT     1
#define QUIXEL_BOX_HANDLE_TYPE_TOP_RIGHT    2
#define QUIXEL_BOX_HANDLE_TYPE_BOTTOM_LEFT  3
#define QUIXEL_BOX_HANDLE_TYPE_BOTTOM_RIGHT 4
#define QUIXEL_BOX_HANDLE_TYPE_TOP          5
#define QUIXEL_BOX_HANDLE_TYPE_BOTTOM       6
#define QUIXEL_BOX_HANDLE_TYPE_LEFT         7
#define QUIXEL_BOX_HANDLE_TYPE_RIGHT        8
#define QUIXEL_BOX_HANDLE_TYPE_MOVE         9

#define QUIXEL_BOX_HANDLE_STATE_IDLE        0
#define QUIXEL_BOX_HANDLE_STATE_HOVER       1
#define QUIXEL_BOX_HANDLE_STATE_MOVING      2

typedef struct
{

	int type;
	int offset_x, offset_y;
	int screen_x, screen_y;

	int * link_x;
	int * link_y;

	int state;

} QUIXEL_BOX_HANDLE;

typedef struct
{

	int start_x, start_y;
	int middle_x, middle_y;
	int end_x, end_y;
	int width, height;
	ALLEGRO_BITMAP * bitmap;
	QUIXEL_BOX_HANDLE handle[9];
	int hover_tick; // goes up each tick that the mouse is hovering over this box
	int hover_handle;

} QUIXEL_BOX;

void quixel_initialize_box(QUIXEL_BOX * bp, int x, int y, int width, int height, ALLEGRO_BITMAP * handle_bitmap);
void quixel_update_box_handles(QUIXEL_BOX * bp, int view_x, int view_y, int view_zoom);
void quixel_box_logic(QUIXEL_BOX * bp, int view_x, int view_y, int view_zoom, int offset_x, int offset_y);
void quixel_box_render(QUIXEL_BOX * bp, int style, int view_x, int view_y, int view_zoom, int offset_x, int offset_y);

#endif
