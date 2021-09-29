#ifndef PA_UNDO_H
#define PA_UNDO_H

#define PA_UNDO_TYPE_TOOL                    0
#define PA_UNDO_TYPE_FRAME                   1
#define PA_UNDO_TYPE_FLOAT_SELECTION         2
#define PA_UNDO_TYPE_UNFLOAT_SELECTION       3
#define PA_REDO_TYPE_FLOAT_SELECTION         4
#define PA_REDO_TYPE_UNFLOAT_SELECTION       5
#define PA_UNDO_TYPE_ADD_LAYER               6
#define PA_REDO_TYPE_ADD_LAYER               7
#define PA_UNDO_TYPE_REMOVE_LAYER            8
#define PA_REDO_TYPE_REMOVE_LAYER            9
#define PA_UNDO_TYPE_FLOOD_FILL             10
#define PA_UNDO_TYPE_IMPORT_IMAGE           11
#define PA_UNDO_TYPE_SWAP_LAYER             12
#define PA_UNDO_TYPE_FLIP_HORIZONTAL        13
#define PA_UNDO_TYPE_FLIP_VERTICAL          14
#define PA_UNDO_TYPE_TURN_CLOCKWISE         15
#define PA_UNDO_TYPE_TURN_COUNTER_CLOCKWISE 16
#define PA_UNDO_TYPE_DELETE_SELECTION       17
#define PA_UNDO_TYPE_CUT_SELECTION          18

typedef struct
{

	int type;
	char * name;
	int layer_max;
	int * layer_offset_x;
	int * layer_offset_y;
	int * layer_width;
	int * layer_height;

} PA_UNDO_HEADER;

const char * pa_get_undo_name(const char * fn, char * out, int out_size);
const char * pa_get_undo_path(const char * base, int count, char * out, int out_size);
void pa_update_undo_name(PA_CANVAS_EDITOR * cep);
void pa_update_redo_name(PA_CANVAS_EDITOR * cep);

bool pa_write_undo_header(ALLEGRO_FILE * fp, PA_CANVAS_EDITOR * cep, int type, const char * name);
PA_UNDO_HEADER * pa_read_undo_header(ALLEGRO_FILE * fp);
void pa_finalize_undo(PA_CANVAS_EDITOR * cep);

bool pa_apply_undo(PA_CANVAS_EDITOR * cep, const char * fn, bool revert);
bool pa_apply_redo(PA_CANVAS_EDITOR * cep, const char * fn);
void pa_undo_clean_up(PA_CANVAS_EDITOR * cep);

#endif
