#ifndef QUIXEL_UNDO_H
#define QUIXEL_UNDO_H

#define QUIXEL_UNDO_TYPE_TOOL       0
#define QUIXEL_UNDO_TYPE_FRAME      1
#define QUIXEL_UNDO_FLOAT_SELECTION 2

bool quixel_make_tool_undo(QUIXEL_CANVAS_EDITOR * cep, const char * action, int layer, int x, int y, int width, int height, const char * fn);
bool quixel_make_tool_redo(QUIXEL_CANVAS_EDITOR * cep, const char * action, int layer, int x, int y, int width, int height, const char * fn);

const char * quixel_get_undo_name(const char * fn, char * out, int out_size);
const char * quixel_get_undo_path(const char * base, int count, char * out, int out_size);
void quixel_update_undo_name(QUIXEL_CANVAS_EDITOR * cep);
void quixel_update_redo_name(QUIXEL_CANVAS_EDITOR * cep);
bool quixel_apply_undo(QUIXEL_CANVAS_EDITOR * cep, const char * fn, bool redo, bool revert);
bool quixel_apply_redo(QUIXEL_CANVAS_EDITOR * cep, const char * fn);
void quixel_undo_clean_up(QUIXEL_CANVAS_EDITOR * cep);

#endif
