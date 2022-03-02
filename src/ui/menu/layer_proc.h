#ifndef PA_MENU_LAYER_PROC_H
#define PA_MENU_LAYER_PROC_H

int pa_menu_layer_add(int id, void * data);
int pa_menu_layer_delete(int id, void * data);
int pa_menu_layer_move_up(int id, void * data);
int pa_menu_layer_move_down(int id, void * data);
int pa_menu_layer_previous(int id, void * data);
int pa_menu_layer_next(int id, void * data);
int pa_menu_layer_toggle_visibility(int id, void * data);

#endif
