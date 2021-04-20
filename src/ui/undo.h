#ifndef QUIXEL_UNDO_H
#define QUIXEL_UNDO_H

#define QUIXEL_UNDO_TYPE_TOOL       0
#define QUIXEL_UNDO_TYPE_FRAME      1
#define QUIXEL_UNDO_FLOAT_SELECTION 2

bool quixel_make_tool_undo(QUIXEL_CANVAS_EDITOR * cep, int layer, int x, int y, int width, int height, const char * fn);
bool quixel_make_tool_redo(QUIXEL_CANVAS_EDITOR * cep, int layer, int x, int y, int width, int height, const char * fn);

const char * quixel_get_undo_name(const char * fn);
const char * quixel_get_undo_path(const char * base, int count, char * out, int out_size);
bool quixel_apply_undo(QUIXEL_CANVAS_EDITOR * cep, const char * fn, bool redo);
bool quixel_apply_redo(QUIXEL_CANVAS_EDITOR * cep, const char * fn);

#endif
