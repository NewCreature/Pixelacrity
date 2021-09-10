#ifndef PA_UNDO_SELECTION_H
#define PA_UNDO_SELECTION_H

bool pa_make_float_selection_undo(PA_CANVAS_EDITOR * cep, PA_BOX * box, const char * fn);
bool pa_make_float_selection_redo(PA_CANVAS_EDITOR * cep, int new_x, int new_y, ALLEGRO_BITMAP * bp, const char * fn);
bool pa_make_unfloat_selection_undo(PA_CANVAS_EDITOR * cep, const char * fn);
bool pa_make_unfloat_selection_redo(PA_CANVAS_EDITOR * cep, const char * fn);
bool pa_apply_unfloat_selection_undo(PA_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp, const char * action, bool revert);
bool pa_apply_unfloat_selection_redo(PA_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp, const char * action);
bool pa_apply_float_selection_undo(PA_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp, const char * action, bool revert);
bool pa_apply_float_selection_redo(PA_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp, const char * action);

#endif
