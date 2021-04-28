#include "t3f/t3f.h"

static const char * ega_palette[64] =
{
	"000000", "0000AA", "00AA00", "00AAAA",
	"AA0000", "AA00AA", "AAAA00", "AAAAAA",
	"000055", "0000FF", "00AA55", "00AAFF",
	"AA0055", "AA00FF", "AAAA55", "AAAAFF",
	"005500", "0055AA", "00FF00", "00FFAA",
	"AA5500", "AA55AA", "AAFF00", "AAFFAA",
	"005555", "0055FF", "00FF55", "00FFFF",
	"AA5555", "AA55FF", "AAFF55", "AAFFFF",
	"550000", "5500AA", "55AA00", "55AAAA",
	"FF0000", "FF00AA", "FFAA00", "FFAAAA",
	"550055", "5500FF", "55AA55", "55AAFF",
	"FF0055", "FF00FF", "FFAA55", "FFAAFF",
	"555500", "5555AA", "55FF00", "55FFAA",
	"FF5500", "FF55AA", "FFFF00", "FFFFAA",
	"555555", "5555FF", "55FF55", "55FFFF",
	"FF5555", "FF55FF", "FFFF55", "FFFFFF"
};

ALLEGRO_COLOR quixel_get_ega_palette_color(int index)
{
	if(index >= 0 && index < 64)
	{
		return al_color_html(ega_palette[index]);
	}
	return al_map_rgba_f(0.0, 0.0, 0.0, 0.0);
}
