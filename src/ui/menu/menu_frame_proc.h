#ifndef PA_MENU_FRAME_PROC_H
#define PA_MENU_FRAME_PROC_H

int pa_menu_frame_add_from_selection(int id, void * data);

	int pa_menu_frame_add_8x8(int id, void * data);
	int pa_menu_frame_add_12x12(int id, void * data);
	int pa_menu_frame_add_16x16(int id, void * data);
	int pa_menu_frame_add_24x24(int id, void * data);
	int pa_menu_frame_add_32x32(int id, void * data);
	int pa_menu_frame_add_48x48(int id, void * data);
	int pa_menu_frame_add_64x64(int id, void * data);
	int pa_menu_frame_add_72x72(int id, void * data);
	int pa_menu_frame_add_96x96(int id, void * data);
	int pa_menu_frame_add_128x128(int id, void * data);
	int pa_menu_frame_add_256x256(int id, void * data);
	int pa_menu_frame_add_512x512(int id, void * data);

	int pa_menu_frame_add_256x224(int id, void * data);
	int pa_menu_frame_add_320x200(int id, void * data);
	int pa_menu_frame_add_320x240(int id, void * data);
	int pa_menu_frame_add_640x480(int id, void * data);

int pa_menu_frame_delete(int id, void * data);
int pa_menu_frame_previous(int id, void * data);
int pa_menu_frame_next(int id, void * data);

#endif
