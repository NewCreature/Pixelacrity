#ifndef PA_UNDO_TOOL_H
#define PA_UNDO_TOOL_H

bool pa_make_tool_undo(PA_CANVAS_EDITOR * cep, const char * action, int layer, int x, int y, int width, int height, const char * fn);
bool pa_make_tool_redo(PA_CANVAS_EDITOR * cep, const char * action, int tool, int layer, int x, int y, int width, int height, const char * fn);
bool pa_apply_tool_undo(PA_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp, const char * action, bool revert);
bool pa_apply_tool_redo(PA_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp, const char * action);

#endif
