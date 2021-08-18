#ifndef PA_UI_MENU_TOOL_PROC_H
#define PA_UI_MENU_TOOL_PROC_H

int pa_menu_tool_brush_reset(int id, void * data);
int pa_menu_tool_brush_grab_from_selection(int id, void * data);
int pa_menu_tool_brush_load(int id, void * data);

int pa_menu_tool_pixel(int id, void * data);
int pa_menu_tool_line(int id, void * data);
int pa_menu_tool_rectangle(int id, void * data);
int pa_menu_tool_filled_rectangle(int id, void * data);
int pa_menu_tool_oval(int id, void * data);
int pa_menu_tool_filled_oval(int id, void * data);
int pa_menu_tool_flood_fill(int id, void * data);
int pa_menu_tool_eraser(int id, void * data);
int pa_menu_tool_dropper(int id, void * data);
int pa_menu_tool_selector(int id, void * data);

#endif
