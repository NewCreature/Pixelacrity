#ifndef PA_UNDO_LAYER_H
#define PA_UNDO_LAYER_H

bool pa_make_add_layer_undo(PA_CANVAS_EDITOR * cep, const char * fn);
bool pa_make_add_layer_redo(PA_CANVAS_EDITOR * cep, const char * fn);
bool pa_make_remove_layer_undo(PA_CANVAS_EDITOR * cep, int layer, const char * fn);
bool pa_make_remove_layer_redo(PA_CANVAS_EDITOR * cep, int layer, const char * fn);
bool pa_make_swap_layer_undo(PA_CANVAS_EDITOR * cep, int layer1, int layer2, const char * fn);
bool pa_make_swap_layer_redo(PA_CANVAS_EDITOR * cep, int layer1, int layer2, const char * fn);
bool pa_apply_add_layer_undo(PA_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp);
bool pa_apply_add_layer_redo(PA_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp);
bool pa_apply_remove_layer_undo(PA_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp);
bool pa_apply_remove_layer_redo(PA_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp);
bool pa_apply_swap_layer_undo(PA_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp);
bool pa_apply_swap_layer_redo(PA_CANVAS_EDITOR * cep, ALLEGRO_FILE * fp);

#endif
