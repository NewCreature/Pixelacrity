#ifndef PA_BRUSH_POPUP_DIALOG_H
#define PA_BRUSH_POPUP_DIALOG_H

#include "ui/dialog.h"
#include "ui/theme.h"
#include "ui/canvas_editor/canvas_editor.h"

#define PA_UI_BITMAP_BRUSH_SQUARE          0
#define PA_UI_BITMAP_BRUSH_CIRCLE          1
#define PA_UI_BITMAP_BRUSH_VLINE           2
#define PA_UI_BITMAP_BRUSH_DLINE_UR        3
#define PA_UI_BITMAP_BRUSH_HLINE           4
#define PA_UI_BITMAP_BRUSH_DLINE_DR        5
#define PA_UI_BITMAP_BRUSH_DIAMOND         6

#define PA_BRUSH_DIALOG_ELEMENT_BRUSH       0
#define PA_BRUSH_DIALOG_ELEMENT_SIZE_SLIDER 1

PA_DIALOG * pa_create_brush_popup_dialog(PA_CANVAS_EDITOR * cep, float ox, float oy, int width, int height);

void pa_brush_dialog_pre_logic(PA_DIALOG * dp);
void pa_brush_dialog_post_logic(PA_DIALOG * dp, void * dp3);

#endif
