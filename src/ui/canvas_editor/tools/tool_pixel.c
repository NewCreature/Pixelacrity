#include "t3f/t3f.h"
#include "modules/canvas/canvas.h"
#include "ui/canvas_editor/canvas_editor.h"

static void draw_pixel(QUIXEL_CANVAS_EDITOR * cep, int x, int y, ALLEGRO_COLOR color)
{
	ALLEGRO_TRANSFORM identity;
	ALLEGRO_STATE old_state;

	al_store_state(&old_state, ALLEGRO_STATE_TARGET_BITMAP | ALLEGRO_STATE_TRANSFORM);
	al_set_target_bitmap(cep->scratch_bitmap);
	al_identity_transform(&identity);
	al_put_pixel(x, y, color);
	al_restore_state(&old_state);
}

void quixel_tool_pixel_logic(QUIXEL_CANVAS_EDITOR * cep)
{
	int x, y;
	int dx, dy;

	dx = cep->tool_offset_x - (cep->hover_x - cep->view_x);
	dy = cep->tool_offset_y - (cep->hover_y - cep->view_y);
	cep->tool_offset_x = cep->hover_x - cep->scratch_offset_x;
	cep->tool_offset_y = cep->hover_y - cep->scratch_offset_y;
	x = cep->tool_offset_x;
	y = cep->tool_offset_y;
	if(x < cep->tool_left)
	{
		cep->tool_left = x;
	}
	if(x > cep->tool_right)
	{
		cep->tool_right = x;
	}
	if(y < cep->tool_top)
	{
		cep->tool_top = y;
	}
	if(y > cep->tool_bottom)
	{
		cep->tool_bottom = y;
	}
	if(!quixel_expand_canvas(cep->canvas, cep->current_layer, x, y))
	{
		printf("Tried to expand canvas but failed!\n");
	}
	draw_pixel(cep, x, y, cep->click_color);
}
