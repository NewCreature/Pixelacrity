#include "t3f/t3f.h"
#include "canvas.h"
#include "canvas_editor.h"

static void draw_pixel(QUIXEL_CANVAS_EDITOR * cep, int x, int y, ALLEGRO_COLOR color)
{
	ALLEGRO_TRANSFORM identity;
	ALLEGRO_STATE old_state;

	al_store_state(&old_state, ALLEGRO_STATE_TARGET_BITMAP | ALLEGRO_STATE_TRANSFORM);
	al_set_target_bitmap(cep->canvas->layer[cep->current_layer]->bitmap[y / cep->canvas->bitmap_size][x / cep->canvas->bitmap_size]);
	al_identity_transform(&identity);
	al_draw_pixel(x % cep->canvas->bitmap_size, y % cep->canvas->bitmap_size, color);
	al_restore_state(&old_state);
}

void quixel_tool_pixel_logic(QUIXEL_CANVAS_EDITOR * cep)
{
	if(!quixel_expand_canvas(cep->canvas, cep->current_layer, cep->view_x, cep->view_y))
	{
		printf("Tried to expand canvas but failed!\n");
	}
	draw_pixel(cep, cep->hover_x, cep->hover_y, cep->left_color);
}
