#include <stdio.h>
#include "t3f/t3f.h"

static FILE * pa_debug_file = NULL;

bool pa_open_debug_log(const char * fn)
{
	pa_debug_file = fopen(fn, "w");
	if(pa_debug_file)
	{
		return true;
	}
	return false;
}

void pa_close_debug_log(void)
{
	if(pa_debug_file)
	{
		fclose(pa_debug_file);
		pa_debug_file = NULL;
	}
}

void pa_write_debug_message(const char * message)
{
	fputs(message, pa_debug_file);
	fflush(pa_debug_file);
}
