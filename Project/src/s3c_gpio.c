#include <stdio.h>
#include <io.h>
#include <s3c_gpio.h>
#include <config.h>

static __inline unsigned long mango_gpio_get_baseaddr(S3C_GPIO_PORT port)
{
    unsigned long baseAddress = 0;
    switch(port)
    {
#ifdef CONFIG_S3C2443
    case S3C_GPIO_PORT_A: baseAddress = 0x56000000; break;
    case S3C_GPIO_PORT_B: baseAddress = 0x56000010; break;
    case S3C_GPIO_PORT_C: baseAddress = 0x56000020; break;
    case S3C_GPIO_PORT_D: baseAddress = 0x56000030; break;
    case S3C_GPIO_PORT_E: baseAddress = 0x56000040; break;
    case S3C_GPIO_PORT_F: baseAddress = 0x56000050; break;
    case S3C_GPIO_PORT_G: baseAddress = 0x56000060; break;
    case S3C_GPIO_PORT_H: baseAddress = 0x56000070; break;
    case S3C_GPIO_PORT_J: baseAddress = 0x560000D0; break;
    case S3C_GPIO_PORT_L: baseAddress = 0x560000F0; break;
    case S3C_GPIO_PORT_M: baseAddress = 0x56000100; break;
#elif defined(CONFIG_S3C6410)
    case S3C_GPIO_PORT_A: baseAddress = 0x7F008000; break;
    case S3C_GPIO_PORT_B: baseAddress = 0x7F008020; break;
    case S3C_GPIO_PORT_C: baseAddress = 0x7F008040; break;
    case S3C_GPIO_PORT_D: baseAddress = 0x7F008060; break;
    case S3C_GPIO_PORT_E: baseAddress = 0x7F008080; break;
    case S3C_GPIO_PORT_F: baseAddress = 0x7F0080A0; break;
    case S3C_GPIO_PORT_G: baseAddress = 0x7F0080C0; break;
    case S3C_GPIO_PORT_H: baseAddress = 0x7F0080E0; break;
    case S3C_GPIO_PORT_I: baseAddress = 0x7F008100; break;
    case S3C_GPIO_PORT_J: baseAddress = 0x7F008120; break;
    case S3C_GPIO_PORT_K: baseAddress = 0x7F008800; break;
    case S3C_GPIO_PORT_L: baseAddress = 0x7F008810; break;
    case S3C_GPIO_PORT_M: baseAddress = 0x7F008820; break;
    case S3C_GPIO_PORT_N: baseAddress = 0x7F008830; break;
    case S3C_GPIO_PORT_O: baseAddress = 0x7F008140; break;
    case S3C_GPIO_PORT_P: baseAddress = 0x7F008160; break;
    case S3C_GPIO_PORT_Q: baseAddress = 0x7F008180; break;
#else /* CONFIG_S3C6410 */
#error "No architecure defined. Select CONFIG_S3C2443 or CONFIG_S3C6410"
#endif
    default:
        break;
    }
    return baseAddress;
}

static __inline unsigned long mango_gpio_get_reg_offset(S3C_GPIO_PORT port, unsigned long org_offset)
{
#ifdef CONFIG_S3C6410
    if (port == S3C_GPIO_PORT_H || port == S3C_GPIO_PORT_K || port == S3C_GPIO_PORT_L)
        org_offset += 0x4;
#endif
    return org_offset;
}

#ifdef CONFIG_S3C2443
static __inline int mango24_gpio_set_type(S3C_GPIO_PORT port, int pinnum, S3C_GPIO_TYPE type, unsigned long base)
{
    unsigned long reg;    
    unsigned long offset = 0;

    if (port == S3C_GPIO_PORT_A) {
        if (type == S3C_GPIO_TYPE_INPUT) {
            printf("%s: Port A does not support input mode\n", __func__);
            return -1;
        }

        if (pinnum >= 8) {
            offset = GPCDH;
            pinnum -= 8;
        }

        reg = readl(base + offset);
        reg &= ~(1 << ((pinnum * 2) + 1));	/* clear pin configuration bit */
        if (type == S3C_GPIO_TYPE_ALT1)
            reg |= (1 << ((pinnum * 2) + 1));
        writel(reg, base + offset);
    } else {	
        reg = readl(base + GPCON);
        reg &= ~(0x3 << (pinnum * 2));
        reg |= (type << (pinnum * 2));
        writel(reg, base + GPCON);
    }

    return 0;
}
#endif

#ifdef CONFIG_S3C6410
static __inline int mango64_gpio_port_has_large_config(S3C_GPIO_PORT port)
{
    return ((port == S3C_GPIO_PORT_H) || (port == S3C_GPIO_PORT_K) || (port == S3C_GPIO_PORT_L));
}

