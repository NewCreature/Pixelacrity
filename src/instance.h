#ifndef QUIXEL_INSTANCE_H
#define QUIXEL_INSTANCE_H

#include "canvas.h"
#include "ui/ui.h"
#include "ui/canvas_editor.h"

/* structure to hold all of our app-specific data */
typedef struct
{

	QUIXEL_CANVAS * canvas;
	QUIXEL_UI * ui;
	QUIXEL_CANVAS_EDITOR * canvas_editor;

} APP_INSTANCE;

#endif
