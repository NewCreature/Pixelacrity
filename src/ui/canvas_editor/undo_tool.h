#ifndef QUIXEL_UNDO_TOOL_H
#define QUIXEL_UNDO_TOOL_H

bool quixel_make_tool_undo(QUIXEL_CANVAS_EDITOR * cep, const char * action, int layer, int x, int y, int width, int height, const char * fn);
bool quixel_make_tool_redo(QUIXEL_CANVAS_EDITOR * cep, const char * action, int tool, int layer, int x, int y, int width, int height, const char * fn);
bool quixel_apply_tool_undo(QUIXEL_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp, const char * action, bool revert);
bool quixel_apply_tool_redo(QUIXEL_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp, const char * action);

#endif
