#ifndef PA_UNDO_SELECTION_H
#define PA_UNDO_SELECTION_H

bool pa_make_float_selection_undo(PA_CANVAS_EDITOR * cep, PA_BOX * box, bool multi, const char * fn);
bool pa_make_float_selection_redo(PA_CANVAS_EDITOR * cep, int layer, int new_x, int new_y, int new_widtn, int new_height, bool multi, const char * fn);
bool pa_make_unfloat_selection_undo(PA_CANVAS_EDITOR * cep, const char * fn);
bool pa_make_unfloat_selection_redo(PA_CANVAS_EDITOR * cep, const char * fn);
bool pa_make_flip_selection_undo(PA_CANVAS_EDITOR * cep, bool horizontal, bool vertical, bool multi, const char * fn);
bool pa_make_flip_selection_redo(PA_CANVAS_EDITOR * cep, bool horizontal, bool vertical, bool multi, const char * fn);
bool pa_make_turn_selection_undo(PA_CANVAS_EDITOR * cep, int amount, bool multi, const char * fn);
bool pa_make_turn_selection_redo(PA_CANVAS_EDITOR * cep, int amount, bool multi, const char * fn);
bool pa_make_delete_selection_undo(PA_CANVAS_EDITOR * cep, int type, const char * action, bool multi, const char * fn);
bool pa_make_delete_selection_redo(PA_CANVAS_EDITOR * cep, int type, const char * action, bool f, bool multi, const char * fn);
bool pa_make_paste_undo(PA_CANVAS_EDITOR * cep, int pos, int x, int y, const char * fn);
bool pa_make_paste_redo(PA_CANVAS_EDITOR * cep, int pos, int x, int y, const char * fn);

bool pa_apply_unfloat_selection_undo(PA_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp, const char * action, bool revert);
bool pa_apply_unfloat_selection_redo(PA_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp, const char * action);
bool pa_apply_float_selection_undo(PA_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp, const char * action, bool revert);
bool pa_apply_float_selection_redo(PA_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp, const char * action);
bool pa_apply_flip_selection_undo(PA_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp);
bool pa_apply_flip_selection_redo(PA_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp);
bool pa_apply_turn_selection_undo(PA_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp);
bool pa_apply_turn_selection_redo(PA_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp);
bool pa_apply_delete_selection_undo(PA_CANVAS_EDITOR * cep, int type, const char * action, ALLEGRO_FILE * fp);
bool pa_apply_delete_selection_redo(PA_CANVAS_EDITOR * cep, int type, const char * action, ALLEGRO_FILE * fp);
bool pa_apply_paste_undo(PA_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp);
bool pa_apply_paste_redo(PA_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp);

#endif
