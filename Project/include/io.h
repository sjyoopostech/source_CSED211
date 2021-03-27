#ifndef __IO_H

#define __IO_H

 

#define readl(addr)             (*(volatile unsigned long *)(addr))

#define readw(addr)             (*(volatile unsigned short *)(addr))

#define readb(addr)             (*(volatile unsigned char *)(addr))

 

#define writel(val, addr)       (*(volatile unsigned long *)(addr) = val)

#define writew(val, addr)       (*(volatile unsigned short *)(addr) = val)

#define writeb(val, addr)       (*(volatile unsigned char *)(addr) = val)

 

#endif  /* __IO_H */

