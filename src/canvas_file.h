#ifndef QUIXEL_CANVAS_FILE_H
#define QUIXEL_CANVAS_FILE_H

#include "canvas.h"

QUIXEL_CANVAS * quixel_load_canvas(const char * fn, int bitmap_max);
bool quixel_save_canvas(QUIXEL_CANVAS * cp, const char * fn, const char * format);

#endif
