#ifndef PA_CANVAS_EDITOR_PREVIEW_H
#define PA_CANVAS_EDITOR_PREVIEW_H

#include "modules/canvas/canvas.h"

typedef struct
{

  ALLEGRO_BITMAP * bitmap;
  int width, height;
  int canvas_x, canvas_y, canvas_width, canvas_height;
  int offset_x;
  int offset_y;
  float scale;

} PA_CANVAS_EDITOR_PREVIEW;

PA_CANVAS_EDITOR_PREVIEW * pa_create_canvas_editor_preview(int width, int height);
void pa_destroy_canvas_editor_preview(PA_CANVAS_EDITOR_PREVIEW * pp);

void pa_update_canvas_editor_preview(PA_CANVAS_EDITOR_PREVIEW * pp, PA_CANVAS * cp, ALLEGRO_SHADER * shader);

#endif
