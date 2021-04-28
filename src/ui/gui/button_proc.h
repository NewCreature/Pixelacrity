#ifndef QUIXEL_BUTTON_PROC_H
#define QUIXEL_BUTTON_PROC_H

#include "t3gui/t3gui.h"

int quixel_tool_pixel_button_proc(T3GUI_ELEMENT * d, void *dp3);
int quixel_tool_line_button_proc(T3GUI_ELEMENT * d, void *dp3);
int quixel_tool_rectangle_button_proc(T3GUI_ELEMENT * d, void *dp3);
int quixel_tool_filled_rectangle_button_proc(T3GUI_ELEMENT * d, void *dp3);
int quixel_tool_oval_button_proc(T3GUI_ELEMENT * d, void *dp3);
int quixel_tool_filled_oval_button_proc(T3GUI_ELEMENT * d, void *dp3);
int quixel_tool_flood_fill_button_proc(T3GUI_ELEMENT * d, void *dp3);
int quixel_tool_dropper_button_proc(T3GUI_ELEMENT * d, void *dp3);
int quixel_tool_selection_button_proc(T3GUI_ELEMENT * d, void * dp3);

#endif