static __inline int mango64_gpio_get_conf_bitsnum(S3C_GPIO_PORT port)
{
    if ((port == S3C_GPIO_PORT_F) || (port == S3C_GPIO_PORT_I) || \
        (port == S3C_GPIO_PORT_J) || (port == S3C_GPIO_PORT_N) || \
        (port == S3C_GPIO_PORT_O) || (port == S3C_GPIO_PORT_P) || \
        (port == S3C_GPIO_PORT_Q)) {
        return 2;
    } else {
        return 4;
    }
}

static __inline int mango64_gpio_set_type(S3C_GPIO_PORT port, int pinnum, S3C_GPIO_TYPE type, unsigned long base)
{
    unsigned long reg;    
    unsigned long offset;
    int bitsnum, seccfg_exist;
    unsigned int mask;

    offset = 0;
    bitsnum = mango64_gpio_get_conf_bitsnum(port);
    if (bitsnum == 4)
        mask = 0xf;
    else
        mask = 0x3;
    seccfg_exist = mango64_gpio_port_has_large_config(port);
    if (seccfg_exist) {
        if (pinnum >= 8) {
            offset += 4;
            pinnum -= 8;
        }
    }
    
    reg = readl(base + offset);
    reg &= ~(mask << (pinnum * bitsnum));
    reg |= (type << (pinnum * bitsnum));
    writel(reg, base + offset);

    return 0;
}
#endif

int mango_gpio_set_type(S3C_GPIO_PORT port, int pinnum, S3C_GPIO_TYPE type)
{
    unsigned long base;

    if (port >= S3C_GPIO_PORT_MAX || type >= S3C_GPIO_TYPE_MAX) {
        printf("%s: Invalid argument\n", __func__);
        return -1;
    }

    base = mango_gpio_get_baseaddr(port);

#ifdef CONFIG_S3C2443
    mango24_gpio_set_type(port, pinnum, type, base);
#elif defined(CONFIG_S3C6410)
    mango64_gpio_set_type(port, pinnum, type, base);
#endif

    return 0;
}

int mango_gpio_set_val(S3C_GPIO_PORT port, int pinnum, int val)
{
    unsigned long reg;
    unsigned long base;

    if (port >= S3C_GPIO_PORT_MAX) {
        printf("%s: Invalid argument\n", __func__);
        return -1;
    }

    base = mango_gpio_get_baseaddr(port);

#ifdef CONFIG_S3C2443
    if (port == S3C_GPIO_PORT_A) {
        unsigned long offset = 0;

        if (pinnum >= 8) {
            offset = GPCDH;
            pinnum -= 8;
        }

        reg = readl(base + offset);
        reg &= ~(1 << (pinnum * 2));	/* clear pin data bit */
        if (val)
            reg |= (1 << (pinnum * 2));
        writel(reg, base + offset);
    } else 
#endif
    {
        reg = readl(base + mango_gpio_get_reg_offset(port, GPDAT));
        reg &= ~(1 << pinnum);
        if (val)
            reg |= (1 << pinnum);
        writel(reg, base + mango_gpio_get_reg_offset(port, GPDAT));
    }

    return 0;
}

int mango_gpio_get_val(S3C_GPIO_PORT port, int pinnum, int *val)
{
    unsigned long reg;
    unsigned long base;

    if (port >= S3C_GPIO_PORT_MAX) {
        printf("%s: Invalid argument\n", __func__);
        return -1;
    }

    base = mango_gpio_get_baseaddr(port);

#ifdef CONFIG_S3C2443
    if (port == S3C_GPIO_PORT_A) {
        unsigned long offset = 0;

        if (pinnum >= 8) {
            offset = GPCDH;
            pinnum -= 8;
        }
        reg = readl(base + offset);
        *val = (reg >> (pinnum * 2)) & 0x1;
    } else 
#endif
    {
        reg = readl(base + mango_gpio_get_reg_offset(port, GPDAT));
        *val = (reg >> pinnum) & 0x1;
    }

    return 0;
}

int mango_gpio_set_pullupdown(S3C_GPIO_PORT port, int pinnum, S3C_GPIO_PUD pud)
{
    unsigned long reg;
    unsigned long base;

    if (port >= S3C_GPIO_PORT_MAX || pud >= S3C_GPIO_PUD_MAX) {
        printf("%s: Invalid argument\n", __func__);
        return -1;
    }

    base = mango_gpio_get_baseaddr(port);

#ifdef CONFIG_S3C2443
    if (port == S3C_GPIO_PORT_A || port == S3C_GPIO_PORT_F) {
        printf("%s: Port A/F does not support input mode\n", __func__);
        return -1;
    }
#endif

    reg = readl(base + mango_gpio_get_reg_offset(port, GPUDP));
    reg &= ~(0x3 << (pinnum * 2));
    reg |= (pud << (pinnum * 2));
    writel(reg, base + mango_gpio_get_reg_offset(port, GPUDP));

    return 0;
}
