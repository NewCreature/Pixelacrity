#ifndef PA_CLIPBOARD_H
#define PA_CLIPBOARD_H

#include "canvas_editor.h"

void pa_free_clipboard(PA_CANVAS_EDITOR * cep);
bool pa_copy_bitmap_to_clipboard(PA_CANVAS_EDITOR * cep, PA_BITMAP_STACK * bp);
bool pa_copy_canvas_to_clipboard(PA_CANVAS_EDITOR * cep, int layer, int x, int y, int width, int height);
bool pa_add_layer_to_clipboard(PA_CANVAS_EDITOR * cep, int layer);
bool pa_remove_layer_from_clipboard(PA_CANVAS_EDITOR * cep, int layer);
void pa_paste_clipboard(PA_CANVAS_EDITOR * cep, int pos, int ox, int oy);

#endif
