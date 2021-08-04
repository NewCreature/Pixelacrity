#ifndef PA_CLIPBOARD_H
#define PA_CLIPBOARD_H

#include "canvas_editor.h"

void pa_free_clipboard(PA_CANVAS_EDITOR * cep);
bool pa_copy_bitmap_to_clipboard(PA_CANVAS_EDITOR * cep, ALLEGRO_BITMAP ** bp, int max);
bool pa_copy_canvas_to_clipboard(PA_CANVAS_EDITOR * cep, int layer, int x, int y, int width, int height);

#endif
