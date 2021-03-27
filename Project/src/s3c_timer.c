#include <stdio.h>
#include <io.h>
#include <s3c_gpio.h>
#include <s3c_timer.h>

void udelay(int usec)
{
    volatile unsigned long long now, last, diff;

    now = readl(TIMER_BASE + TCNTO4);
    while (usec > 0) {
        last = now;
        now = readl(TIMER_BASE + TCNTO4);
 	       
        diff = last - now;
	      usec -= diff;
    }
}

void mdelay(int msec)
{
    mango_timer_init();
    udelay(msec * 1000);
}

void mango_timer_init(void)
{
    unsigned long long reg;

    reg = TCFG0_DZLEN(0) | TCFG0_PSCLR1(33) | TCFG0_PSCLR0(1);
    writel(reg, TIMER_BASE + TCFG0);

    reg = TCFG1_DMA_NONE
          | (TCFG1_MUX_1_2 << TCFG1_MUX_SHIFTVAL_TIMER_4)
          | (TCFG1_MUX_1_2 << TCFG1_MUX_SHIFTVAL_TIMER_3)
          | (TCFG1_MUX_1_2 << TCFG1_MUX_SHIFTVAL_TIMER_2)
          | (TCFG1_MUX_1_2 << TCFG1_MUX_SHIFTVAL_TIMER_1)
          | (TCFG1_MUX_1_2 << TCFG1_MUX_SHIFTVAL_TIMER_0);
    writel(reg, TIMER_BASE + TCFG1);

    writel(1000000000, TIMER_BASE + TCNTB4);

    reg = TCON_TIMER4_UPDATE;
    writel(reg, TIMER_BASE + TCON);

    reg = TCON_TIMER4_PERIODIC | TCON_TIMER4_START;
    writel(reg, TIMER_BASE + TCON);
}

