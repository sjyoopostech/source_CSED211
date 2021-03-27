#include "interrupt.h"

unsigned int send_x;
unsigned int send_y;

/* enable interrupt in CPU level */
void enable_interrupts(void){
  __asm__ __volatile__ ("mrs r0, cpsr");
  __asm__ __volatile__ ("bic r0, r0, #0x80");
  __asm__ __volatile__ ("msr cpsr_c, r0");
}

/* disable interrupt in CPU level */
void disable_interrupts(void){
  __asm__ __volatile__ ("mrs r0, cpsr");
  __asm__ __volatile__ ("orr r0, r0, #0x80");
  __asm__ __volatile__ ("msr cpsr_c, r0");
}

unsigned int timer1_isr_call_count = 0;
unsigned int touch_isr_call_count = 0;

void touchInterruptServiceRoutine(void){
  unsigned int temp;
  unsigned int temp2;

  if( !(VIC1RAWINTR_REG & 1<<30) )
    return;

  /* Disable any other interrupt */
  temp = VIC1INTENABLE_REG;
  VIC1INTENCLEAR_REG = 0xffffffff;

  temp2 = readl(ADCCON);
  temp2 |= 1;
  writel(temp2, ADCCON);
  writel(0xd4, ADCTSC);

  //printf ("Touch Detected\t");
  touch_isr_call_count ++;

  writel(0x1, ADCCLRINTPNDNUP);

  /* Enable other interrupts */
  VIC1INTENABLE_REG = temp;
}

void touchInterruptServiceRoutine2(void){
  unsigned int temp;
  unsigned int x, y, rx, ry;

  if( !(VIC1RAWINTR_REG & 1<<31) )
    return;

  /* Disable any other interrupt */
  temp = VIC1INTENABLE_REG;
  VIC1INTENCLEAR_REG = 0xffffffff;

  while( !(readl(ADCCON) & 1<<15) );
  
  x = readl(ADCDAT0) & 0x3ff;
  y = readl(ADCDAT1) & 0x3ff;

  rx = (x - 200) * 800 / 640;
  ry = (y - 340) * 480 / 360;

  if (rx < 800 && ry < 480) {
    send_x = rx;
    send_y = ry;
  }

  //printf("(%d, %d)\n", rx, ry);
 
  writel(0xd3, ADCTSC);
  writel(0x1, ADCCLRINT);
    
  /* Enable other interrupts */
  VIC1INTENABLE_REG = temp;
}

void mango_interrupt_init(void){
  VIC1INTENABLE_REG |= BIT_ADCEOC;
  VIC1INTENABLE_REG |= BIT_ADC_PEN;

  writel(0xffff, ADCDLY);
  writel(0xd3, ADCTSC);
  writel(0x7fc1, ADCCON); 

  VIC1VECTADDR30 = (unsigned)touchInterruptServiceRoutine;
  VIC1VECTADDR31 = (unsigned)touchInterruptServiceRoutine2;
}
