#include "t3f/t3f.h"
#include "t3gui/t3gui.h"

/* typedef for the listbox callback functions */
typedef const char *(getfuncptr)(int index, int *num_elem, void *dp3);

int quixel_list_proc(int msg, T3GUI_ELEMENT *d, int c)
{
  switch(msg)
  {
    case MSG_KEYDOWN:
    case MSG_KEYREPEAT:
    {
      return D_O_K;
    }
    default:
    {
      return t3gui_list_proc(msg, d, c);
    }
  }
  return D_O_K;
}
