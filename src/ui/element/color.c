#include "t3f/t3f.h"
#include "t3gui/t3gui.h"
#include "modules/color.h"
#include "modules/bitmap.h"
#include "instance.h"
#include "color.h"

static ALLEGRO_BITMAP * color_texture = NULL;
static ALLEGRO_BITMAP * color_background = NULL;
static T3F_ATLAS * color_atlas;

PA_GUI_COLOR_DATA * pa_create_gui_color_data(ALLEGRO_COLOR * color, PA_COLOR_INFO * left_color, PA_COLOR_INFO * right_color, ALLEGRO_COLOR * left_target_color, ALLEGRO_COLOR * right_target_color, int * left_clicked, int * right_clicked)
{
	PA_GUI_COLOR_DATA * color_data;

	color_data = malloc(sizeof(PA_GUI_COLOR_DATA));
	if(!color_data)
	{
		goto fail;
	}
	memset(color_data, 0, sizeof(PA_GUI_COLOR_DATA));
	color_data->color = color;
	color_data->left_color = left_color;
	color_data->right_color = right_color;
	color_data->left_target_color = left_target_color;
	color_data->right_target_color = right_target_color;
	color_data->left_clicked = left_clicked;
	color_data->right_clicked = right_clicked;

	return color_data;

	fail:
	{
		if(color_data)
		{
			pa_destroy_gui_color_data(color_data);
		}
		return NULL;
	}
}

void pa_destroy_gui_color_data(PA_GUI_COLOR_DATA * dp)
{
	free(dp);
}

void pa_drop_or_swap_gui_color_data(PA_GUI_COLOR_DATA * sdp, PA_GUI_COLOR_DATA * ddp)
{
	ALLEGRO_COLOR tcol;
	bool swap = false;

	tcol = *ddp->color;
	if(!ddp->left_target_color && !sdp->left_target_color)
	{
		swap = true;
	}
	else if(ddp->left_target_color && sdp->left_target_color)
	{
		swap = true;
	}
	if(swap)
	{
		*ddp->color = *sdp->color;
		if(!t3f_key[ALLEGRO_KEY_LSHIFT] && !t3f_key[ALLEGRO_KEY_RSHIFT])
		{
			*sdp->color = tcol;
		}
	}
	else
	{
		*ddp->color = *sdp->color;
	}
}

int pa_gui_color_proc(int msg, T3GUI_ELEMENT * d, int c)
{
	PA_GUI_COLOR_DATA * color_data = (PA_GUI_COLOR_DATA *)d->dp;

	switch(msg)
	{
		case MSG_START:
		{
			if(!color_texture)
			{
				color_texture = pa_make_checkerboard_bitmap(t3f_color_white, t3f_color_white);
			}
			if(!color_background)
			{
				color_background = pa_make_checkerboard_bitmap(t3f_color_white, al_map_rgba_f(0.9, 0.9, 0.9, 1.0));
			}
			if(!color_atlas)
			{
				color_atlas = t3f_create_atlas(32, 32);
				if(color_atlas)
				{
					t3f_add_bitmap_to_atlas(color_atlas, &color_texture, T3F_ATLAS_TILE);
					t3f_add_bitmap_to_atlas(color_atlas, &color_background, T3F_ATLAS_TILE);
				}
			}
			d->id1 = 0;
			break;
		}
		case MSG_END:
		{
			if(d->dp)
			{
				pa_destroy_gui_color_data(d->dp);
			}
			break;
		}
		case MSG_MOUSEDOWN:
		{
			d->flags &= ~D_USER;
			d->d2 = c;
			if(!(d->flags & D_DISABLED))
			{
				d->flags |= D_TRACKMOUSE;
			}
			break;
		}
		case MSG_MOUSEUP:
		{
			d->d2 = 0;
			if(!(d->flags & D_DISABLED))
			{
				d->flags &= ~D_TRACKMOUSE;
				if(t3gui_get_mouse_x() >= d->x && t3gui_get_mouse_x() < d->x + d->w && t3gui_get_mouse_y() >= d->y && t3gui_get_mouse_y() < d->y + d->h)
				{
					if(!color_data->left_color && !color_data->right_color)
					{
						d->id1 = 1;
					}
					else
					{
						if(t3f_key[ALLEGRO_KEY_LCTRL] || t3f_key[ALLEGRO_KEY_RCTRL] || t3f_key[ALLEGRO_KEY_COMMAND])
						{
							if(c == 1 && color_data->left_target_color)
							{
								*(color_data->color) = *(color_data->left_target_color);
							}
							else if(color_data->right_target_color)
							{
								*(color_data->color) = *(color_data->right_target_color);
							}
						}
						else
						{
							if(c == 1 && color_data->left_color)
							{
								color_data->left_color->base_color = *(color_data->color);
							}
							else if(color_data->right_color)
							{
								color_data->right_color->base_color = *(color_data->color);
							}
						}
					}
					if(c == 1 && color_data->left_clicked)
					{
						*(color_data->left_clicked) = 1;
					}
					else if(c == 2 && color_data->right_clicked)
					{
						*(color_data->right_clicked) = 1;
					}
				}
			}
			break;
		}
		case MSG_MOUSEMOVE:
		{
			if(d->flags & D_TRACKMOUSE)
			{
				d->flags |= D_USER;
			}
			break;
		}
		case MSG_DRAW:
		{
			if(!(d->flags & D_DISABLED))
			{
				al_draw_tinted_scaled_bitmap(color_background, t3f_color_white, 0, 0, al_get_bitmap_width(color_background), al_get_bitmap_height(color_background), d->x, d->y, d->w, d->h, 0);
				al_draw_tinted_scaled_bitmap(color_texture, *(color_data->color), 0, 0, al_get_bitmap_width(color_background), al_get_bitmap_height(color_background), d->x, d->y, d->w, d->h, 0);
				if(d->flags & D_SELECTED)
				{
					draw_nine_patch_bitmap(d->theme->state[0].bitmap[0], t3f_color_white, d->x, d->y, d->w, d->h);
				}
			}
			break;
		}
	}
	return D_O_K;
}
