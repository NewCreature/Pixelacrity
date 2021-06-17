#ifndef QUIXEL_UNDO_IMPORT_H
#define QUIXEL_UNDO_IMPORT_H

bool quixel_make_import_undo(QUIXEL_CANVAS_EDITOR * cep, int layer, const char * import_fn, const char * fn);
bool quixel_make_import_redo(QUIXEL_CANVAS_EDITOR * cep, int layer, const char * import_fn, const char * fn);
bool quixel_apply_import_undo(QUIXEL_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp);
bool quixel_apply_import_redo(QUIXEL_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp);

#endif
