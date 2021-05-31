#ifndef QUIXEL_UNDO_LAYER_H
#define QUIXEL_UNDO_LAYER_H

bool quixel_make_add_layer_undo(QUIXEL_CANVAS_EDITOR * cep, const char * fn);
bool quixel_make_add_layer_redo(QUIXEL_CANVAS_EDITOR * cep, const char * fn);
bool quixel_make_remove_layer_undo(QUIXEL_CANVAS_EDITOR * cep, int layer, const char * fn);
bool quixel_make_remove_layer_redo(QUIXEL_CANVAS_EDITOR * cep, int layer, const char * fn);
bool quixel_apply_add_layer_undo(QUIXEL_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp);
bool quixel_apply_add_layer_redo(QUIXEL_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp);
bool quixel_apply_remove_layer_undo(QUIXEL_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp);
bool quixel_apply_remove_layer_redo(QUIXEL_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp);

#endif
