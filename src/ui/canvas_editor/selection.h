#ifndef PA_SELECTION_H
#define PA_SELECTION_H

#include "canvas_editor.h"
#include "modules/box.h"

void pa_clear_canvas_editor_selection(PA_CANVAS_EDITOR * cep);
bool pa_handle_float_canvas_editor_selection(PA_CANVAS_EDITOR * cep, PA_BOX * bp);
void pa_float_canvas_editor_selection(PA_CANVAS_EDITOR * cep, PA_BOX * bp);
void pa_handle_unfloat_canvas_editor_selection(PA_CANVAS_EDITOR * cep, PA_BOX * bp);
void pa_unfloat_canvas_editor_selection(PA_CANVAS_EDITOR * cep, PA_BOX * bp);
void pa_update_selection(PA_CANVAS_EDITOR * canvas_editor, T3GUI_ELEMENT * d);
bool pa_add_layer_to_selection(PA_CANVAS_EDITOR * cep, int layer);
bool pa_remove_layer_from_selection(PA_CANVAS_EDITOR * cep, int layer);

#endif
