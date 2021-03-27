#ifndef __S3C_GPIO_H
#define __S3C_GPIO_H

#include <config.h>

typedef enum {
    S3C_GPIO_PORT_A,
    S3C_GPIO_PORT_B,
    S3C_GPIO_PORT_C,
    S3C_GPIO_PORT_D,
    S3C_GPIO_PORT_E,
    S3C_GPIO_PORT_F,
    S3C_GPIO_PORT_G,
    S3C_GPIO_PORT_H,
    S3C_GPIO_PORT_I,
    S3C_GPIO_PORT_J,
    S3C_GPIO_PORT_K,
    S3C_GPIO_PORT_L,
    S3C_GPIO_PORT_M,
#ifdef CONFIG_S3C6410
    S3C_GPIO_PORT_N,
    S3C_GPIO_PORT_O,
    S3C_GPIO_PORT_P,
    S3C_GPIO_PORT_Q,
#endif
    S3C_GPIO_PORT_MAX,
} S3C_GPIO_PORT;

typedef enum {
    S3C_GPIO_TYPE_INPUT,
    S3C_GPIO_TYPE_OUTPUT,
    S3C_GPIO_TYPE_ALT1,
    S3C_GPIO_TYPE_ALT2,
#ifdef CONFIG_S3C6410
    S3C_GPIO_TYPE_ALT3,
    S3C_GPIO_TYPE_ALT4,
    S3C_GPIO_TYPE_ALT5,
    S3C_GPIO_TYPE_ALT6,
#endif
    S3C_GPIO_TYPE_MAX,
} S3C_GPIO_TYPE;

typedef enum {
#ifdef CONFIG_S3C2443
    S3C_GPIO_PUD_UP,
    S3C_GPIO_PUD_UPDOWN_DISABLE,
    S3C_GPIO_PUD_DOWN,
#else /* CONFIG_S3C6410 */
    S3C_GPIO_PUD_UPDOWN_DISABLE,
    S3C_GPIO_PUD_DOWN,
    S3C_GPIO_PUD_UP,
#endif
    S3C_GPIO_PUD_MAX,
} S3C_GPIO_PUD;

#ifdef CONFIG_S3C2443
#define GPCDH				0x04
#endif
#define GPCON				0x00
#define GPDAT				0x04
#define GPUDP				0x08

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

int mango_gpio_set_type(S3C_GPIO_PORT port, int pinnum, S3C_GPIO_TYPE type);

int mango_gpio_set_val(S3C_GPIO_PORT port, int pinnum, int val);

int mango_gpio_get_val(S3C_GPIO_PORT port, int pinnum, int *val);

int mango_gpio_set_pullupdown(S3C_GPIO_PORT port, int pinnum, S3C_GPIO_PUD pud);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* __S3C_GPIO_H */

