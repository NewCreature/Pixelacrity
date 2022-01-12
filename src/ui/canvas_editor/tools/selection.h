#ifndef PA_TOOL_SELECTION_H
#define PA_TOOL_SELECTION_H

#include "ui/canvas_editor/canvas_editor.h"

void pa_tool_selection_render_layer_preview(PA_CANVAS_EDITOR * cep, int layer, ALLEGRO_BITMAP * scratch, int * offset_x, int * offset_y);
void pa_tool_selection_render_layer(PA_CANVAS_EDITOR * cep, int layer);
void pa_tool_selection_render(PA_CANVAS_EDITOR * cep);

#endif
