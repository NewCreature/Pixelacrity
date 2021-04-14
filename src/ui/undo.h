#ifndef QUIXEL_UNDO_H
#define QUIXEL_UNDO_H

#define QUIXEL_UNDO_TYPE_TOOL 0

bool quixel_make_tool_undo(QUIXEL_CANVAS_EDITOR * cep, int layer, int x, int y, int width, int height, const char * fn);
bool quixel_make_tool_redo(QUIXEL_CANVAS_EDITOR * cep, int layer, int x, int y, int width, int height, const char * fn);

const char * quixel_get_undo_name(const char * fn);
const char * quixel_get_undo_path(int count);
bool quixel_apply_undo(QUIXEL_CANVAS_EDITOR * cep, const char * fn);
bool quixel_apply_redo(QUIXEL_CANVAS_EDITOR * cep, const char * fn);

#endif
