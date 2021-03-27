#include "graphics.h"
#include "lcd.h"
#include "util.h"
#include "vic.h"

void gfx_bitblck_ext(unsigned *dest, unsigned *src, 
		int dx1, int dy1, int dx2, int dy2,
		int destw, int desth, 
		int sx1, int sy1, int sx2, int sy2,
		int srcw, int srch) {
	int stretch = 0;

	GFXCOORD0 = sx1 | (sy1 << 16);
	GFXCOORD1 = (sx2-1) | ((sy2-1) << 16);
	GFXCOORD2 = dx1 | (dy1 << 16);
	GFXCOORD3 = (dx2-1) | ((dy2-1) << 16);

	if (dx2 - dx1 != sx2 - sx1) {
		float f;
		f = 2048.0 * (sx2 - sx1) / (dx2 - dx1);
		GFXXINC = (int)f;
		stretch = 1;
	}
	else
		GFXXINC = 1 << 11;

	if (dy2 - dy1 != sy2 - sy1) {
		float f;
		f = 2048.0 * (sy2 - sy1) / (dy2 - dy1);
		GFXYINC = (int)f;
		stretch = 1;
	}
	else
		GFXYINC = 1 << 11;

	GFXSRCBASE = src;
	GFXDESTBASE = dest;

	GFXCWLTX = 0;
	GFXCWLTY = 0;
	GFXCWRBX = destw;
	GFXCWRBY = desth;

	GFXRESH = destw;
	GFXRESV = desth;

	GFXSRCRESH = srcw;

	GFXFGCOL = 0xFFFFFFFF;
	GFXBGCOL = 0xFF0000FF;
	GFXBSCOL = 0xFF0000FF;
	GFXROP = (1 << 13) | (1 << 10) | (0xF0);

	GFXSRCCOLMOD = 4;
	GFXDESTCOLMOD = 4;

	GFXCMD1 = stretch ? 2 : 1;
}

void gfx_bitblck(unsigned *dest, unsigned *src, 
		int destw, int desth, int srcw, int srch, int x, int y) {

	gfx_bitblck_ext(dest, src, 
		x, y, x + srcw, y + srch, destw, desth,
		0, 0, srcw, srch, srcw, srch);
}

void gfx_init(void) {

}
