#ifndef QUIXEL_UNDO_FRAME_H
#define QUIXEL_UNDO_FRAME_H

bool quixel_make_frame_undo(QUIXEL_CANVAS_EDITOR * cep, const char * action, const char * fn);
bool quixel_make_frame_redo(QUIXEL_CANVAS_EDITOR * cep, const char * action, const char * fn);
bool quixel_apply_frame_undo(QUIXEL_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp, const char * action, bool revert);
bool quixel_apply_frame_redo(QUIXEL_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp, const char * action);

#endif
