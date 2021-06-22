#ifndef PA_CANVAS_FILE_H
#define PA_CANVAS_FILE_H

#define PA_CANVAS_SAVE_AUTO   -1
#define PA_CANVAS_SAVE_MINIMAL 0
#define PA_CANVAS_SAVE_FULL    1

#include "canvas.h"

PA_CANVAS * pa_load_canvas(const char * fn, int bitmap_max);
bool pa_save_canvas(PA_CANVAS * cp, const char * fn, const char * format, int method);

#endif
