#ifndef __S3C_UART_H

#define __S3C_UART_H

#include "io.h"
#include "config.h" 

/* register offset */

#define ULCON       0x00

#define UCON        0x04

#define UFCON       0x08

#define UMCON       0x0c

#define UTRSTAT     0x10

#define UERSTAT     0x14

#define UFSTAT      0x18

#define UMSTAT      0x1c

#define UTXH        0x20

#define URXH        0x24

#define UBRDIV      0x28

#define UDIVSLOT    0x2c

 

/* ULCON */

#define ULCON_IR_MODE       (1 << 6)

#define ULCON_PARITY_NO     (0)

#define ULCON_PARITY_ODD    (4 << 3)

#define ULCON_PARITY_EVEN   (5 << 3)

#define ULCON_PARITY_FORCE1 (6 << 3)

#define ULCON_PARITY_FORCE0 (7 << 3)

#define ULCON_STOPBIT_1     (0)

#define ULCON_STOPBIT_2     (1 << 2)

#define ULCON_WL_5BITS      (0)

#define ULCON_WL_6BITS      (1)

#define ULCON_WL_7BITS      (2)

#define ULCON_WL_8BITS      (3)

#define ULCON_8N1           (ULCON_PARITY_NO | ULCON_STOPBIT_1 | ULCON_WL_8BITS)

 

/* UCON */

#define UCON_CLK_PCLK       (2 << 10)

#define UCON_CLK_EXTUARTCLK (1 << 10)

#define UCON_CLK_DIVEPLL    (3 << 10)

#define UCON_TXIRQ_PULSE    (0)

#define UCON_TXIRQ_LEVEL    (1 << 9)

#define UCON_RXIRQ_PULSE    (0)

#define UCON_RXIRQ_LEVEL    (1 << 8)

#define UCON_RXTMOUT_ENABLE (1 << 7)

#define UCON_ERRIRQ_ELABLE  (1 << 6)

#define UCON_LOOPBACK       (1 << 5)

#define UCON_SEND_BREAK     (1 << 4)

#define UCON_TXMODE_IRQ     (1 << 2)

#define UCON_TXMODE_DMA0    (2 << 2)

#define UCON_TXMODE_DMA1    (3 << 2)

#define UCON_RXMODE_IRQ     (1)

#define UCON_RXMODE_DMA0    (2)

#define UCON_RXMODE_DMA1    (3)

 

/* UFCON */

#define UFCON_TXTRG_EMPTY   (0)

#define UFCON_TXTRG_16BYTES (1 << 6)

#define UFCON_TXTRG_32BYTES (2 << 6)

#define UFCON_TXTRG_48BYTES (3 << 6)

#define UFCON_RXTRG_1BYTES  (0)

#define UFCON_RXTRG_8BYTES  (1 << 4)

#define UFCON_RXTRG_16BYTES (2 << 4)

#define UFCON_RXTRG_32BYTES (3 << 4)

#define UFCON_TXFIFO_RESET  (1 << 2)

#define UFCON_RXFIFO_RESET  (1 << 1)

#define UFCON_FIFO_ENABLE   (1)

 

/* UMCON */

#define UMCON_RTSTRG_63BYTES    (0)

#define UMCON_RTSTRG_56BYTES    (1 << 5)

#define UMCON_RTSTRG_48BYTES    (2 << 5)

#define UMCON_RTSTRG_40BYTES    (3 << 5)

#define UMCON_RTSTRG_32BYTES    (4 << 5)

#define UMCON_RTSTRG_24BYTES    (5 << 5)

#define UMCON_RTSTRG_16BYTES    (6 << 5)

#define UMCON_RTSTRG_8BYTES     (7 << 5)

#define UMCON_AFC_ENABLE        (1 << 4)

#define UMCON_RTS_ACTIVE        (1)

 

/* UTRSTAT */

#define UTRSTAT_TX_EMPTY        (1 << 2)

#define UTRSTAT_TXBUF_EMPTY     (1 << 1)

#define UTRSTAT_RXBUF_READY     (1)

 

/* UERSTAT */

#define UERSTAT_BREAK           (1 << 3)

#define UERSTAT_FRAME_ERR       (1 << 2)

#define UERSTAT_PARITY_ERR      (1 << 1)

#define UERSTAT_OVERRUN_ERR     (1)

 

/* UFSTAT */

#define UFSTAT_TXFIFO_FULL       (1 << 14)

#define UFSTAT_TXFIFO_COUNT(x)   ((x >> 8) & 0x3f)

#define UFSTAT_RXFIFO_FULL       (1 << 6)

#define UFSTAT_RXFIFO_COUNT(x)   (x & 0x3f)

 

/* UMSTAT */

#define UMSTAT_CTS_CHANGED      (1 << 4)

#define UMSTAT_CTS_ACTIVATE     (1)



#ifdef CONFIG_MANGO_24

#define UART_BASE       0x50000000

#define UART0_BASE      (UART_BASE)

#define UART1_BASE      (UART_BASE + 0x4000)

#define UART2_BASE      (UART_BASE + 0x8000)

#define UART3_BASE      (UART_BASE + 0xC000)

#elif defined(CONFIG_MANGO_64)

#define UART_BASE       0x7F005000

#define UART0_BASE      (UART_BASE)

#define UART1_BASE      (UART_BASE + 0x400)

#define UART2_BASE      (UART_BASE + 0x800)

#define UART3_BASE      (UART_BASE + 0xC00)

#else   /* CONFIG_MANGO_24 */

#error "Invalid Target configuration"

#endif  /* CONFIG_MANGO_24 */


#ifdef __cplusplus

extern "C" {

#endif

 

int mango_uart_init(int chn, int baud);

 

void mango_uart_putc(const char c);

 

int mango_uart_write(const char *buf, int len);

 

int mango_uart_read(char *buf, int len, int nowait);

 

#ifdef __cplusplus

}

#endif

 

#endif  /* __S3C_UART_H */

