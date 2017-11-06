/*
 *  MikroTik RouterBOARD SXT Lite support
 *
 *  Copyright (C) 2012 Stijn Tintel <stijn@linux-ipv6.be>
 *  Copyright (C) 2012 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2013 Vyacheslav Adamanov <adamanov@gmail.com>
 *  Copyright (C) 2017 Robert Marko <robimarko@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#define pr_fmt(fmt) "sxtlite: " fmt

#include <linux/phy.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/ath9k_platform.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/partitions.h>
#include <linux/spi/spi.h>
#include <linux/spi/flash.h>
#include <linux/rle.h>
#include <linux/routerboot.h>
#include <linux/gpio.h>
#include <linux/version.h>

#include <asm/mach-ath79/ath79.h>
#include <asm/mach-ath79/ar71xx_regs.h>
#include "common.h"
#include "dev-ap9x-pci.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-eth.h"
#include "dev-m25p80.h"
#include "dev-nfc.h"
#include "dev-wmac.h"
#include "dev-usb.h"
#include "machtypes.h"
#include "routerboot.h"
#include <linux/ar8216_platform.h>

#define SXT_GPIO_LED_POWER      1
#define SXT_GPIO_LED_USER       12
#define SXT_GPIO_LED_1          5
#define SXT_GPIO_LED_2          6
#define SXT_GPIO_LED_3          7
#define SXT_GPIO_LED_4          8
#define SXT_GPIO_LED_5          9
#define SXT_GPIO_USB_POWER_OFF  3

#define SXT_KEYS_POLL_INTERVAL 20
#define SXT_KEYS_DEBOUNCE_INTERVAL (3 * SXT_KEYS_POLL_INTERVAL)

static struct mtd_partition rbsxt_nand_partitions[] = {
	{
		.name   = "booter",
		.offset = 0,
		.size   = (256 * 1024),
		.mask_flags = MTD_WRITEABLE,
	},
	{
		.name   = "kernel",
		.offset = (256 * 1024),
		.size   = (4 * 1024 * 1024) - (256 * 1024),
	},
	{
		.name   = "ubi",
		.offset = MTDPART_OFS_NXTBLK,
		.size   = MTDPART_SIZ_FULL,
	},
};


static struct gpio_led rbsxt_leds_gpio[] __initdata = {
	{
		.name		=	"rb:green:user",
		.gpio		=	SXT_GPIO_LED_USER,
		.active_low	=	1,
	},
	{
		.name		=	"rb:green:led1",
		.gpio		=	SXT_GPIO_LED_1,
		.active_low	=	1,
	},
	{
		.name		=	"rb:green:led2",
		.gpio		=	SXT_GPIO_LED_2,
		.active_low	=	1,
	},
	{
		.name		=	"rb:green:led3",
		.gpio		=	SXT_GPIO_LED_3,
		.active_low	=	1,
	},
	{
		.name		=	"rb:green:led4",
		.gpio		=	SXT_GPIO_LED_4,
		.active_low	=	1,
	},
	{
		.name		=	"rb:green:led5",
		.gpio		=	SXT_GPIO_LED_5,
		.active_low	=	1,
	},
	{
		.name		=	"rb:green:power",
		.gpio		=	SXT_GPIO_LED_POWER,
		.default_state	=	LEDS_GPIO_DEFSTATE_KEEP,
	},
};

static int __init rbsxt_rbinfo_init(void)
{
	const struct rb_info *info;

	info = rb_init_info((void *)(KSEG1ADDR(AR71XX_SPI_BASE)), 0x10000);
	if (!info)
		return -EINVAL;
	return 0;

}
