#ifndef PA_UNDO_IMPORT_H
#define PA_UNDO_IMPORT_H

bool pa_make_import_undo(PA_CANVAS_EDITOR * cep, int layer, const char * import_fn, const char * fn);
bool pa_make_import_redo(PA_CANVAS_EDITOR * cep, int layer, const char * import_fn, const char * fn);
bool pa_apply_import_undo(PA_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp);
bool pa_apply_import_redo(PA_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp);

#endif
