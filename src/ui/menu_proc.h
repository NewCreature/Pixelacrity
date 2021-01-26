#ifndef QUIXEL_MENU_PROC_H
#define QUIXEL_MENU_PROC_H

int quixel_menu_file_new_helper(int id, void * data);
int quixel_menu_file_new(int id, void * data);
int quixel_menu_file_load(int id, void * data);
int quixel_menu_file_save(int id, void * data);
int quixel_menu_file_save_as(int id, void * data);
int quixel_menu_file_import(int id, void * data);
int quixel_menu_file_export(int id, void * data);
int quixel_menu_file_exit(int id, void * data);

#endif
