#ifndef __S3C_TIMER_H
#define __S3C_TIMER_H

#include <config.h>

#define TCFG0       0x00
#define TCFG1       0x04
#define TCON        0x08
#define TCNTB0      0x0C
#define TCMPB0      0x10
#define TCNTO0      0x14
#define TCNTB1      0x18
#define TCMPB1      0x1C
#define TCNTO1      0x20
#define TCNTB2      0x24
#define TCNTO2      0x2C
#define TCNTB3      0x30
#define TCNTO3      0x38
#define TCNTB4      0x3C
#define TCNTO4      0x40

#ifdef CONFIG_S3C2443
#define TCMPB2      0x28
#define TCMPB3      0x34
#define TIMER_BASE  0x51000000
#define TCNTB_MAX   0x0000FFFF
#endif	/* CONFIG_S3C2443 */

#ifdef CONFIG_S3C6410
#define TINT_CSTAT  0x44
#define TIMER_BASE  0x7F006000
#define TCNTB_MAX   0xFFFFFFFF
#endif	/* CONFIG_S3C6410 */

#define TCFG0_PSCLR0(x) 	(x - 1)
#define TCFG0_PSCLR1(x)		((x - 1) << 8)
#define TCFG0_DZLEN(x)		(x << 16)

#define TCFG1_DMA_NONE		(0)
#define TCFG1_DMA_TIMER0	(1 << 20)
#define TCFG1_DMA_TIMER1	(2 << 20)
#define TCFG1_DMA_TIMER2	(3 << 20)
#define TCFG1_DMA_TIMER3	(4 << 20)
#define TCFG1_DMA_TIMER4	(5 << 20)

#ifdef CONFIG_S3C2443
#define TCFG1_MUX_1_2       (0)
#define TCFG1_MUX_1_4       (1)
#define TCFG1_MUX_1_8       (2)
#define TCFG1_MUX_1_16      (3)
#define TCFG1_MUX_EXTCLK    (4)
#endif	/* CONFIG_S3C2443 */

#ifdef CONFIG_S3C6410
#define TCFG1_MUX_1_1       (0)
#define TCFG1_MUX_1_2       (1)
#define TCFG1_MUX_1_4       (2)
#define TCFG1_MUX_1_8       (3)
#define TCFG1_MUX_1_16      (4)
#define TCFG1_MUX_EXTCLK    (5)
#endif	/* CONFIG_S3C6410 */

#define TCFG1_MUX_SHIFTVAL_TIMER_0   (0)
#define TCFG1_MUX_SHIFTVAL_TIMER_1   (4)
#define TCFG1_MUX_SHIFTVAL_TIMER_2   (8)
#define TCFG1_MUX_SHIFTVAL_TIMER_3   (12)
#define TCFG1_MUX_SHIFTVAL_TIMER_4   (16)

#define TCON_TIMER_START		(1)
#define TCON_TIMER_UPDATE		(1 << 1)
#define TCON_TIMER_INVERT		(1 << 2)
#define TCON_TIMER_PERIODIC		(1 << 3)
#define TCON_TIMER0_DEADZONE	(1 << 4)
#define TCON_TIMER4_START		(1 << 20)
#define TCON_TIMER4_UPDATE		(1 << 21)
#define TCON_TIMER4_PERIODIC	(1 << 22)

#ifdef __cplusplus
extern "C" {
#endif	/* __cplusplus */

void udelay(int usec);
void mdelay(int msec);

void mango_timer_init(void);

#ifdef __cplusplus
}
#endif	/* __cplusplus */

#endif	/* __S3C_TIMER_H */
