#ifndef QUIXEL_UNDO_SELECTION_H
#define QUIXEL_UNDO_SELECTION_H

bool quixel_make_float_selection_undo(QUIXEL_CANVAS_EDITOR * cep, const char * fn);
bool quixel_make_float_selection_redo(QUIXEL_CANVAS_EDITOR * cep, int new_x, int new_y, const char * fn);
bool quixel_make_unfloat_selection_undo(QUIXEL_CANVAS_EDITOR * cep, const char * fn);
bool quixel_make_unfloat_selection_redo(QUIXEL_CANVAS_EDITOR * cep, const char * fn);
bool quixel_apply_unfloat_selection_undo(QUIXEL_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp, const char * action, bool revert);
bool quixel_apply_unfloat_selection_redo(QUIXEL_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp, const char * action);
bool quixel_apply_float_selection_undo(QUIXEL_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp, const char * action, bool revert);
bool quixel_apply_float_selection_redo(QUIXEL_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp, const char * action);

#endif
