#ifndef PA_UNDO_FRAME_H
#define PA_UNDO_FRAME_H

bool pa_make_frame_undo(PA_CANVAS_EDITOR * cep, const char * action, const char * fn);
bool pa_make_frame_redo(PA_CANVAS_EDITOR * cep, const char * action, const char * fn);
bool pa_apply_frame_undo(PA_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp, const char * action, bool revert);
bool pa_apply_frame_redo(PA_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp, const char * action);

#endif
