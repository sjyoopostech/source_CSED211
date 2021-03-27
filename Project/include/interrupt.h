#ifndef __INTERRUPT_H
#define __INTERRUPT_H

#include <stdio.h>

#include "lcd.h"
#include "util.h"
#include "graphics.h"
#include "s3c_uart.h"
#include "s3c6410.h"
#include "vic.h"

/* Registers for timer interrupt */
#define BIT_TIMER1 (1<<24)
#define TINT_CSTAT_REG __REG(0x7f006044)
#define BIT_TIMER1_STAT (1<<6)
#define BIT_TIMER1_EN (1<<1)

/* Registers for touch interrupt  */
#define BIT_ADCEOC (1<<31)
#define BIT_ADC_PEN (1<<30)

void enable_interrupts(void);
void disable_interrupts(void);
void touchInterruptServiceRoutine(void);
void touchInterruptServiceRoutine2(void);
void mango_interrupt_init(void);

extern unsigned int send_x;
extern unsigned int send_y;

#endif
