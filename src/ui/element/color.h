#ifndef PA_GUI_COLOR_H
#define PA_GUI_COLOR_H

#include "ui/canvas_editor/canvas_editor.h"

typedef struct
{

  ALLEGRO_COLOR * color;
  PA_COLOR_INFO * color_info;
  PA_COLOR_INFO * left_color;
  PA_COLOR_INFO * right_color;
  ALLEGRO_COLOR * left_target_color;
  ALLEGRO_COLOR * right_target_color;
  ALLEGRO_COLOR * hover_target_color;
  int * left_clicked;
  int * right_clicked;
  int button;

} PA_GUI_COLOR_DATA;

PA_GUI_COLOR_DATA * pa_create_gui_color_data(ALLEGRO_COLOR * color, PA_COLOR_INFO * color_info, PA_COLOR_INFO * left_color, PA_COLOR_INFO * right_color, ALLEGRO_COLOR * left_target_color, ALLEGRO_COLOR * right_target_color, ALLEGRO_COLOR * hover_target_color, int * left_clicked, int * right_clicked, int button);
void pa_destroy_gui_color_data(PA_GUI_COLOR_DATA * dp);
void pa_drop_or_swap_gui_color_data(PA_GUI_COLOR_DATA * sdp, PA_GUI_COLOR_DATA * ddp);
int pa_gui_color_proc(int msg, T3GUI_ELEMENT * d, int c);

#endif
