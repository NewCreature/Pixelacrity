#ifndef PA_BUTTON_PROC_H
#define PA_BUTTON_PROC_H

#include "t3gui/t3gui.h"

int pa_tool_pixel_button_proc(T3GUI_ELEMENT * d, void *dp3);
int pa_tool_line_button_proc(T3GUI_ELEMENT * d, void *dp3);
int pa_tool_rectangle_button_proc(T3GUI_ELEMENT * d, void *dp3);
int pa_tool_filled_rectangle_button_proc(T3GUI_ELEMENT * d, void *dp3);
int pa_tool_oval_button_proc(T3GUI_ELEMENT * d, void *dp3);
int pa_tool_filled_oval_button_proc(T3GUI_ELEMENT * d, void *dp3);
int pa_tool_flood_fill_button_proc(T3GUI_ELEMENT * d, void *dp3);
int pa_tool_dropper_button_proc(T3GUI_ELEMENT * d, void *dp3);
int pa_tool_eraser_button_proc(T3GUI_ELEMENT * d, void *dp3);
int pa_tool_selection_button_proc(T3GUI_ELEMENT * d, void * dp3);
int pa_layer_add_button_proc(T3GUI_ELEMENT * d, void * dp3);
int pa_layer_remove_button_proc(T3GUI_ELEMENT * d, void * dp3);

#endif
