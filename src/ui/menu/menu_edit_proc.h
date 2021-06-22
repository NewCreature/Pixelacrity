#ifndef PA_MENU_EDIT_PROC_H
#define PA_MENU_EDIT_PROC_H

int pa_menu_edit_undo(int id, void * data);
int pa_menu_edit_redo(int id, void * data);
int pa_menu_edit_cut(int id, void * data);
int pa_menu_edit_copy(int id, void * data);
int pa_menu_edit_paste(int id, void * data);
int pa_menu_edit_delete(int id, void * data);

#endif
