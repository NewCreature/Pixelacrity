#ifndef PA_DEBUG_H
#define PA_DEBUG_H

bool pa_open_debug_log(const char * fn);
void pa_close_debug_log(void);
void pa_write_debug_message(const char * message);

#endif
