#ifndef PA_BUTTON_PROC_H
#define PA_BUTTON_PROC_H

#include "t3gui/t3gui.h"

int pa_toolbar_new_button_proc(T3GUI_ELEMENT * d, void * dp3);
int pa_toolbar_open_button_proc(T3GUI_ELEMENT * d, void * dp3);
int pa_toolbar_save_button_proc(T3GUI_ELEMENT * d, void * dp3);
int pa_toolbar_import_button_proc(T3GUI_ELEMENT * d, void * dp3);
int pa_toolbar_export_button_proc(T3GUI_ELEMENT * d, void * dp3);
int pa_toolbar_undo_button_proc(T3GUI_ELEMENT * d, void * dp3);
int pa_toolbar_redo_button_proc(T3GUI_ELEMENT * d, void * dp3);
int pa_toolbar_cut_button_proc(T3GUI_ELEMENT * d, void * dp3);
int pa_toolbar_copy_button_proc(T3GUI_ELEMENT * d, void * dp3);
int pa_toolbar_paste_button_proc(T3GUI_ELEMENT * d, void * dp3);
int pa_toolbar_turn_cw_button_proc(T3GUI_ELEMENT * d, void * dp3);
int pa_toolbar_turn_ccw_button_proc(T3GUI_ELEMENT * d, void * dp3);
int pa_toolbar_flip_h_button_proc(T3GUI_ELEMENT * d, void * dp3);
int pa_toolbar_flip_v_button_proc(T3GUI_ELEMENT * d, void * dp3);
int pa_tool_composite_button_proc(T3GUI_ELEMENT * d, void * dp3);
int pa_tool_outline_button_proc(T3GUI_ELEMENT * d, void * dp3);
int pa_tool_pixel_button_proc(T3GUI_ELEMENT * d, void * dp3);
int pa_tool_line_button_proc(T3GUI_ELEMENT * d, void * dp3);
int pa_tool_rectangle_button_proc(T3GUI_ELEMENT * d, void * dp3);
int pa_tool_filled_rectangle_button_proc(T3GUI_ELEMENT * d, void * dp3);
int pa_tool_oval_button_proc(T3GUI_ELEMENT * d, void * dp3);
int pa_tool_filled_oval_button_proc(T3GUI_ELEMENT * d, void * dp3);
int pa_tool_flood_fill_button_proc(T3GUI_ELEMENT * d, void * dp3);
int pa_tool_replace_button_proc(T3GUI_ELEMENT * d, void * dp3);
int pa_tool_dropper_button_proc(T3GUI_ELEMENT * d, void * dp3);
int pa_tool_eraser_button_proc(T3GUI_ELEMENT * d, void * p3);
int pa_tool_selection_button_proc(T3GUI_ELEMENT * d, void * dp3);
int pa_tool_frame_button_proc(T3GUI_ELEMENT * d, void * dp3);
int pa_layer_add_button_proc(T3GUI_ELEMENT * d, void * dp3);
int pa_layer_remove_button_proc(T3GUI_ELEMENT * d, void * dp3);

int pa_color_okay_button_proc(T3GUI_ELEMENT * d, void * dp3);
int pa_color_cancel_button_proc(T3GUI_ELEMENT * d, void * dp3);
int pa_color_r_up_button_proc(T3GUI_ELEMENT * d, void * dp3);
int pa_color_r_down_button_proc(T3GUI_ELEMENT * d, void * dp3);
int pa_color_g_up_button_proc(T3GUI_ELEMENT * d, void * dp3);
int pa_color_g_down_button_proc(T3GUI_ELEMENT * d, void * dp3);
int pa_color_b_up_button_proc(T3GUI_ELEMENT * d, void * dp3);
int pa_color_b_down_button_proc(T3GUI_ELEMENT * d, void * dp3);
int pa_color_a_up_button_proc(T3GUI_ELEMENT * d, void * dp3);
int pa_color_a_down_button_proc(T3GUI_ELEMENT * d, void * dp3);

int pa_brush_square_proc(T3GUI_ELEMENT * d, void * dp3);
int pa_brush_circle_proc(T3GUI_ELEMENT * d, void * dp3);
int pa_brush_vline_proc(T3GUI_ELEMENT * d, void * dp3);
int pa_brush_dline_ur_proc(T3GUI_ELEMENT * d, void * dp3);
int pa_brush_hline_proc(T3GUI_ELEMENT * d, void * dp3);
int pa_brush_dline_dr_proc(T3GUI_ELEMENT * d, void * dp3);
int pa_brush_diamond_proc(T3GUI_ELEMENT * d, void * dp3);
int pa_brush_okay_button_proc(T3GUI_ELEMENT * d, void * dp3);
int pa_brush_cancel_button_proc(T3GUI_ELEMENT * d, void * dp3);

#endif
