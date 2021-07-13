#ifndef PA_MENU_EDIT_PROC_H
#define PA_MENU_EDIT_PROC_H

int pa_menu_edit_undo(int id, void * data);
int pa_menu_edit_redo(int id, void * data);
int pa_menu_edit_cut(int id, void * data);
int pa_menu_edit_copy(int id, void * data);
int pa_menu_edit_paste(int id, void * data);
int pa_menu_edit_paste_in_place(int id, void * data);
int pa_menu_edit_flip_horizontal(int id, void * data);
int pa_menu_edit_flip_vertical(int id, void * data);
int pa_menu_edit_turn_clockwise(int id, void * data);
int pa_menu_edit_turn_counter_clockwise(int id, void * data);
int pa_menu_edit_delete(int id, void * data);
int pa_menu_edit_float_selection(int id, void * data);
int pa_menu_edit_unfloat_selection(int id, void * data);

#endif
