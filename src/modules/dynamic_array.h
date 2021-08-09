#ifndef PA_DYNAMIC_ARRAY
#define PA_DYNAMIC_ARRAY

void ** pa_malloc(int element_size, int elements);
void pa_free(void ** p);

#endif
