#include "canvas_editor.h"
#include "modules/canvas/canvas_helpers.h"

bool pa_copy_bitmap_to_clipboard(PA_CANVAS_EDITOR * cep, ALLEGRO_BITMAP * bp)
{
	ALLEGRO_STATE old_state;

	al_store_state(&old_state, ALLEGRO_STATE_NEW_BITMAP_PARAMETERS);
	if(cep->clipboard.bitmap)
	{
		al_destroy_bitmap(cep->clipboard.bitmap);
	}
	al_set_new_bitmap_flags(ALLEGRO_NO_PREMULTIPLIED_ALPHA);
	cep->clipboard.bitmap = al_clone_bitmap(bp);
	al_restore_state(&old_state);
	if(cep->clipboard.bitmap)
	{
		return true;
	}
	return false;
}

bool pa_copy_canvas_to_clipboard(PA_CANVAS_EDITOR * cep, int layer, int x, int y, int width, int height)
{
	ALLEGRO_STATE old_state;

	al_store_state(&old_state, ALLEGRO_STATE_NEW_BITMAP_PARAMETERS);
	if(cep->clipboard.bitmap)
	{
		al_destroy_bitmap(cep->clipboard.bitmap);
	}
	al_set_new_bitmap_flags(ALLEGRO_NO_PREMULTIPLIED_ALPHA);
	cep->clipboard.bitmap = al_create_bitmap(width, height);
	al_restore_state(&old_state);
	if(cep->clipboard.bitmap)
	{
		pa_render_canvas_to_bitmap(cep->canvas, layer, layer + 1, x, y, width, height, 0, cep->clipboard.bitmap);
		cep->clipboard.x = x, cep->clipboard.y = y;
		return true;
	}
	return false;
}
