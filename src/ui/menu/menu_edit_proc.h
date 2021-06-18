#ifndef QUIXEL_MENU_EDIT_PROC_H
#define QUIXEL_MENU_EDIT_PROC_H

int quixel_menu_edit_undo(int id, void * data);
int quixel_menu_edit_redo(int id, void * data);
int quixel_menu_edit_cut(int id, void * data);
int quixel_menu_edit_copy(int id, void * data);
int quixel_menu_edit_paste(int id, void * data);
int quixel_menu_edit_delete(int id, void * data);

#endif
