#ifndef PA_DEFINES_H
#define PA_DEFINES_H

#define PA_CANVAS_FILE_EXTENSION    ".pxc"
#define PA_PALETTE_FILE_EXTENSION   ".pxp"
#define PA_WORKSPACE_FILE_EXTENSION ".pxw"

#define PA_DEFAULT_THEME "data/themes/standard/theme.ini"

/* Give half a second under non MacOS Unixes, otherwise trigger clear state on
   the next tick. */
#if defined(ALLEGRO_UNIX) && !defined(ALLEGRO_MACOSX)
  #define PA_CLEAR_KEYBOARD_STATE_WAIT_TICKS 30
#else
  #define PA_CLEAR_KEYBOARD_STATE_WAIT_TICKS  1
#endif

#endif
