#ifndef PA_CLIPBOARD_H
#define PA_CLIPBOARD_H

#include "canvas_editor.h"

bool pa_copy_bitmap_to_clipboard(QUIXEL_CANVAS_EDITOR * cep, ALLEGRO_BITMAP * bp);
bool pa_copy_canvas_to_clipboard(QUIXEL_CANVAS_EDITOR * cep, int layer, int x, int y, int width, int height);

#endif
