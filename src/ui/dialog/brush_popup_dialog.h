#ifndef PA_BRUSH_POPUP_DIALOG_H
#define PA_BRUSH_POPUP_DIALOG_H

#include "ui/dialog.h"
#include "ui/theme.h"
#include "ui/canvas_editor/canvas_editor.h"

PA_DIALOG * pa_create_brush_popup_dialog(PA_COLOR_INFO * color_info, float ox, float oy, int width, int height);

#endif
