#include "t3f/t3f.h"
#include "filename.h"

const char * pa_get_path_filename(const char * fn)
{
  int i;

  if(fn)
  {
    for(i = strlen(fn) - 2; i >= 0; i--)
    {
      if(fn[i] == '/')
      {
        return &fn[i + 1];
      }
    }
  }
  return fn;
}
