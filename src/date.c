#include <stdio.h>
#include <time.h>

char * quixel_get_date_string(char * out, int out_size)
{
	time_t etime = time(NULL);
	struct tm  * btime;
	btime = localtime(&etime);
	if(out_size > 32)
	{
		sprintf(out, "%02d-%02d-%d - %02d%02d%02d", btime->tm_mon + 1, btime->tm_mday, 1900 + btime->tm_year, btime->tm_hour, btime->tm_min, btime->tm_sec);
		return out;
	}
	return NULL;
}
