#ifndef GRAPHICS
#define GRAPHICS

#include "s3c6410.h"


#define GFXBASE ((void *)0x76100000)

#define GFXCON __REG(GFXBASE + 0x0000)
#define GFXINT __REG(GFXBASE + 0x0004)
#define GFXINTCON __REG(GFXBASE + 0x0008)
#define GFXINTPEN __REG(GFXBASE + 0x000C)

#define GFXINTPENCMD (1 << 10)
#define GFXINTPENALL (1 << 9)
#define GFXINTPENFULL (1 << 8)
#define GFXINTPENLEVEL (1)

#define GFXFIFOSTAT __REG(GFXBASE + 0x0010)

#define GFXCMD0 __REG(GFXBASE + 0x0100) //Line/Point drawing
#define GFXCMD1 __REG(GFXBASE + 0x0104) //BitBLT
#define GFXCMD2 __REG(GFXBASE + 0x0108)
#define GFXCMD3 __REG(GFXBASE + 0x010C)
#define GFXCMD4 __REG(GFXBASE + 0x0110)
#define GFXCMD5 __REG(GFXBASE + 0x0114)
#define GFXCMD6 __REG(GFXBASE + 0x0118)
#define GFXCMD7 __REG(GFXBASE + 0x011C)

#define GFXSRCRES __REG(GFXBASE + 0x0200)
#define GFXSRCRESH __REG(GFXBASE + 0x0204)
#define GFXSRCRESV __REG(GFXBASE + 0x0208)

#define GFXRES __REG(GFXBASE + 0x0210)
#define GFXRESH __REG(GFXBASE + 0x0214)
#define GFXRESV __REG(GFXBASE + 0x0218)

#define GFXCWLT __REG(GFXBASE + 0x0220)
#define GFXCWLTX __REG(GFXBASE + 0x0224)
#define GFXCWLTY __REG(GFXBASE + 0x0228)

#define GFXCWRB __REG(GFXBASE + 0x0230)
#define GFXCWRBX __REG(GFXBASE + 0x0234)
#define GFXCWRBY __REG(GFXBASE + 0x0238)

#define GFXCOORD0 __REG(GFXBASE + 0x0300)
#define GFXCOORD0X __REG(GFXBASE + 0x0304)
#define GFXCOORD0Y __REG(GFXBASE + 0x0308)

#define GFXCOORD1 __REG(GFXBASE + 0x0310)
#define GFXCOORD1X __REG(GFXBASE + 0x0314)
#define GFXCOORD1Y __REG(GFXBASE + 0x0318)

#define GFXCOORD2 __REG(GFXBASE + 0x0320)
#define GFXCOORD2X __REG(GFXBASE + 0x0324)
#define GFXCOORD2Y __REG(GFXBASE + 0x0328)

#define GFXCOORD3 __REG(GFXBASE + 0x0330)
#define GFXCOORD3X __REG(GFXBASE + 0x0334)
#define GFXCOORD3Y __REG(GFXBASE + 0x0338)

#define GFXXINC __REG(GFXBASE + 0x0400)
#define GFXYINC __REG(GFXBASE + 0x0404)

#define GFXROP __REG(GFXBASE + 0x0410)
#define GFXALPHA __REG(GFXBASE + 0x0420)

#define GFXFGCOL __REG(GFXBASE + 0x0500)
#define GFXBGCOL __REG(GFXBASE + 0x0504)
#define GFXBSCOL __REG(GFXBASE + 0x0508)
#define GFXSRCCOLMOD __REG(GFXBASE + 0x0510)
#define GFXDESTCOLMOD __REG(GFXBASE + 0x0514)

#define GFXSRCBASE __REG(GFXBASE + 0x0730)
#define GFXDESTBASE __REG(GFXBASE + 0x0734)

void gfx_init(void);
void gfx_bitblck_ext(unsigned *dest, unsigned *src, 
		int dx1, int dy1, int dx2, int dy2,
		int destw, int desth, 
		int sx1, int sy1, int sx2, int sy2,
		int srcw, int srch);
void gfx_bitblck(unsigned *dest, unsigned *src, 
	int destw, int desth, int srcw, int srch, int x, int y);

#endif
