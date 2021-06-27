#ifndef PA_INSTANCE_H
#define PA_INSTANCE_H

#include "t3f/t3f.h"
#include "modules/canvas/canvas.h"
#include "ui/ui.h"
#include "ui/canvas_editor/canvas_editor.h"

/* structure to hold all of our app-specific data */
typedef struct
{

	PA_CANVAS * canvas;
	PA_UI * ui;
	PA_CANVAS_EDITOR * canvas_editor;
	bool restart_ui;

} APP_INSTANCE;

#endif
