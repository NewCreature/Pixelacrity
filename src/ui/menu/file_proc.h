#ifndef PA_MENU_FILE_PROC_H
#define PA_MENU_FILE_PROC_H

#include "instance.h"

int pa_menu_file_new(int id, void * data);
bool pa_handle_load_canvas(APP_INSTANCE * app, const char * file_path);
int pa_menu_file_load(int id, void * data);
int pa_menu_file_save(int id, void * data);
int pa_menu_file_save_as(int id, void * data);
int pa_menu_file_resave_image(int id, void * data);
int pa_menu_file_import(int id, void * data);
int pa_menu_file_export(int id, void * data);
int pa_menu_file_reexport(int id, void * data);
int pa_menu_file_reexport_all(int id, void * data);
int pa_menu_file_load_palette(int id, void * data);
int pa_menu_file_save_palette(int id, void * data);
int pa_menu_file_exit(int id, void * data);

#endif
